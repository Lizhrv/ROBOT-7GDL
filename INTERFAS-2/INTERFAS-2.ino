   #include <WiFiS3.h> // Librería para Arduino R4 WiFi
#include <Servo.h> 

// CONFIGURACIÓN DE RED WIFI (¡ACTUALIZA ESTOS VALORES!)
char ssid[] = "iPhone Liz";       // Nombre de tu red WiFi
char pass[] = "lizlizliz";         // Contraseña de tu red WiFi
int status = WL_IDLE_STATUS;       

// CONFIGURACIÓN DEL SERVIDOR
WiFiServer server(80);
volatile bool rutinaActiva = false; 



// CONFIGURACIÓN DEL ROBOT
const int NUM_SERVOS = 7;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8};
Servo myservo[NUM_SERVOS]; 

// --- FUNCIÓN PARA POSICIÓN DE REPOSO Y RESET ---
void configurarPosicionReposo() {
    myservo[0].write(90);  // Base
    myservo[1].write(160); // Hombro
    myservo[2].write(90);  // Codo
    myservo[3].write(90);  // Art. 4
    myservo[4].write(90);  // Art. 5
    myservo[5].write(90);  // Muñeca
    myservo[6].write(40);  // Pinza
    delay(1000); 
    Serial.println("Robot en posicion de reposo.");
}

// --- FUNCIÓN DE RUTINA DE MOVIMIENTO PROGRAMADO ---
void ejecutarRutina() {
    Serial.println("Ejecutando secuencia...");

    myservo[0].write(20); delay(1000); if (!rutinaActiva) return;
    myservo[0].write(160); delay(1000); if (!rutinaActiva) return;

    myservo[1].write(20); delay(1000); if (!rutinaActiva) return;
    myservo[1].write(160); delay(1000); if (!rutinaActiva) return;

    myservo[2].write(0); delay(1000); if (!rutinaActiva) return;
    myservo[2].write(180); delay(1000); if (!rutinaActiva) return;

    myservo[3].write(140); myservo[4].write(70); delay(2000); if (!rutinaActiva) return;

    myservo[6].write(40); delay(1000); if (!rutinaActiva) return;
    myservo[6].write(140); delay(1000); if (!rutinaActiva) return;

    Serial.println("Secuencia completada.");
    configurarPosicionReposo(); 
    rutinaActiva = false; 
}

// --- CONFIGURACIÓN ---
void setup() {
    Serial.begin(9600);
    
    // Inicialización de Servos
    for (int i = 0; i < NUM_SERVOS; i++) {
        myservo[i].attach(servoPins[i]); 
    }
    configurarPosicionReposo();
    
    // Conexión WiFi
    while (status != WL_CONNECTED) {
        Serial.print("Intentando conectar a la red SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000); 
    }

    Serial.println("¡Conectado a la red!");
    Serial.print("Direccion IP del Arduino: ");
    Serial.println(WiFi.localIP()); 

    // Iniciar Servidor
    server.begin();
}

// --- BUCLE PRINCIPAL ---
void loop() {
    if (rutinaActiva) {
        ejecutarRutina();
    }
    
    WiFiClient client = server.available(); 
    if (client) {
        // Capturamos la primera línea de la petición
        String requestLine = client.readStringUntil('\r');
        client.flush(); // limpia el resto
        
        Serial.print("Petición recibida: ");
        Serial.println(requestLine);

        // Procesar comandos
        if (requestLine.indexOf("GET /start") != -1) {
            rutinaActiva = true;
            Serial.println("Comando START recibido. Rutina Activada.");
        } else if (requestLine.indexOf("GET /stop") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando STOP recibido. Rutina Detenida.");
        } else if (requestLine.indexOf("GET /reset") != -1) {
            rutinaActiva = false;
            configurarPosicionReposo();
            Serial.println("Comando RESET recibido. Robot en reposo forzado.");
        } else {
            Serial.print("Petición no reconocida: ");
            Serial.println(requestLine);
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
