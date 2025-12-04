#include <WiFiS3.h> 
#include <Servo.h> 

// ****************************************************
// === 1. CONFIGURACIÓN INICIAL Y HARDWARE ===
// ****************************************************

// --- RED WIFI (Actualizado a petición del usuario) ---
char ssid[] = "COMPUTACION E4";     
char pass[] = "C0MPUT4C10N%16";        
int status = WL_IDLE_STATUS;        
WiFiServer server(80);
volatile bool rutinaActiva = false; 

// --- SERVOS DEL BRAZO (6 Servos) ---
const int NUM_SERVOS = 6; 
// Pines: D2, D3, D4, D5, D6, y D8
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 8}; 
Servo myservo[NUM_SERVOS]; 
/* Mapeo de Servos (índice en array):
[0] = D2 (Base)
[1] = D3 (Hombro)
[2] = D4 (Giro Hombro)
[3] = D5 (Extensión 1)
[4] = D6 (Extensión 2)
[5] = D8 (Muñeca)
*/

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
// === 2. FUNCIONES DE MOVIMIENTO BASE (CARRO Y REPOSO) ===
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

void configurarPosicionReposo() {
    Serial.println("Robot en Reposo: Hombro (D3) a 0. Demas a 90.");
    
    // Servo 2 (Hombro/D3) inicia y termina a 0°
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

// Las funciones de posicionamiento específicas (Agarre, Transporte, Liberacion) 
// han sido ELIMINADAS para ser reemplazadas por la nueva Rutina 1 más simple.

// ****************************************************
// === 3. RUTINAS DE EJECUCIÓN ===
// ****************************************************

// --- RUTINA 1: Movimiento Simple de los 6 Servos ---
// Basada en el código original simple del usuario.
void ejecutarRutina() {
    Serial.println("INICIO: Rutina 1 (Movimiento de 6 Servos).");
    
    configurarPosicionReposo(); 
    
    // Nota: Se han eliminado todos los comandos de carro y compresor.

    // 1. Movimiento del Servo 0 (Base / D2): 20° y 160°
    Serial.println("Moviendo Servo 0 (Base)...");
    myservo[0].write(20); delay(1500); 
    myservo[0].write(160); delay(1500); 
    myservo[0].write(90); delay(1500); 

    // 2. Movimiento del Servo 1 (Hombro / D3): 20° y 160°
    Serial.println("Moviendo Servo 1 (Hombro)...");
    myservo[1].write(20); delay(1500); 
    myservo[1].write(160); delay(1500); 
    myservo[1].write(0); delay(1500); // Vuelve a la posición de Reposo (0°)

    // 3. Movimiento del Servo 2 (Giro Hombro / D4): 0° y 180°
    Serial.println("Moviendo Servo 2 (Giro Hombro)...");
    myservo[2].write(0); delay(1500); 
    myservo[2].write(180); delay(1500); 
    myservo[2].write(90); delay(1500); 

    // 4. Movimiento Contradictorio de Servos 3 y 4 (Extensión 1/2, D5/D6)
    // El servo 3 va a 140° y el servo 4 va a 70°, creando un movimiento coordinado/contradictorio.
    Serial.println("Moviendo Servos 3 y 4 (Contradictorio)...");
    myservo[3].write(140); 
    myservo[4].write(70); 
    delay(2000); // Mayor espera para el movimiento coordinado
    myservo[3].write(90); // Reposo
    myservo[4].write(90); // Reposo
    delay(1500);

    // 5. Movimiento del Servo 5 (Muñeca / D8): 20° y 160°
    Serial.println("Moviendo Servo 5 (Muñeca)...");
    myservo[5].write(20); delay(1500);
    myservo[5].write(160); delay(1500); 
    myservo[5].write(90); delay(1500); 

    // 6. Reposo Final
    Serial.println("FINAL: Volviendo a reposo.");
    configurarPosicionReposo(); 
    rutinaActiva = false; 
}

// --- RUTINA 2: Movimiento de Carro y Activación de Compresor ---
// Solo mueve los motores del carro (1s cada movimiento) y luego activa el compresor.
void ejecutarRutina2() {
    Serial.println("INICIO: Rutina 2 (Movimiento del Carro y Compresor).");
    
    configurarPosicionReposo(); // Comienza en Reposo

    // 1. Movimiento Adelante (1 segundo)
    Serial.println("Movimiento 1: Adelante (1s)");
    moverAdelante(); delay(1000); detenerCarro(); delay(500);

    // 2. Movimiento Atras (1 segundo)
    Serial.println("Movimiento 2: Atras (1s)");
    moverAtras(); delay(1000); detenerCarro(); delay(500);

    // 3. Movimiento Girar Derecha (1 segundo)
    Serial.println("Movimiento 3: Girar Derecha (1s)");
    girarDerecha(); delay(1000); detenerCarro(); delay(500);

    // 4. Activación del Compresor (Mantiene por 2s)
    Serial.println("Activando Compresor...");
    compresorRelay.write(COMPRESOR_ON); 
    delay(2000); 
    compresorRelay.write(COMPRESOR_OFF); 

    // 5. Reposo Final
    Serial.println("FINAL: Volviendo a reposo.");
    configurarPosicionReposo(); // Reposo final
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
        // Usa el nuevo SSID: COMPUTACION E4
        status = WiFi.begin(ssid, pass);
        delay(10000); 
    }

    Serial.println("¡Conectado a la red!");
    Serial.print("172.16.1.113");
    Serial.println(WiFi.localIP()); 
    
    server.begin();
}

void loop() {
    if (rutinaActiva) {
        // Ejecuta Rutina 1 si se activa por el comando /start
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
            if (!rutinaActiva) rutinaActiva = true; // Activa Rutina 1
            Serial.println("Comando START (R1) - OK. Activando Rutina...");
        } 
        else if (requestLine.indexOf("GET /rutina2") != -1) {
            if (!rutinaActiva) {
               ejecutarRutina2(); // Ejecuta Rutina 2 (Bloqueante)
            }
            Serial.println("Comando RUTINA 2 - OK. Ejecutando...");
        } 
        else if (requestLine.indexOf("GET /reset") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando RESET - OK. Reposo Forzado.");
        } 

        // Respuesta HTTP
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("Comando Recibido y Procesado.");
        client.stop();
    }
}