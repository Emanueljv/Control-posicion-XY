#include <Stepper.h>
const int analogOutPin = 5; // PWM
double vel = 0;

volatile int giro=0, xgiro=0;
static volatile unsigned long debounce = 0;

volatile int ppulsos,pulsos;
static volatile unsigned long debounce2 = 0;
int   posant=0, eje_y=0, eje_x=0;
Stepper myStepper(20, 8, 9, 10, 11);//L298N driver 2 puente H

int  posy=0,posx=0,velocidad=180;   //valor: pos
float pos,fposx,fposy,a,b,c;     //Constantes para parámetros de controlador PID 
float rt,eT,iT,dT,yT,uT,iT0,eT0; //Variables de controlador PID 
float maxi,mini; //Variables para anti-windup
String distancia_x, distancia_y; //Variables leidas del buffer
float dis_x, dis_y; //Variables leidas a float


void setup() {
  pinMode(2, INPUT);//Interrupción
  attachInterrupt(digitalPinToInterrupt(2), rpasos, RISING); ppulsos = 0;
  attachInterrupt(digitalPinToInterrupt(3), contador, RISING);
  myStepper.setSpeed(1000); //ESTABA EN 1200
  
  Serial.begin(9600);  
  pulsos = 0;
  pinMode(3, INPUT); 
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6,HIGH);
  digitalWrite(7,LOW);

  iT0=0.0; eT0=0.0; 
  a=1.5; b=0.50; c=0.50;
  
}
void loop() {                               //posx=pasos    posy=pulsos
 if (Serial.available() > 0) {
    delay(20);                              // y 0-115 pulsos  x 0-896 ó 2150 pasos
    String bufferString = "";
    while (Serial.available() > 0) {
      distancia_x = Serial.readStringUntil(' ');
      distancia_y = Serial.readStringUntil(';'); 
      dis_x = distancia_x.toFloat();
      dis_y = distancia_y.toFloat();
      }
      
      
      posx = dis_x * 12; //12 Equivale a 1 mm, con máx  166 mm
      posy= dis_y * 1.3; //1.3 Equivale a 1 mm, con máx 130 mm
      Serial.print("Moviendo ");
      Serial.print(dis_x);
      Serial.println("mm en X ");
      Serial.print("Moviendo ");
      Serial.print(dis_y);
      Serial.println("mm en Y  ");
    }

       if (dis_x >= 0.00 && dis_x <= 166.00 && dis_y >= 0.00 && dis_y <= 130.00 ){
            detachInterrupt(digitalPinToInterrupt(3));  //PULSOS
            attachInterrupt(digitalPinToInterrupt(3), contador, RISING);
         
         int eT= posy-pulsos;
            if(eT>0){
              giro=1;
              digitalWrite(6,HIGH);
              digitalWrite(7,LOW);
              analogWrite(analogOutPin, velocidad);
            }
            else if(eT<0){
              giro=2;
              eT=-eT;
              digitalWrite(6,LOW);
              digitalWrite(7,HIGH);
              analogWrite(analogOutPin, velocidad);
            }
            else if (eT == 0){
              digitalWrite(6,LOW);
              digitalWrite(7,LOW);
              eje_y = 1;     
            }
         
   //------final de posy---------------------------
            
            
          while(posx!=posant){ //posx
          int error= posx - posant;
          posant=posx;
          if(error>0){
          xgiro=1;
          myStepper.step(error);
          }
          else if (error<0){
          xgiro=-1;
          myStepper.step(error);
          }
          else{
            error = 0;
            
            }
          } 

      //-------Final de posx--------------------------
        
         if(eje_y == 1){ 
            delay(2500);
            eje_x = 0;
            eje_y = 0; 
            posx=0;
            posy=0;
           }
            else;

        }    

       else{
            Serial.println(" 'ERROR'  Ingrese un numero dentro del rango: ");
            delay(1000);
             }   
      
}

void rpasos(){  //Encoder CD
  if(  digitalRead (2) && (micros()-debounce > 500) && digitalRead (2) ) { 
        debounce = micros(); // Almacena el tiempo para comprobar que no contamos el rebote que hay en la señal.
    if (xgiro==1){ppulsos++;}
    if (xgiro==-1){ppulsos--;}  
      }
        else ;
}

void contador(){  
  if(  digitalRead (3) && (micros()-debounce2 > 500) && digitalRead (3) ) { 
// Vuelve a comprobar que el encoder envia una señal buena y luego comprueba que el tiempo es superior a 1000 microsegundos y vuelve a comprobar que la señal es correcta.
        debounce2 = micros(); // Almacena el tiempo para comprobar que no contamos el rebote que hay en la señal.
if (giro==1){pulsos++;}
if (giro==2){pulsos--;}     
      }  // Suma el pulso bueno que entra.
        else ;
}
