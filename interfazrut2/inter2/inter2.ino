#include <WiFiS3.h> 
#include <Servo.h> 

// ****************************************************
// === 1. CONFIGURACIÓN INICIAL Y HARDWARE ===
// ****************************************************

// --- RED WIFI ---
char ssid[] = "iPhone de Liz";     
char pass[] = "lizlizliz";       
int status = WL_IDLE_STATUS;        
WiFiServer server(80);
volatile bool rutinaActiva = false; 

// --- SERVOS DEL BRAZO (6 Servos, D2-D8, Omitiendo D7) ---
const int NUM_SERVOS = 6; 
// Pines: D2, D3, D4, D5, D6, y D8
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 8}; 
Servo myservo[NUM_SERVOS]; 
// Mapeo: [0]=D2(Base), [1]=D3(Hombro), [2]=D4(Giro Hombro), [3]=D5(Extensión 1), [4]=D6(Extensión 2), [5]=D8(Muñeca)

// --- COMPRESOR (D9) ---
Servo compresorRelay; 
const int PIN_COMPRESOR = 9; 
const int COMPRESOR_ON = 180;
const int COMPRESOR_OFF = 0;

// --- CARRO (D10-D13) ---
const int IZQ_IN1 = 10; 
const int IZQ_IN2 = 11; 
const int DER_IN3 = 12; 
const int DER_IN4 = 13; 


// ****************************************************
// === 2. FUNCIONES DE MOVIMIENTO BASE (REPOSO CORREGIDO) ===
// ****************************************************

void detenerCarro() {
  digitalWrite(IZQ_IN1, LOW);
  digitalWrite(IZQ_IN2, LOW);
  digitalWrite(DER_IN3, LOW);
  digitalWrite(DER_IN4, LOW);
}

void moverAdelante() {
  digitalWrite(IZQ_IN1, HIGH); 
  digitalWrite(IZQ_IN2, LOW);
  digitalWrite(DER_IN3, HIGH); 
  digitalWrite(DER_IN4, LOW);
}

void moverAtras() {
  digitalWrite(IZQ_IN1, LOW); 
  digitalWrite(IZQ_IN2, HIGH);
  digitalWrite(DER_IN3, LOW); 
  digitalWrite(DER_IN4, HIGH);
}

void girarDerecha() {
    digitalWrite(IZQ_IN1, HIGH); 
    digitalWrite(IZQ_IN2, LOW);
    digitalWrite(DER_IN3, LOW); 
    digitalWrite(DER_IN4, HIGH);
}

// 🛑 FUNCIÓN DE REPOSO CORREGIDA 🛑
void configurarPosicionReposo() {
    Serial.println("Robot en Reposo: Hombro (D3) a 0. Demas a 90.");
    
    // 🛑 Servo 2 (Hombro/D3) inicia y termina a 0°
    myservo[1].write(0);
    
    // Los demás servos inician y terminan a 90°
    myservo[0].write(90); // Base (D2)
    myservo[2].write(90); // Giro Hombro (D4)
    myservo[3].write(90); // Extensión 1 (D5)
    myservo[4].write(90); // Extensión 2 (D6)
    myservo[5].write(90); // Muñeca (D8)
    
    compresorRelay.write(COMPRESOR_OFF); 
    detenerCarro();
    delay(1000); 
}

void posicionAgarre() {
    // 🛑 Hombro se levanta de 0° a 130°
    myservo[0].write(45);   // Servo 1 (Base): Gira
    myservo[1].write(130);  // Servo 2 (Hombro): Levanta el brazo
    myservo[2].write(50);   // Servo 3 (Giro Hombro)
    myservo[3].write(40);   // Servo 4 (Extensión 1)
    myservo[4].write(140);  // Servo 5 (Extensión 2)
    myservo[5].write(160);  // Servo 7 (Muñeca)
    delay(1000);
}

void posicionTransporte() {
    // Brazo se baja a la posición segura (90°)
    myservo[1].write(90);   // Servo 2 (Hombro): 90° para estabilidad
    myservo[2].write(90);   
    myservo[3].write(90);   
    myservo[4].write(90);   
    myservo[5].write(90);   
    delay(1000);
}

void posicionLiberacion() {
    // 🛑 Hombro se levanta a 130° para bajar el objeto
    myservo[0].write(135);  // Servo 1 (Base): Gira
    myservo[1].write(130);  // Servo 2 (Hombro): Levanta el brazo
    myservo[2].write(50);   
    myservo[3].write(50);   
    myservo[4].write(130);  
    delay(1000);
}


