#include <WiFiS3.h> // Librería para Arduino R4 WiFi
#include <Servo.h> 

// CONFIGURACIÓN DE RED WIFI (¡ACTUALIZA ESTOS VALORES!)
char ssid[] = "COMPUTACION E4";       // Nombre de tu red WiFi
char pass[] = "C0MPUT4C10N%16";    // Contraseña de tu red WiFi
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
    // La posición de reposo final que has definido:
    myservo[0].write(90);  // Base: 90°
    myservo[1].write(160); // Hombro: 160° (Posición Baja)
    myservo[2].write(90);  // Codo: 90°
    myservo[3].write(90);  // Art. 4: 90°
    myservo[4].write(90);  // Art. 5: 90°
    myservo[5].write(90);  // Muñeca Arriba/Abajo: 90°
    myservo[6].write(40);  // Pinza/Giro: 40°
    delay(1000); 
    Serial.println("Robot en posicion de reposo.");
}

// --- FUNCIÓN DE RUTINA DE MOVIMIENTO PROGRAMADO ---
void ejecutarRutina() {
    // Nota: Mantuvimos delays cortos (1s) para que el servidor siga respondiendo.
    
    Serial.println("Ejecutando secuencia...");

    // Prueba Servo 0 (Base)
    myservo[0].write(20); delay(1000); 
    if (!rutinaActiva) return;
    myservo[0].write(160); delay(1000); 
    if (!rutinaActiva) return;

    // Prueba Servo 1 (Hombro)
    myservo[1].write(20); delay(1000); 
    if (!rutinaActiva) return;
    myservo[1].write(160); delay(1000); // 4to movimiento
    if (!rutinaActiva) return;

    // Prueba Servo 2 (Codo)
    myservo[2].write(0); delay(1000); 
    if (!rutinaActiva) return;
    myservo[2].write(180); delay(1000); 
    if (!rutinaActiva) return;

    // Movimiento Coordinado/Opuesto (Articulación 4 y 5)
    myservo[3].write(140); myservo[4].write(70); delay(2000); 
    if (!rutinaActiva) return;

    // Movimiento de Pinza/Agarre (Giro)
    myservo[6].write(40); delay(1000);  
    if (!rutinaActiva) return;
    myservo[6].write(140); delay(1000);  
    if (!rutinaActiva) return;
    
    // Al finalizar la secuencia, vuelve a reposo y se desactiva
    Serial.println("Secuencia completada.");
    configurarPosicionReposo(); 
    rutinaActiva = false; 
}

// --- CONFIGURACIÓN ---
void setup() {
    Serial.begin(9600);
    
    // 1. Inicialización de Servos
    for (int i = 0; i < NUM_SERVOS; i++) {
        myservo[i].attach(servoPins[i]); 
    }
    configurarPosicionReposo(); // Establece la posición inicial de reposo
    
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

    // 3. Iniciar Servidor
    server.begin();
}

// --- BUCLE PRINCIPAL ---
void loop() {
    if (rutinaActiva) {
        ejecutarRutina(); // Ejecuta la rutina con la bandera activa
    }
    
    // Escuchar nuevas peticiones HTTP
    WiFiClient client = server.available(); 
    if (client) {
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // 1. Enviar respuesta HTTP
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println();
                        client.println("Comando Recibido");
                        
                        // 2. Procesar el comando
                        if (currentLine.indexOf("GET /start") != -1) {
                            // Si ya está activa, no hace nada; si no, la activa.
                            if(!rutinaActiva) rutinaActiva = true; 
                            Serial.println("Comando START recibido. Rutina Activada.");
                        } else if (currentLine.indexOf("GET /stop") != -1) {
                            rutinaActiva = false;
                            configurarPosicionReposo(); // Detiene y va a reposo
                            Serial.println("Comando STOP recibido. Rutina Detenida.");
                        } else if (currentLine.indexOf("GET /reset") != -1) {
                            // El comando RESET fuerza la bandera a falso y va a reposo.
                            rutinaActiva = false;
                            configurarPosicionReposo(); 
                            Serial.println("Comando RESET recibido. Robot en reposo forzado.");
                        }
                        break; 
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c; 
                }
            }
        }
        client.stop(); 
    }
}