#include <Servo.h> 

// Declaración de los 7 objetos Servo:
Servo myservo0; // Pin 2: Base/Rotación
Servo myservo1; // Pin 3: Hombro -> Termina en 160° (Posición Baja)
Servo myservo2; // Pin 4: Codo
Servo myservo3; // Pin 5: Articulación 4 (Eje)
Servo myservo4; // Pin 6: Articulación 5 (Eje opuesto)
Servo myservo5; // Pin 7: Muñeca Arriba/Abajo
Servo myservo6; // Pin 8: Pinza/Agarre (Giro/Rotación) -> Termina en 40°

void setup() 
{ 
  // Asignación de Pines para los 7 Servos (Pines 2 al 8)
  myservo0.attach(2); 
  myservo1.attach(3);
  myservo2.attach(4);
  myservo3.attach(5);
  myservo4.attach(6);
  myservo5.attach(7);  
  myservo6.attach(8);  
  
  // Posición inicial: 90 grados para todos
  myservo0.write(90);  
  myservo1.write(90); 
  myservo2.write(90);  
  myservo3.write(90);  
  myservo4.write(90);
  myservo5.write(90); 
  myservo6.write(90); 
} 

void loop() 
{ 
  // --- SECUENCIAS DE MOVIMIENTO (No se modifican) ---

  // Prueba Servo 0 (Base)
  myservo0.write(20); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  myservo0.write(160); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  
  // Prueba Servo 1 (Hombro)
  myservo0.write(90); myservo1.write(20); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  myservo0.write(90); myservo1.write(160); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  
  // Reset a 90°
  myservo0.write(90); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 

  // Prueba Servo 2 (Codo)
  myservo0.write(90); myservo1.write(90); myservo2.write(0); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  myservo0.write(90); myservo1.write(90); myservo2.write(180); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  
  // Reset a 90°
  myservo0.write(90); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  
  // Movimiento Coordinado/Opuesto (Articulación 4 y 5)
  myservo0.write(90); myservo1.write(90); myservo2.write(90); myservo3.write(140); myservo4.write(70); myservo5.write(90); myservo6.write(90); delay(2000); 
  
  // Reset a 90°
  myservo0.write(90); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(90); delay(1500); 
  
  // --- MOVIMIENTO DE PINZA/AGARRE - GIRO (MYSPEVO6 en Pin 8) ---
  myservo0.write(90); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(40); delay(1500);  
  myservo0.write(90); myservo1.write(90); myservo2.write(90); myservo3.write(90); myservo4.write(90); myservo5.write(90); myservo6.write(140); delay(1500);  
  
  // 🚀 REPOSO FINAL AJUSTADO
  
  // 1. Regreso a la posición de reposo final:
  myservo0.write(90); 
  myservo1.write(160); // Hombro: Posición Baja
  myservo2.write(90); 
  myservo3.write(90); 
  myservo4.write(90); 
  myservo5.write(90); // Muñeca Arriba/Abajo: Centro
  myservo6.write(40); // Pinza/Giro: Posición de Reposo ajustada a 40°
  
  // 2. Espera 1.5 segundos.
  delay(1500); 
  
  // 3. Bloquea el robot en esta posición.
  delay(15000000); 
}