// ****************************************************
// === 3. RUTINAS DE EJECUCIÓN ===
// ****************************************************

// --- RUTINA 1: Recoger y Entregar ---
void ejecutarRutina() {
    Serial.println("INICIO: Rutina 1 (Recogida y Entrega).");
    
    configurarPosicionReposo(); // 🛑 Comienza en Reposo (Hombro a 0°)
    
    // 1. IR A ZONA DE RECOLECCIÓN
    moverAdelante(); delay(2000); detenerCarro(); delay(500);

    // 2. RECOGER OBJETO
    posicionAgarre(); // Hombro a 130° (levantado)
    compresorRelay.write(COMPRESOR_ON); 
    delay(1500); 

    // 3. TRANSPORTE
    posicionTransporte(); // Hombro a 90° (estabilidad)
    moverAtras(); delay(3000); detenerCarro();
    
    // 4. LIBERACIÓN
    posicionLiberacion(); // Hombro a 130° (levantado)
    compresorRelay.write(COMPRESOR_OFF); 
    delay(1500); 

    // 5. REPOSO FINAL
    Serial.println("FINAL: Volviendo a reposo.");
    configurarPosicionReposo(); // 🛑 Termina en Reposo (Hombro a 0°)
    rutinaActiva = false; 
}

// --- RUTINA 2: Movimiento de Exhibición ---
void ejecutarRutina2() {
    Serial.println("INICIO: Rutina 2 (Exhibición).");
    
    configurarPosicionReposo(); // 🛑 Comienza en Reposo (Hombro a 0°)

    // Movimiento 1: Giro y elevación
    myservo[0].write(160); // Base gira
    myservo[1].write(40);  // 🛑 Hombro se levanta ligeramente (0° -> 40°)
    delay(1500);

    // Movimiento 2: Extensión y GIRO DE EXHIBICIÓN
    myservo[2].write(150); 
    girarDerecha(); 
    delay(2000); 
    detenerCarro();
    
    // Movimiento 3: Simulación de Succión
    compresorRelay.write(COMPRESOR_ON); 
    delay(500);
    compresorRelay.write(COMPRESOR_OFF); 
    
    // Movimiento 4: Vuelta al centro
    myservo[0].write(90); 
    myservo[1].write(90); // 🛑 Hombro vuelve a 90° momentáneamente
    myservo[2].write(90); 
    delay(1000);

    Serial.println("FINAL: Rutina 2 completada.");
    configurarPosicionReposo(); // 🛑 Termina en Reposo (Hombro a 0°)
    rutinaActiva = false;
}


// ****************************************************
// === 4. SETUP Y LOOP ===
// ****************************************************

void setup() {
    Serial.begin(9600);
    
    // 1. Inicialización de Hardware
    for (int i = 0; i < NUM_SERVOS; i++) {
        myservo[i].attach(servoPins[i]); 
    }
    compresorRelay.attach(PIN_COMPRESOR);
    pinMode(IZQ_IN1, OUTPUT);
    pinMode(IZQ_IN2, OUTPUT);
    pinMode(DER_IN3, OUTPUT);
    pinMode(DER_IN4, OUTPUT);

    configurarPosicionReposo(); 
    
    // 2. Conexión WiFi (Sin cambios)
    while (status != WL_CONNECTED) {
        Serial.print("Intentando conectar a la red SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000); 
    }

    Serial.println("¡Conectado a la red!");
    Serial.print("Direccion IP del Arduino: ");
    Serial.println(WiFi.localIP()); 
    
    server.begin();
}

void loop() {
    if (rutinaActiva) {
        ejecutarRutina(); 
        return; 
    }
    
    // Gestión del servidor web (Sin cambios)
    WiFiClient client = server.available(); 
    if (client) {
        String requestLine = client.readStringUntil('\r');
        client.flush();
        Serial.print("Petición recibida: ");
        Serial.println(requestLine);

        if (requestLine.indexOf("GET /start") != -1) {
            if (!rutinaActiva) rutinaActiva = true;
            Serial.println("Comando START (R1) - OK. Activando Rutina...");
        } 
        else if (requestLine.indexOf("GET /rutina2") != -1) {
            if (!rutinaActiva) {
               ejecutarRutina2(); 
            }
            Serial.println("Comando RUTINA 2 - OK. Ejecutando...");
        } 
        else if (requestLine.indexOf("GET /reset") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando RESET - OK. Reposo Forzado.");
        } 

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("Comando Recibido y Procesado.");
        client.stop();
    }
}