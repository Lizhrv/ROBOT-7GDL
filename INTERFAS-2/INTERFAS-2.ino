#include <WiFiS3.h>
#include <Servo.h>

// --- CONFIGURACIÓN DE RED ---
char ssid[] = "A55 de Arantxa";
char pass[] = "hola2008";
WiFiServer server(80);
volatile bool rutinaActiva = false;

// --- OBJETOS DE SERVO (Tu estructura original) ---
Servo myservo0; Servo myservo1; Servo myservo2;
Servo myservo3; Servo myservo4; Servo myservo5;

const int PIN_COMPRESOR = A2;

void configurarPosicionReposo() {
  digitalWrite(PIN_COMPRESOR, HIGH); // Compresor OFF
  myservo0.write(90); myservo1.write(90); myservo2.write(90);
  myservo3.write(90); myservo4.write(90); myservo5.write(90);
}

void ejecutarRutina1() {
  Serial.println("Ejecutando Rutina 1 Original...");
  digitalWrite(PIN_COMPRESOR, LOW); // Activa succión al iniciar
  
  // Paso 1: Base
  myservo0.write(20); myservo1.write(90); myservo2.write(90);
  myservo3.write(90); myservo4.write(90); myservo5.write(90);
  delay(1500); if(!rutinaActiva) return;

  myservo0.write(160); delay(1500); if(!rutinaActiva) return;

  // Paso 2: Hombro
  myservo0.write(90); myservo1.write(20); delay(1500); if(!rutinaActiva) return;
  myservo1.write(160); delay(1500); if(!rutinaActiva) return;

  // Paso 3: Codo
  myservo1.write(90); myservo2.write(0); delay(1500); if(!rutinaActiva) return;
  myservo2.write(180); delay(1500); if(!rutinaActiva) return;

  // Paso 4: Coordinado (Articulación 4 y 5)
  myservo2.write(90); myservo3.write(140); myservo4.write(70); 
  delay(2000); if(!rutinaActiva) return;

  // Paso 5: Muñeca
  myservo3.write(90); myservo4.write(90); myservo5.write(20); 
  delay(1500); if(!rutinaActiva) return;
  myservo5.write(160); delay(1500); if(!rutinaActiva) return;

  Serial.println("Rutina Finalizada.");
  digitalWrite(PIN_COMPRESOR, HIGH); // Apaga succión
  configurarPosicionReposo();
  rutinaActiva = false;
}

void setup() {
  // Seguridad Compresor
  pinMode(PIN_COMPRESOR, OUTPUT);
  digitalWrite(PIN_COMPRESOR, HIGH);

  Serial.begin(9600);

  // Attach de tus 6 servos originales
  myservo0.attach(2); myservo1.attach(3); myservo2.attach(4);
  myservo3.attach(5); myservo4.attach(6); myservo5.attach(7);

  configurarPosicionReposo();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando al iPhone...");
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  server.begin();
  Serial.print("Conectado. IP: "); Serial.println(WiFi.localIP());
}

void loop() {
  if (rutinaActiva) {
    ejecutarRutina1();
  }

  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    
    // Respuesta para que la interfaz web funcione (CORS)
    client.println("HTTP/1.1 200 OK");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("OK");
    client.stop();

    if (request.indexOf("GET /start") != -1) {
      rutinaActiva = true;
    }
    if (request.indexOf("GET /reset") != -1) {
      rutinaActiva = false;
      configurarPosicionReposo();
    }
  }
}