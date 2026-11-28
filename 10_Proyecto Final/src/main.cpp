#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

const char* AP_SSID = "CONTROL_PID_ESP32";
const char* AP_PASSWORD = "PID123456";
WebServer server(80);

#define PIN_DS18B20 32
#define PIN_SERVO 13
#define PIN_RELE_VENTILADOR 23
const byte RELE_ON = LOW;
const byte RELE_OFF = HIGH;

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensor(&oneWire);
Servo servo;
const int SERVO_MIN_ANG = 0;
const int SERVO_MAX_ANG = 180;
const float CONTROL_MIN = 0.0f;
const float CONTROL_MAX = 180.0f;

float setpoint = 40.0f;
float Kp = 1.0f, Ki = 0.0f, Kd = 0.0f;
float temperatura = 0.0f, error = 0.0f;
float control = 0.0f, anguloServo = SERVO_MIN_ANG;
float terminoIntegral = 0.0f, terminoDerivativo = 0.0f;
float errorAnterior = 0.0f, temperaturaAnterior = 0.0f;
bool pidInicializado = false;
const float TAU_DERIVATIVA = 1.0f;
bool sensorValido = false;

const byte NUM_LECTURAS = 5;
float lecturasTemperatura[NUM_LECTURAS] = {0};
byte indiceLectura = 0, lecturasLlenas = 0;
float sumaTemperaturas = 0.0f;

const float DIF_ENCENDIDO_VENT = 1.0f;
const float DIF_APAGADO_VENT = 0.1f;
bool ventiladorEncendido = false;

unsigned long tiempoSensor = 0;
const unsigned long INTERVALO_SENSOR = 500;

