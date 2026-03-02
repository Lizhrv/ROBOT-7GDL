#include <WiFiS3.h> 
#include <Servo.h> 

// ****************************************************
// === 1. CONFIGURACIÓN INICIAL ===
// ****************************************************

// --- RED WIFI ---
char ssid[] = "Totalplay-2.4G-0338";
char pass[] = "JWyK3SFzKdhBChR8";
int status = WL_IDLE_STATUS;
WiFiServer server(80);
volatile bool rutinaActiva = false;
int tipoRutina = 0; // 1 para Rutina 1, 2 para Rutina 2

// --- SERVOS DEL BRAZO (D2-D8) ---
const int NUM_SERVOS = 7;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8};
Servo myservo[NUM_SERVOS]; 

// --- COMPRESOR (AHORA EN A2) ---
const int PIN_COMPRESOR = A2; // Pin analógico 2 usado como digital

// --- CARRO (D10-D13) ---
const int IZQ_IN1 = 10; const int IZQ_IN2 = 11; 
const int DER_IN3 = 12; const int DER_IN4 = 13; 

// ****************************************************
// === 2. FUNCIONES DE MOVIMIENTO ===
// ****************************************************

void moverSuave(int index, int anguloFinal) {
  if (!myservo[index].attached()) myservo[index].attach(servoPins[index]);
  int angActual = myservo[index].read();
  int paso = (angActual < anguloFinal) ? 1 : -1;
  while (angActual != anguloFinal) {
    angActual += paso;
    myservo[index].write(angActual);
    delay(15);
  }
}

void detenerCarro() {
  digitalWrite(IZQ_IN1, LOW); digitalWrite(IZQ_IN2, LOW);
  digitalWrite(DER_IN3, LOW); digitalWrite(DER_IN4, LOW);
}

void moverAdelante() {
  digitalWrite(IZQ_IN1, HIGH); digitalWrite(IZQ_IN2, LOW);
  digitalWrite(DER_IN3, HIGH); digitalWrite(DER_IN4, LOW);
}

void moverAtras() {
  digitalWrite(IZQ_IN1, LOW); digitalWrite(IZQ_IN2, HIGH);
  digitalWrite(DER_IN3, LOW); digitalWrite(DER_IN4, HIGH);
}

void configurarPosicionReposo() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    moverSuave(i, 90);
    myservo[i].detach(); // Silenciar motores para evitar zumbido
  }
  digitalWrite(PIN_COMPRESOR, LOW); 
  detenerCarro();
  Serial.println("Sistema en Reposo.");
}

// ****************************************************
// === 3. TUS DOS RUTINAS ORIGINALES ===
// ****************************************************

void ejecutarRutina1() {
  Serial.println("INICIO: Rutina 1.");
  moverAdelante(); delay(2000); detenerCarro(); delay(500);
  
  // Posicion Agarre
  moverSuave(0, 45); moverSuave(1, 130); moverSuave(2, 50);  
  moverSuave(3, 40); moverSuave(4, 140); moverSuave(5, 90); moverSuave(6, 160);
  
  digitalWrite(PIN_COMPRESOR, HIGH); delay(1500); // Succión ON
  
  // Posicion Transporte
  moverSuave(1, 90); moverSuave(2, 90); moverSuave(3, 90); moverSuave(4, 90); moverSuave(5, 90);
  
  moverAtras(); delay(3000); detenerCarro();
  
  // Posicion Liberacion
  moverSuave(0, 135); moverSuave(1, 130); moverSuave(2, 50); moverSuave(3, 50); moverSuave(4, 130); moverSuave(5, 90);
  
  digitalWrite(PIN_COMPRESOR, LOW); delay(1500); // Succión OFF
  configurarPosicionReposo(); 
  rutinaActiva = false;
}

void ejecutarRutina2() {
  Serial.println("INICIO: Rutina 2.");
  moverSuave(0, 160); moverSuave(1, 40); delay(1000);
  moverSuave(2, 150); 
  moverAdelante(); delay(1000); detenerCarro();
  
  digitalWrite(PIN_COMPRESOR, HIGH); delay(500); 
  digitalWrite(PIN_COMPRESOR, LOW);
  
  configurarPosicionReposo(); 
  rutinaActiva = false;
}

// ****************************************************
// === 4. SETUP Y LOOP ===
// ****************************************************

void setup() {
  Serial.begin(115200);
  pinMode(PIN_COMPRESOR, OUTPUT);
  digitalWrite(PIN_COMPRESOR, LOW);
  
  pinMode(IZQ_IN1, OUTPUT); pinMode(IZQ_IN2, OUTPUT);
  pinMode(DER_IN3, OUTPUT); pinMode(DER_IN4, OUTPUT);

  configurarPosicionReposo(); 
  
  while (status != WL_CONNECTED) {
    Serial.print("Intentando conectar a: "); Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  server.begin();
  Serial.print("IP del Robot: "); Serial.println(WiFi.localIP());
}

void loop() {
  // Manejo de rutinas
  if (rutinaActiva) {
    if (tipoRutina == 1) ejecutarRutina1();
    else if (tipoRutina == 2) ejecutarRutina2();
    rutinaActiva = false;
    return;
  }
  
  // Servidor Web
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("GET /start") != -1) {
      tipoRutina = 1;
      rutinaActiva = true;
    } 
    else if (request.indexOf("GET /rutina2") != -1) {
      tipoRutina = 2;
      rutinaActiva = true;
    } 
    else if (request.indexOf("GET /reset") != -1) {
      rutinaActiva = false;
      configurarPosicionReposo();
    } 

    client.println("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nOK");
    client.stop();
  }
}