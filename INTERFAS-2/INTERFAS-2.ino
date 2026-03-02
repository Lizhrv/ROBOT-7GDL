#include <WiFiS3.h> 
#include <Servo.h> 

// CONFIGURACIÓN DE RED WIFI
char ssid[] = "iPhone Liz";       
char pass[] = "lizlizliz";    
int status = WL_IDLE_STATUS;       

WiFiServer server(80);
volatile bool rutinaActiva = false; 

// CONFIGURACIÓN DEL ROBOT
const int NUM_SERVOS = 7;
const int servoPins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8};
Servo myservo[NUM_SERVOS]; 

// --- PIN DEL COMPRESOR ---
const int PIN_COMPRESOR = A2; 

void configurarPosicionReposo() {
    digitalWrite(PIN_COMPRESOR, HIGH); // Mantiene compresor APAGADO
    myservo[0].write(90);  
    myservo[1].write(160); 
    myservo[2].write(90);  
    myservo[3].write(90);  
    myservo[4].write(90);  
    myservo[5].write(90);  
    myservo[6].write(40);  
    delay(1000); 
    Serial.println("Robot en reposo y compresores OFF.");
}

void ejecutarRutina() {
    Serial.println("Ejecutando secuencia...");
    
    // Ejemplo: Prender compresor al iniciar
    digitalWrite(PIN_COMPRESOR, LOW); // ON
    
    myservo[0].write(20); delay(1000); 
    if (!rutinaActiva) return;
    myservo[0].write(160); delay(1000); 
    
    // Ejemplo: Apagar compresor al final
    digitalWrite(PIN_COMPRESOR, HIGH); // OFF
    
    Serial.println("Secuencia completada.");
    configurarPosicionReposo(); 
    rutinaActiva = false; 
}

void setup() {
    // IMPORTANTE: Apagar antes de declarar salida para que no den el "chispazo"
    digitalWrite(PIN_COMPRESOR, HIGH); 
    pinMode(PIN_COMPRESOR, OUTPUT);

    Serial.begin(9600);
    
    for (int i = 0; i < NUM_SERVOS; i++) {
        myservo[i].attach(servoPins[i]); 
    }
    configurarPosicionReposo(); 
    
    while (status != WL_CONNECTED) {
        Serial.print("Conectando a: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000); 
    }

    Serial.println("¡Conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP()); 
    server.begin();
}

void loop() {
    if (rutinaActiva) {
        ejecutarRutina(); 
    }
    
    WiFiClient client = server.available(); 
    if (client) {
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // RESPUESTA HTTP con permiso para la web (CORS)
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Access-Control-Allow-Origin: *"); // ESTO ACTIVA TU INTERFAZ
                        client.println("Connection: close");
                        client.println();
                        client.println("OK");
                        
                        if (currentLine.indexOf("GET /start") != -1) {
                            rutinaActiva = true; 
                        } else if (currentLine.indexOf("GET /reset") != -1) {
                            rutinaActiva = false;
                            configurarPosicionReposo(); 
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