const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="es"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#101827"><title>Control PID</title>
<style>
*{box-sizing:border-box}body{margin:0;background:#0b1220;color:#e5edf7;font-family:Arial,sans-serif}
header{padding:20px 16px;background:#111c2e;border-bottom:1px solid #26364d}
h1{margin:0;color:#38bdf8;font-size:22px}header p{margin:6px 0 0;color:#94a3b8;font-size:12px;letter-spacing:1px}
main{max-width:650px;margin:auto;padding:14px}.estado{padding:12px;border-radius:10px;background:#172338;margin-bottom:12px;display:flex;justify-content:space-between;font-size:13px}
.punto{width:9px;height:9px;border-radius:50%;display:inline-block;background:#f59e0b;margin-right:7px}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}.card,.seccion{background:#172338;border:1px solid #26364d;border-radius:13px;padding:15px}
.nombre{font-size:11px;color:#9fb0c5;letter-spacing:.7px}.valor{font-size:27px;font-weight:bold;margin-top:8px}
.cyan{color:#38bdf8}.amarillo{color:#fbbf24}.rojo{color:#fb7185}.verde{color:#4ade80}
.seccion{margin-top:15px}h2{font-size:15px;margin:0 0 13px;color:#dbeafe}
label{display:block;color:#a9b8ca;font-size:12px;margin:10px 0 5px}
input{width:100%;padding:12px;border:1px solid #354861;border-radius:8px;background:#0d1728;color:white;font-size:17px}
button{border:0;border-radius:9px;padding:14px;font-size:14px;font-weight:bold;color:white;width:100%;margin-top:14px;background:#2563eb;cursor:pointer}
.btn-mini{width:auto;margin:0;padding:6px 11px;font-size:11px;background:#0d1728;border:1px solid #354861;color:#cbd5e1}
.graf-head{display:flex;justify-content:space-between;align-items:center;gap:8px;margin-bottom:10px;flex-wrap:wrap}
.leyenda{display:flex;gap:12px;font-size:12px;color:#9fb0c5}
.dot{display:inline-block;width:9px;height:9px;border-radius:50%;margin-right:5px}
canvas{width:100%;height:220px;display:block;background:#0b1220;border:1px solid #26364d;border-radius:8px}
.act{display:flex;justify-content:space-between;padding:10px 0;border-bottom:1px solid #293a50;font-size:13px}.act:last-child{border:0}
small{color:#94a3b8}@media(min-width:500px){main{padding:22px}.valor{font-size:32px}}
</style></head><body>
<header><h1>PID TEMPERATURE CONTROL</h1><p>MONITOREO Y CONFIGURACION DEL SISTEMA</p></header><main>
<div class="estado"><span><i class="punto" id="punto"></i><span id="conexion">Conectando...</span></span><span id="sensor">Sensor: --</span></div>
<div class="grid">
<div class="card"><div class="nombre">TEMPERATURA ACTUAL (VP)</div><div class="valor cyan" id="vp">-- °C</div></div>
<div class="card"><div class="nombre">TEMPERATURA DESEADA (SP)</div><div class="valor amarillo" id="sp">-- °C</div></div>
<div class="card"><div class="nombre">ERROR</div><div class="valor rojo" id="err">-- °C</div></div>
<div class="card"><div class="nombre">CONTROL (ANGULO)</div><div class="valor verde" id="salida">-- °</div></div>
</div>

<div class="seccion">
<div class="graf-head">
<div class="leyenda">
<span><i class="dot" style="background:#38bdf8"></i>Actual (VP)</span>
<span><i class="dot" style="background:#fbbf24"></i>Setpoint (SP)</span>
</div>
<button class="btn-mini" onclick="limpiarGrafica()">Limpiar</button>
</div>
<canvas id="grafica"></canvas>
</div>

<div class="seccion"><h2>AJUSTE DE PARAMETROS PID</h2>
<label for="setpoint">Setpoint (°C)</label><input id="setpoint" type="number" min="0" max="100" step="0.1">
<label for="kp">Ganancia proporcional (Kp)</label><input id="kp" type="number" min="0" step="0.001">
<label for="ki">Ganancia integral (Ki)</label><input id="ki" type="number" min="0" step="0.001">
<label for="kd">Ganancia derivativa (Kd)</label><input id="kd" type="number" min="0" step="0.001">
<button onclick="aplicar()">APLICAR PARAMETROS</button><p id="mensaje"><small>Modifica los valores y pulsa aplicar.</small></p></div>

<div class="seccion"><h2>ESTADO DE ACTUADORES</h2>
<div class="act"><span>Servomotor / dimmer</span><b id="servo">-- °</b></div>
<div class="act"><span>Ventilador</span><b id="vent">--</b></div></div>
<p style="text-align:center;color:#64748b;font-size:11px;margin:18px 0">ESP32 · CONTROL PID DISCRETO</p>
</main><script>
let cargado=false;
const MAX_PUNTOS=120;
let histT=[], histSP=[];
const canvas=document.getElementById('grafica'), ctx=canvas.getContext('2d');

function limpiarGrafica(){histT=[];histSP=[];dibujarGrafica();}

function dibujarGrafica(){
 const dpr=window.devicePixelRatio||1, r=canvas.getBoundingClientRect();
 canvas.width=Math.max(260,Math.floor(r.width*dpr));
 canvas.height=Math.max(180,Math.floor(r.height*dpr));
 ctx.setTransform(dpr,0,0,dpr,0,0);
 const W=r.width, H=r.height, pL=40, pR=12, pT=12, pB=20, gW=W-pL-pR, gH=H-pT-pB;
 ctx.clearRect(0,0,W,H);

 let minY=20, maxY=50;
 if(histT.length>0){
  const todos=histT.concat(histSP);
  minY=Math.min(...todos)-2; maxY=Math.max(...todos)+2;
  if(maxY-minY<10){const m=(maxY+minY)/2; minY=m-5; maxY=m+5;}
  minY=Math.floor(minY); maxY=Math.ceil(maxY);
 }

 ctx.strokeStyle='#1e293b'; ctx.fillStyle='#94a3b8'; ctx.font='11px Arial,sans-serif'; ctx.lineWidth=1;
 for(let i=0;i<=4;i++){
  const y=pT+(gH*i)/4, val=maxY-((maxY-minY)*i)/4;
  ctx.beginPath(); ctx.moveTo(pL,y); ctx.lineTo(W-pR,y); ctx.stroke();
  ctx.fillText(val.toFixed(1),4,y+4);
 }

 if(histT.length<2){
  ctx.fillStyle='#64748b'; ctx.fillText('Esperando datos...',pL+10,pT+20); return;
 }

 const mapX=i=>pL+(gW*i)/(MAX_PUNTOS-1);
 const mapY=v=>pT+gH-((v-minY)/(maxY-minY))*gH;

 ctx.save(); ctx.strokeStyle='#fbbf24'; ctx.lineWidth=2; ctx.setLineDash([6,4]);
 ctx.beginPath();
 histSP.forEach((v,i)=>{const x=mapX(i),y=mapY(v); if(i===0)ctx.moveTo(x,y); else ctx.lineTo(x,y);});
 ctx.stroke(); ctx.restore();

 ctx.strokeStyle='#38bdf8'; ctx.lineWidth=2.4;
 ctx.beginPath();
 histT.forEach((v,i)=>{const x=mapX(i),y=mapY(v); if(i===0)ctx.moveTo(x,y); else ctx.lineTo(x,y);});
 ctx.stroke();
}

async function actualizar(){try{
 const r=await fetch('/api/status',{cache:'no-store'});if(!r.ok)throw Error('HTTP');const d=await r.json();
 document.getElementById('conexion').textContent='ESP32 CONECTADO';document.getElementById('punto').style.background='#4ade80';
 document.getElementById('sensor').textContent=d.sensor_ok?'Sensor: OK':'Sensor: ERROR';
 document.getElementById('vp').textContent=d.sensor_ok?Number(d.temperature).toFixed(2)+' °C':'ERR';
 document.getElementById('sp').textContent=Number(d.setpoint).toFixed(2)+' °C';
 document.getElementById('err').textContent=d.sensor_ok?Number(d.error).toFixed(2)+' °C':'--';
 document.getElementById('salida').textContent=Number(d.servo).toFixed(0)+' °';
 document.getElementById('servo').textContent=Number(d.servo).toFixed(0)+' °';
 document.getElementById('vent').textContent=d.fan?'ENCENDIDO':'APAGADO';
 if(d.sensor_ok){
  histT.push(Number(d.temperature)); histSP.push(Number(d.setpoint));
  if(histT.length>MAX_PUNTOS){histT.shift();histSP.shift();}
  dibujarGrafica();
 }
 if(!cargado){['setpoint','kp','ki','kd'].forEach(k=>document.getElementById(k).value=d[k]);cargado=true;}
 }catch(e){document.getElementById('conexion').textContent='SIN CONEXION';document.getElementById('punto').style.background='#ef4444';}
 finally{setTimeout(actualizar,500);}}

async function aplicar(){const d={setpoint:parseFloat(document.getElementById('setpoint').value),kp:parseFloat(document.getElementById('kp').value),ki:parseFloat(document.getElementById('ki').value),kd:parseFloat(document.getElementById('kd').value)};
 if(Object.values(d).some(v=>!Number.isFinite(v)||v<0)||d.setpoint>100){document.getElementById('mensaje').textContent='Verifica los valores. SP: 0 a 100 °C; ganancias no negativas.';return;}
 try{const r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(d)});const j=await r.json();document.getElementById('mensaje').textContent=j.message||'Parametros enviados.';}
 catch(e){document.getElementById('mensaje').textContent='No se pudieron enviar los parametros.';}}

window.addEventListener('resize',dibujarGrafica);
dibujarGrafica();
actualizar();
</script></body></html>
)rawliteral";

void actualizarSalidaPID() {
  if (ventiladorEncendido) {
    control = CONTROL_MIN; anguloServo = SERVO_MIN_ANG; servo.write(SERVO_MIN_ANG); return;
  }
  float salidaSinSaturar = Kp * error + terminoIntegral + terminoDerivativo;
  control = constrain(salidaSinSaturar, CONTROL_MIN, CONTROL_MAX);
  anguloServo = constrain((float)SERVO_MIN_ANG + control, (float)SERVO_MIN_ANG, (float)SERVO_MAX_ANG);
  servo.write((int)roundf(anguloServo));
}

void reiniciarEstadosPID() {
  terminoIntegral = 0; terminoDerivativo = 0; errorAnterior = 0; temperaturaAnterior = 0; pidInicializado = false;
}

void establecerVentilador(bool encender) {
  if (ventiladorEncendido == encender) return;
  ventiladorEncendido = encender;
  digitalWrite(PIN_RELE_VENTILADOR, ventiladorEncendido ? RELE_ON : RELE_OFF);
  Serial.println(ventiladorEncendido ? "VENTILADOR: ENCENDIDO" : "VENTILADOR: APAGADO");
}

void controlarVentilador() {
  if (!sensorValido) {
    establecerVentilador(true); reiniciarEstadosPID(); control = CONTROL_MIN;
    anguloServo = SERVO_MIN_ANG; servo.write(SERVO_MIN_ANG); return;
  }
  float temperaturaEncendido = setpoint + DIF_ENCENDIDO_VENT;
  float temperaturaApagado = setpoint + DIF_APAGADO_VENT;
  bool estabaEncendido = ventiladorEncendido;
  if (!ventiladorEncendido && temperatura >= temperaturaEncendido) establecerVentilador(true);
  else if (ventiladorEncendido && temperatura <= temperaturaApagado) establecerVentilador(false);
  if (ventiladorEncendido) {
    terminoIntegral = 0; terminoDerivativo = 0; errorAnterior = setpoint - temperatura;
    temperaturaAnterior = temperatura; pidInicializado = true; error = setpoint - temperatura; actualizarSalidaPID();
  } else if (estabaEncendido && !ventiladorEncendido) {
    terminoIntegral = 0; terminoDerivativo = 0; error = setpoint - temperatura;
    errorAnterior = error; temperaturaAnterior = temperatura; pidInicializado = true; actualizarSalidaPID();
  }
}

float calcularPromedio(float nuevaLectura) {
  if (lecturasLlenas == NUM_LECTURAS) sumaTemperaturas -= lecturasTemperatura[indiceLectura];
  else lecturasLlenas++;
  lecturasTemperatura[indiceLectura] = nuevaLectura; sumaTemperaturas += nuevaLectura;
  indiceLectura++; if (indiceLectura >= NUM_LECTURAS) indiceLectura = 0;
  return sumaTemperaturas / lecturasLlenas;
}

void reiniciarPromedio() {
  sumaTemperaturas = 0; indiceLectura = 0; lecturasLlenas = 0;
  for (byte i = 0; i < NUM_LECTURAS; i++) lecturasTemperatura[i] = 0;
}

void calcularControlPID(float Ts) {
  error = setpoint - temperatura;
  if (ventiladorEncendido) {
    terminoIntegral = 0; terminoDerivativo = 0; errorAnterior = error;
    temperaturaAnterior = temperatura; pidInicializado = true; actualizarSalidaPID(); return;
  }
  if (!pidInicializado) {
    errorAnterior = error; temperaturaAnterior = temperatura; terminoIntegral = 0;
    terminoDerivativo = 0; pidInicializado = true; actualizarSalidaPID(); return;
  }
  if (Ts <= 0 || !isfinite(Ts)) { actualizarSalidaPID(); return; }
  float P = Kp * error;
  float incrementoIntegral = Ki * (Ts / 2.0f) * (error + errorAnterior);
  float integralPropuesta = terminoIntegral + incrementoIntegral;
  float denominadorD = 2.0f * TAU_DERIVATIVA + Ts;
  float alphaD = (2.0f * TAU_DERIVATIVA - Ts) / denominadorD;
  float betaD = (2.0f * Kd) / denominadorD;
  float derivadaPropuesta = alphaD * terminoDerivativo - betaD * (temperatura - temperaturaAnterior);
  float salidaPropuesta = P + integralPropuesta + derivadaPropuesta;
  bool integralEmpeoraSaturacion =
    (salidaPropuesta > CONTROL_MAX && incrementoIntegral > 0) ||
    (salidaPropuesta < CONTROL_MIN && incrementoIntegral < 0);
  if (!integralEmpeoraSaturacion) terminoIntegral = integralPropuesta;
  terminoDerivativo = derivadaPropuesta; errorAnterior = error; temperaturaAnterior = temperatura;
  actualizarSalidaPID();
}

void leerTemperatura(float Ts) {
  float nuevaTemperatura = sensor.getTempCByIndex(0);
  if (nuevaTemperatura == DEVICE_DISCONNECTED_C || nuevaTemperatura < -55 || nuevaTemperatura > 125) {
    sensorValido = false; control = CONTROL_MIN; error = 0; anguloServo = SERVO_MIN_ANG;
    servo.write(SERVO_MIN_ANG); reiniciarPromedio(); reiniciarEstadosPID(); establecerVentilador(true);
    Serial.println("ERROR: SENSOR DESCONECTADO"); return;
  }
  sensorValido = true; temperatura = calcularPromedio(nuevaTemperatura);
  calcularControlPID(Ts); controlarVentilador();
  Serial.printf("T: %.2f C | SP: %.2f | E: %.2f | Kp: %.3f | Ki: %.5f | Kd: %.5f | Control: %.2f | Servo: %.1f | Ventilador: %s\n",
    temperatura,setpoint,error,Kp,Ki,Kd,control,anguloServo,ventiladorEncendido?"ON":"OFF");
}

void enviarEstado() {
  String json = "{";
  json += "\"temperature\":" + String(temperatura,2) + ",";
  json += "\"setpoint\":" + String(setpoint,2) + ",";
  json += "\"error\":" + String(error,2) + ",";
  json += "\"output\":" + String(control,2) + ",";
  json += "\"servo\":" + String(anguloServo,1) + ",";
  json += "\"kp\":" + String(Kp,4) + ",";
  json += "\"ki\":" + String(Ki,4) + ",";
  json += "\"kd\":" + String(Kd,4) + ",";
  json += "\"fan\":" + String(ventiladorEncendido ? "true" : "false") + ",";
  json += "\"sensor_ok\":" + String(sensorValido ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

bool obtenerNumeroJSON(const String& body, const char* clave, float& valor) {
  String buscada = String("\"") + clave + "\"";
  int pos = body.indexOf(buscada); if (pos < 0) return false;
  int dosPuntos = body.indexOf(':', pos); if (dosPuntos < 0) return false;
  int inicio = dosPuntos + 1;
  while (inicio < (int)body.length() && (body[inicio] == ' ' || body[inicio] == '\t')) inicio++;
  int inicioFin = inicio;
  while (inicioFin < (int)body.length() && (isDigit(body[inicioFin]) || body[inicioFin] == '.' || body[inicioFin] == '-' || body[inicioFin] == '+' || body[inicioFin] == 'e' || body[inicioFin] == 'E')) inicioFin++;
  if (inicioFin == inicio) return false;
  valor = body.substring(inicio, inicioFin).toFloat();
  return isfinite(valor);
}

void recibirConfiguracion() {
  if (!server.hasArg("plain")) { server.send(400,"application/json","{\"message\":\"Falta JSON\"}"); return; }
  String body = server.arg("plain");
  float nuevoSP, nuevoKp, nuevoKi, nuevoKd;
  if (!obtenerNumeroJSON(body,"setpoint",nuevoSP) || !obtenerNumeroJSON(body,"kp",nuevoKp) ||
      !obtenerNumeroJSON(body,"ki",nuevoKi) || !obtenerNumeroJSON(body,"kd",nuevoKd) ||
      nuevoSP < 0 || nuevoSP > 100 || nuevoKp < 0 || nuevoKi < 0 || nuevoKd < 0) {
    server.send(400,"application/json","{\"message\":\"Valores invalidos. SP: 0 a 100; ganancias no negativas.\"}"); return;
  }
  setpoint = nuevoSP; Kp = nuevoKp; Ki = nuevoKi; Kd = nuevoKd;
  if (sensorValido) {
    error = setpoint - temperatura; errorAnterior = error; temperaturaAnterior = temperatura;
    terminoDerivativo = 0; pidInicializado = true; actualizarSalidaPID(); controlarVentilador();
  }
  server.send(200,"application/json","{\"message\":\"Parametros PID actualizados correctamente\"}");
}

void setup() {
  Serial.begin(115200);
  digitalWrite(PIN_RELE_VENTILADOR, RELE_OFF);
  pinMode(PIN_RELE_VENTILADOR, OUTPUT);

  sensor.begin(); sensor.setResolution(11); sensor.setWaitForConversion(false);
  sensor.requestTemperatures(); tiempoSensor = millis();

  servo.setPeriodHertz(50); servo.attach(PIN_SERVO,500,2400); servo.write(SERVO_MIN_ANG);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  delay(300);

  Serial.println("\n=== CONTROL PID - INTERFAZ WEB ===");
  Serial.print("Red WiFi: "); Serial.println(AP_SSID);
  Serial.print("Contrasena: "); Serial.println(AP_PASSWORD);
  Serial.print("Abre en el celular: http://"); Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](){ server.send_P(200,"text/html; charset=utf-8",PAGINA); });
  server.on("/api/status", HTTP_GET, enviarEstado);
  server.on("/api/config", HTTP_POST, recibirConfiguracion);
  server.onNotFound([](){ server.send(404,"text/plain","Ruta no encontrada"); });
  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
  server.handleClient();
  unsigned long ahora = millis();
  if (ahora - tiempoSensor >= INTERVALO_SENSOR) {
    float Ts = (ahora - tiempoSensor) / 1000.0f;
    tiempoSensor = ahora;
    leerTemperatura(Ts);
    sensor.requestTemperatures();
  }
}