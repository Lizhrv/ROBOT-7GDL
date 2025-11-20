#include <WiFiS3.h> 
#include <Servo.h> 

// ****************************************************
// === 1. CONFIGURACIÓN INICIAL ===
// ****************************************************

// --- RED WIFI ---
char ssid[] = "COMPUTACION E4";     // Nombre de tu red WiFi
char pass[] = "C0MPUT4C10N%16";       // Contraseña de tu red WiFi
int status = WL_IDLE_STATUS;        
WiFiServer server(80);
volatile bool rutinaActiva = false; // Bandera para Rutina 1

// --- SERVOS DEL BRAZO (D2-D8) ---
const int NUM_SERVOS = 7;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8};
Servo myservo[NUM_SERVOS]; 

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
// === 2. FUNCIONES DE MOVIMIENTO BASE ===
// ****************************************************

// --- Control del Carro ---
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

// --- Posiciones del Brazo (Muñeca Opuesta y D7 Seguro) ---
void configurarPosicionReposo() {
    for (int i = 0; i < NUM_SERVOS; i++) {
        myservo[i].write(90);
    }
    compresorRelay.write(COMPRESOR_OFF); 
    detenerCarro();
    delay(1000); 
    Serial.println("Robot en posicion de reposo.");
}

void posicionAgarre() {
    // Baja el brazo y prepara muñeca opuesta (D5 y D6)
    myservo[0].write(45); myservo[1].write(130); myservo[2].write(50);  
    myservo[3].write(40); myservo[4].write(140); // Muñeca opuesta
    myservo[5].write(90); // D7 Seguro
    myservo[6].write(160); 
    delay(1000);
}

void posicionTransporte() {
    // Levanta y centra el brazo
    myservo[1].write(90); myservo[2].write(90); 
    myservo[3].write(90); myservo[4].write(90); // Muñeca a neutral
    myservo[5].write(90); 
    delay(1000);
}

void posicionLiberacion() {
    // Baja el brazo y prepara muñeca opuesta para soltar
    myservo[0].write(135); myservo[1].write(130); myservo[2].write(50);  
    myservo[3].write(50); myservo[4].write(130); 
    myservo[5].write(90); 
    delay(1000);
}


// ****************************************************
// === 3. RUTINAS DE EJECUCIÓN ===
// ****************************************************

// --- RUTINA 1: Recoger y Entregar ---
void ejecutarRutina() {
    Serial.println("INICIO: Rutina 1 (Recogida y Entrega).");
    
    // 1. IR A RECOLECCIÓN
    moverAdelante(); delay(2000); detenerCarro(); delay(500);

    // 2. RECOGER (Suciona al bajar)
    posicionAgarre(); 
    compresorRelay.write(COMPRESOR_ON); 
    delay(1500); 

    // 3. TRANSPORTE
    posicionTransporte(); 
    moverAtras(); delay(3000); detenerCarro();
    
    // 4. LIBERACIÓN
    posicionLiberacion(); 
    compresorRelay.write(COMPRESOR_OFF); 
    delay(1500); 

    // 5. REPOSO FINAL
    Serial.println("FINAL: Rutina 1 completada.");
    configurarPosicionReposo(); 
    rutinaActiva = false; 
}

// --- RUTINA 2: Movimiento de Exhibición ---
void ejecutarRutina2() {
    Serial.println("INICIO: Rutina 2 (Exhibición).");
    
    // Movimiento 1: Giro y elevación
    myservo[0].write(160); 
    myservo[1].write(40);  
    delay(1500);

    // Movimiento 2: Extensión y movimiento corto del carro
    myservo[2].write(150); 
    moverAdelante();
    delay(1000);
    detenerCarro();
    
    // Movimiento 3: Succión y vuelta
    compresorRelay.write(COMPRESOR_ON); 
    delay(500);
    compresorRelay.write(COMPRESOR_OFF); 
    
    myservo[0].write(90);
    myservo[1].write(90);
    myservo[2].write(90);
    delay(1000);

    Serial.println("FINAL: Rutina 2 completada.");
    configurarPosicionReposo(); 
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
    
    // 2. Conexión WiFi
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
    // Rutina 1 se ejecuta aquí cuando rutinaActiva es TRUE
    if (rutinaActiva) {
        ejecutarRutina(); 
        return; 
    }
    
    // Gestión del servidor web
    WiFiClient client = server.available(); 
    if (client) {
        String requestLine = client.readStringUntil('\r');
        client.flush();

        // --- LÓGICA DE PROCESAMIENTO (SIN SEGURIDAD POR CLAVE) ---
        
        // Comando /start (Rutina 1)
        if (requestLine.indexOf("GET /start") != -1) {
            if (!rutinaActiva) rutinaActiva = true;
            Serial.println("Comando START (R1) - OK.");
        } 
        // Comando /rutina2 (Rutina 2)
        else if (requestLine.indexOf("GET /rutina2") != -1) {
            if (!rutinaActiva) {
               ejecutarRutina2(); // Se ejecuta inmediatamente
            }
            Serial.println("Comando RUTINA 2 - OK.");
        } 
        // Comando /reset
        else if (requestLine.indexOf("GET /reset") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando RESET - OK.");
        } 

        // Respuesta HTTP estándar
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("Comando Recibido y Procesado.");
        client.stop();
    }
}