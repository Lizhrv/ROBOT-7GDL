#include <WiFiS3.h> 
#include <Servo.h> 

// ****************************************************
// === 1. CONFIGURACIÓN INICIAL ===
// ****************************************************

// --- RED WIFI ---
char ssid[] = "Totalplay-2.4G-0338";     //
char pass[] = "JWyK3SFzKdhBChR8";       //
int status = WL_IDLE_STATUS;            //
WiFiServer server(80);                  //
volatile bool rutinaActiva = false;     //

// --- SERVOS DEL BRAZO (D2-D8) ---
const int NUM_SERVOS = 7;               //
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8}; //
Servo myservo[NUM_SERVOS];              //

// --- COMPRESOR (D9) ---
Servo compresorRelay;                   //
const int PIN_COMPRESOR = 9;            //
const int COMPRESOR_ON = 180;           //
const int COMPRESOR_OFF = 0;            //

// --- CARRO (D10-D13) ---
const int IZQ_IN1 = 10;                 //
const int IZQ_IN2 = 11;                 //
const int DER_IN3 = 12;                 //
const int DER_IN4 = 13;                 //

// ****************************************************
// === 2. FUNCIONES DE MOVIMIENTO SUAVE (CORRECCIÓN DE VIBRACIÓN) ===
// ****************************************************

// Mueve el servo grado a grado para evitar tirones y ruido eléctrico
void moverSuave(int index, int anguloFinal) {
  int anguloActual = myservo[index].read();
  if (anguloActual < anguloFinal) {
    for (int p = anguloActual; p <= anguloFinal; p++) {
      myservo[index].write(p);
      delay(15); // Velocidad del movimiento
    }
  } else {
    for (int p = anguloActual; p >= anguloFinal; p--) {
      myservo[index].write(p);
      delay(15);
    }
  }
}

// ****************************************************
// === 3. FUNCIONES DE MOVIMIENTO BASE ===
// ****************************************************

void detenerCarro() {
  digitalWrite(IZQ_IN1, LOW);           //
  digitalWrite(IZQ_IN2, LOW);           //
  digitalWrite(DER_IN3, LOW);           //
  digitalWrite(DER_IN4, LOW);           //
}

void moverAdelante() {
  digitalWrite(IZQ_IN1, HIGH);          //
  digitalWrite(IZQ_IN2, LOW);           //
  digitalWrite(DER_IN3, HIGH);          //
  digitalWrite(DER_IN4, LOW);           //
}

void moverAtras() {
  digitalWrite(IZQ_IN1, LOW);           //
  digitalWrite(IZQ_IN2, HIGH);          //
  digitalWrite(DER_IN3, LOW);           //
  digitalWrite(DER_IN4, HIGH);          //
}

// --- Posiciones del Brazo con Suavizado ---
void configurarPosicionReposo() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    moverSuave(i, 90);                  // Corregido: movimiento suave
  }
  compresorRelay.write(COMPRESOR_OFF);  //
  detenerCarro();                       //
  delay(500);
  Serial.println("Robot en reposo.");
}

void posicionAgarre() {
  moverSuave(0, 45);  moverSuave(1, 130); moverSuave(2, 50);  
  moverSuave(3, 40);  moverSuave(4, 140); moverSuave(5, 90); 
  moverSuave(6, 160);                   //
  delay(500);
}

void posicionTransporte() {
  moverSuave(1, 90);  moverSuave(2, 90); 
  moverSuave(3, 90);  moverSuave(4, 90); 
  moverSuave(5, 90);                    //
  delay(500);
}

void posicionLiberacion() {
  moverSuave(0, 135); moverSuave(1, 130); moverSuave(2, 50);  
  moverSuave(3, 50);  moverSuave(4, 130); moverSuave(5, 90); 
  delay(500);                           //
}

// ****************************************************
// === 4. RUTINAS DE EJECUCIÓN ===
// ****************************************************

void ejecutarRutina() {
  Serial.println("INICIO: Rutina 1.");
  moverAdelante(); delay(2000); detenerCarro(); delay(500); //
  posicionAgarre(); 
  compresorRelay.write(COMPRESOR_ON); delay(1500);          //
  posicionTransporte(); 
  moverAtras(); delay(3000); detenerCarro();                //
  posicionLiberacion(); 
  compresorRelay.write(COMPRESOR_OFF); delay(1500);         //
  configurarPosicionReposo(); 
  rutinaActiva = false;                                     //
}

void ejecutarRutina2() {
  Serial.println("INICIO: Rutina 2.");
  moverSuave(0, 160); moverSuave(1, 40); delay(1000);       //
  moverSuave(2, 150); moverAdelante(); delay(1000); detenerCarro(); //
  compresorRelay.write(COMPRESOR_ON); delay(500); 
  compresorRelay.write(COMPRESOR_OFF);                      //
  configurarPosicionReposo(); 
  rutinaActiva = false;                                     //
}

// ****************************************************
// === 5. SETUP Y LOOP ===
// ****************************************************

void setup() {
  Serial.begin(115200); // Velocidad recomendada para R4
  
  for (int i = 0; i < NUM_SERVOS; i++) {
    myservo[i].attach(servoPins[i]);    //
  }
  compresorRelay.attach(PIN_COMPRESOR); //
  pinMode(IZQ_IN1, OUTPUT); pinMode(IZQ_IN2, OUTPUT); //
  pinMode(DER_IN3, OUTPUT); pinMode(DER_IN4, OUTPUT); //

  configurarPosicionReposo(); 
  
  while (status != WL_CONNECTED) {      //
    Serial.print("Conectando a: "); Serial.println(ssid);
    status = WiFi.begin(ssid, pass);    //
    delay(5000); 
  }
  Serial.print("IP: "); Serial.println(WiFi.localIP()); //
  server.begin();                       //
}

void loop() {
  if (rutinaActiva) {                   //
    ejecutarRutina(); 
    return; 
  }
  
  WiFiClient client = server.available(); //
  if (client) {
    String requestLine = client.readStringUntil('\r'); //
    client.flush();

    if (requestLine.indexOf("GET /start") != -1) {
      rutinaActiva = true;              //
    } 
    else if (requestLine.indexOf("GET /rutina2") != -1) {
      ejecutarRutina2();                //
    } 
    else if (requestLine.indexOf("GET /reset") != -1) {
      rutinaActiva = false;             //
      configurarPosicionReposo();       //
    } 

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("OK");
    client.stop();                      //
  }
}