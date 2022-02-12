// C++ code
//
#define HCSR 0
#define TMP 1
#define PHOTO 2
#define ALARM 3 // gjer "ALARM" om til talet 3

const int sensorPin = A0;
const int tempPin = A1;
const int buttonPin = 5; // tildelar namn til pin-ane
const int echo = 7;
const int trigger = 8;
const int ledPin = 10;
int OLEDstate = 0;
int buttonstate = 0; // skapar globalevariablar


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels



// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup(){
  pinMode(sensorPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);  // gir pin-ane inn eller ut funksjon
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
}

float readVoltage(int pin){ // lagar funksjon som finn spenninga tilsvarande sensorverdien frå pin-en målt
  float readvalue = analogRead(pin); // les av ein pin
  float voltage = ((float)readvalue*(float)5)/(float)1023; // gjer om avlest verdi til tilsvarande spenning
  return voltage; // gir ut den funnet spenninga. Einaste parameteret/argumentet er kva pin den skal måle.
}

float readTemp(int pin){// funksjon som finn temp i celsius, ved hjelp av forrige funksjon
  float tempspenning = readVoltage(pin); // får spenninga som er i forhold til sensorverdien
  float celsius = ((float)tempspenning - (float)0.5) * (float)100; // reknar om spenninga til celsius
  return celsius; // returnera celsius. Berre pin som parameter/Argument.
}

float measureDistance(int trigger, int echo){ // ikkje min funksjon, men finn avstanden til ultrasonisk sensor
  // Skrur trigger lav for å være sikker på at
  // høypulsen er "ren"
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Skrur på triggerpin i 10µs, deretter av igjen
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  // Leser av echopinne, og returnerer lydbølgens reisetid
  // i mikrosekunder
  long duration = pulseIn(echo, HIGH);
  // Utregning av distanse. 0.0343 er lydens hastighet i cm/µs.
  // Deles på to da lyden skal både frem og tilbake igjen
  float distance = duration * 0.0343 / 2;
  return distance; // gir avstanden i cm frå hindring til avstandsensoren
}

void loop(){
  buttonstate = digitalRead(buttonPin); // startar med å lese om knappen er trykt eller ikkje, somregel ikkje.
  display.clearDisplay(); // resettar displayet på OLED skjermen
  display.setCursor(0,10); // resettar posisjonen til kvar teksta blir printa
  float spenning = readVoltage(sensorPin); // kallar på spenning funksjonen, og gir den pin-en til lysmålaren
  float temperatur = readTemp(tempPin); // kallar på temp funkjsonen og gir den pin-en til tempmålaren
  float avstand = measureDistance(trigger, echo); // kallar på avtsand funksjonen og gir dei to pin-ane til sensoren
  
  if (buttonstate == HIGH){ // mange if og else løkker for å sjekke og styre kva stadie knappen for å bytte måling er på. 
                            //Oppfylt viss knappen er trykt inn, som blir sjekka først i void loop
    if (OLEDstate != 0){ // opppfylt viss Oledsate ikkje er 0, den er global og startar som 0
      if (OLEDstate != 1){ // oppfylt når Oledstate ikkje er 1, men kan heller ikkje være 0 for å kome hit.
        if (OLEDstate != 2){ // oppfylt når den ikkje er 2, men er heller ikkje 0 eller 1, så må være 3
          OLEDstate = 0; // her må oledstate være 3, så då blir den resetta til 0
        }
        else{ // hamnar her når OLedstate er 2, så den blir dermed 3
          OLEDstate = 3;
        }
      }
      else{ // hanar her når OLEDstate er 1, så den blir 2
        OLEDstate = 2;
      }
    }
    else{
      OLEDstate = 1; // kjem hit når OLEDstate er 0, så blir den til 1, som er det naturlege etter 0.
    }
    while (buttonstate == HIGH){  // while løkka frys programmet til knappen er sluppet igjen, så 
      buttonstate = digitalRead(buttonPin); // den ikkje gjer mange gjennomkøyringar og hoppar over stadier. sjekkar om den er sluppet
    }
  }

  switch (OLEDstate) { // kva tilstand/tal OLEDSstate er i avgjer kva case som blir køyrd, og kva som viser på skjermen
   case HCSR:
   display.println((String)spenning+"V");// case 0, resultatet frå spenningfunksjonen med V for volt blir vist på OLED
   break;
   case TMP:
   display.println((String)temperatur+" Grader");// case 1, resuktatet frå tempfunksjonen med grader blir vist på skjermen
   break;
   case PHOTO:
   display.println((String)avstand+"cm"); // case 2, resultat frå avstandfunskjonen pluss cm blir vist på skjermen
   break;
   case ALARM:
   display.println((String)"Alarm on"); // case 3, på dette stillet er alarmen på.
   if(avstand < 10){ // sjekkar om resultatet i cm frå avstandfunksjonen er mindre enn 10
    digitalWrite(ledPin, HIGH); // viss det er det, gir den alarm i for av lysande raud diode. 
   }
   else{
    digitalWrite(ledPin, LOW); // er den lenger vekk enn 10cm vil dioda ikkje lyse.
   }
   break; // når OLEDstate ikkje er 3, er alarmen avslått.
  }
  
  display.display(); // oppdaterar displayet så den vise det som nettop blei bestemt i loopen.
}
