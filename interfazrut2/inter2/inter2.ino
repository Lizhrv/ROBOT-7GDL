#include <WiFiS3.h> 
#include <Servo.h> 

// === 1. CONFIGURACIÓN DE RED WIFI ===
char ssid[] = "COMPUTACION E4";     
char pass[] = "C0MPUT4C10N%16";       
int status = WL_IDLE_STATUS;        
WiFiServer server(80);
volatile bool rutinaActiva = false; 

// === 2. CONFIGURACIÓN DE SERVOS DEL BRAZO (D2-D8) ===
const int NUM_SERVOS = 7;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8};
Servo myservo[NUM_SERVOS]; 
// [0]D2 Base, [1]D3 Hombro, [2]D4 Codo, [3]D5 Muñeca-Pitch, 
// [4]D6 Muñeca-Roll, [5]D7 (SEGURO), [6]D8 Garra/Herramienta

// === 3. CONFIGURACIÓN DE COMPRESOR (D9) ===
Servo compresorRelay; 
const int PIN_COMPRESOR = 9; 
const int COMPRESOR_ON = 180;
const int COMPRESOR_OFF = 0;

// === 4. CONFIGURACIÓN DEL CARRO (D10-D13) ===
const int IZQ_IN1 = 10; 
const int IZQ_IN2 = 11; 
const int DER_IN3 = 12; 
const int DER_IN4 = 13; 

// --- Funciones de Control del Carro ---

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

// --- Funciones de Posición del Brazo ---

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
    // Brazo baja
    myservo[0].write(45);  
    myservo[1].write(130); 
    myservo[2].write(50);  
    
    // Muñeca opuesta para nivelar (D5 y D6)
    myservo[3].write(40);  
    myservo[4].write(140); 
    
    myservo[5].write(90);  // D7 (Pin seguro)
    myservo[6].write(160); 
    delay(1000);
}

void posicionTransporte() {
    // Brazo levanta (Compresor sigue ON)
    myservo[1].write(90); 
    myservo[2].write(90); 
    
    // Muñeca a neutral para estabilidad
    myservo[3].write(90);
    myservo[4].write(90);
    myservo[5].write(90); 
    delay(1000);
}

void posicionLiberacion() {
    // Brazo baja en el destino
    myservo[0].write(135); 
    myservo[1].write(130); 
    myservo[2].write(50);  
    
    // Muñeca opuesta en posición de liberación
    myservo[3].write(50);  
    myservo[4].write(130); 
    myservo[5].write(90);  
    delay(1000);
}


// --- FUNCIÓN DE RUTINA REFINADA: RECOGER Y ENTREGAR ---

void ejecutarRutina() {
    Serial.println("INICIO: Rutina de Recogida y Entrega.");
    
    // === 1. IR A ZONA DE RECOLECCIÓN ===
    Serial.println("1. Carro: Movimiento ADELANTE.");
    moverAdelante(); 
    delay(2000); // Avanza
    detenerCarro();
    delay(500);

    // === 2. RECOGER OBJETO ===
    Serial.println("2. Brazo: Bajando para el agarre.");
    posicionAgarre(); // El brazo baja al piso
    
    // *** ¡AQUÍ SUCCIONA AL LLEGAR AL PISO! ***
    Serial.println("3. Compresor: ON (Succionando).");
    compresorRelay.write(COMPRESOR_ON); // Activa Compresores
    delay(1500); // Tiempo para establecer el vacío

    // 3. TRANSPORTE
    Serial.println("4. Brazo: Levantando objeto para transporte.");
    posicionTransporte(); // Levanta el brazo
    
    Serial.println("5. Carro: Movimiento ATRÁS.");
    moverAtras(); // Mueve el carro hacia el destino
    delay(3000); 
    detenerCarro();
    
    // 4. LIBERACIÓN
    Serial.println("6. Brazo: Bajando en zona de liberación.");
    posicionLiberacion(); // Baja el brazo en el destino
    
    Serial.println("7. Compresor: OFF (Soltando).");
    compresorRelay.write(COMPRESOR_OFF); // Desactiva Compresores
    delay(1500); // Tiempo para que el objeto caiga

    // 5. REPOSO FINAL
    Serial.println("8. FINAL: Volviendo a reposo.");
    configurarPosicionReposo(); 
    rutinaActiva = false; 
}


// --- SETUP y LOOP (Sin Cambios) ---

void setup() {
    Serial.begin(9600);
    
    // 1. Inicialización de Servos (D2-D8)
    for (int i = 0; i < NUM_SERVOS; i++) {
        myservo[i].attach(servoPins[i]); 
    }
    
    // 2. Inicialización del Compresor (D9)
    compresorRelay.attach(PIN_COMPRESOR);
    
    // 3. Configuración del Puente H (D10-D13)
    pinMode(IZQ_IN1, OUTPUT);
    pinMode(IZQ_IN2, OUTPUT);
    pinMode(DER_IN3, OUTPUT);
    pinMode(DER_IN4, OUTPUT);

    configurarPosicionReposo(); 
    
    // 4. Conexión WiFi
    while (status != WL_CONNECTED) {
        Serial.print("Intentando conectar a la red SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000); 
    }

    Serial.println("COMPUTACION E4");
    Serial.print("172.16.1.112172.20.10.2172.16.1.106172.16.1.106");
    Serial.println(WiFi.localIP()); 
    server.begin();
}

void loop() {
    // Si la bandera está activa, ejecuta la rutina completa
    if (rutinaActiva) {
        ejecutarRutina();
    }
    
    // Gestión del servidor web
    WiFiClient client = server.available(); 
    if (client) {
        String requestLine = client.readStringUntil('\r');
        client.flush();
        Serial.print("Petición recibida: ");
        Serial.println(requestLine);

        // Procesar comandos START, STOP, RESET
        if (requestLine.indexOf("GET /start") != -1) {
            if (!rutinaActiva) rutinaActiva = true;
            Serial.println("Comando START recibido. Rutina Activada.");
        } else if (requestLine.indexOf("GET /stop") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando STOP recibido. Rutina Detenida.");
        } else if (requestLine.indexOf("GET /reset") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando RESET recibido. Robot en reposo forzado.");
        } 

        // Respuesta HTTP
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("Comando Recibido");
        client.stop();
    }
}