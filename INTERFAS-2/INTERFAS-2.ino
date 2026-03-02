#include <WiFiS3.h> 
#include <Servo.h> 

// === 1. CONFIGURACIÓN ===
char ssid[] = "Totalplay-2.4G-0338";
char pass[] = "JWyK3SFzKdhBChR8";
int status = WL_IDLE_STATUS;
WiFiServer server(80);
volatile bool rutinaActiva = false;

const int NUM_SERVOS = 7;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8};
Servo myservo[NUM_SERVOS]; 

const int PIN_COMPRESOR = 9;
Servo compresorRelay; 

const int IZQ_IN1 = 10, IZQ_IN2 = 11, DER_IN3 = 12, DER_IN4 = 13;

// === 2. MOVIMIENTO SUAVE ===
void moverSuave(int index, int anguloFinal) {
  // Si el servo estaba "dormido" (detached), lo despertamos
  if (!myservo[index].attached()) {
    myservo[index].attach(servoPins[index]);
  }

  int anguloActual = myservo[index].read();
  if (anguloActual < anguloFinal) {
    for (int p = anguloActual; p <= anguloFinal; p++) {
      myservo[index].write(p);
      delay(15);
    }
  } else {
    for (int p = anguloActual; p >= anguloFinal; p--) {
      myservo[index].write(p);
      delay(15);
    }
  }
}

// === 3. POSICIONES Y SILENCIADOR ===
void configurarPosicionReposo() {
  Serial.println("Moviendo a reposo y silenciando...");
  for (int i = 0; i < NUM_SERVOS; i++) {
    moverSuave(i, 90);
    delay(100); 
    myservo[i].detach(); // <--- AQUÍ SE CALLAN LOS MOTORES
  }
  compresorRelay.write(0);
  detenerCarro();
}

void detenerCarro() {
  digitalWrite(10, 0); digitalWrite(11, 0); digitalWrite(12, 0); digitalWrite(13, 0);
}

void moverAdelante() {
  digitalWrite(10, 1); digitalWrite(11, 0); digitalWrite(12, 1); digitalWrite(13, 0);
}

void moverAtras() {
  digitalWrite(10, 0); digitalWrite(11, 1); digitalWrite(12, 0); digitalWrite(13, 1);
}

// === 4. RUTINAS ===
void ejecutarRutina() {
  moverAdelante(); delay(2000); detenerCarro();
  
  // Agarre
  moverSuave(0, 45); moverSuave(1, 130); moverSuave(2, 50);
  compresorRelay.write(180); delay(1500);

  // Transporte
  moverSuave(1, 90); moverSuave(2, 90);
  moverAtras(); delay(3000); detenerCarro();

  configurarPosicionReposo(); 
  rutinaActiva = false;
}

// === 5. SETUP Y LOOP ===
void setup() {
  Serial.begin(115200);
  compresorRelay.attach(PIN_COMPRESOR);
  pinMode(10, OUTPUT); pinMode(11, OUTPUT); pinMode(12, OUTPUT); pinMode(13, OUTPUT);

  configurarPosicionReposo(); 

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  server.begin();
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void loop() {
  if (rutinaActiva) {
    ejecutarRutina();
    return;
  }

  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    if (req.indexOf("/start") != -1) rutinaActiva = true;
    if (req.indexOf("/reset") != -1) {
       rutinaActiva = false;
       configurarPosicionReposo();
    }
    client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nOK");
    client.stop();
  }
}