
//dvojitej pistostroj u nehoz matne tusim co dela. 


//------------------------------------------
//Konstanty

//debugovaci

//Pokud je tohle true, tak se bude vypisovat co dela na seriak (115200 baud). pokud neni treba, melo by byt false. 
bool LOGOVAT = true;
//na zacatku se sekundu blikne diodama, aby se vedelo ze se to restartlo. Ovsem pokud tohle neni false... 
bool BLIKAT_NA_STARTU = true;

//se nevedelo esli laser bude preruseny na nule nebo jednicce. tady se to nastavuje. cislo udava log hodnotu pro preruseny laser.
int LASER_PRERUSENY = 0;

//stejna logika pro indikatory odsunuti. Pokud to cidlo tam neni, tak na tom pinu bude 
//nejspis 0 a tim padem tady ten stav s jednickou nikdy nenastane a bude se to ridit jenom casem 
int ODSUNUTO=1;

//Piny
int PIN_LASER1 = 2;
int PIN_LASER2 = 3;
int PIN_STRIH1 = 8;
int PIN_STRIH2 = 12;
int PIN_POSUN1 = 13;
int PIN_POSUN2 = 11;
int PIN_ODSUN1 = 4;
int PIN_ODSUN2 = 5;
int PIN_DIODA1 = 9;
int PIN_DIODA2 = 10;

//Cekani konstanty - vsechny jsou v milisekundach - konstanta 1000 = 1 sekunda. 

//kazdy cyklus bude cekat tuto dobu, aby se necetlo vsechno furt dokola jak rychle to jenom jde.
int CYKLYCKA_PRODLEVA = 1;
//doba strihu (prodleva mezi zacatkem strihani a zapnutim odsunu)
int DOBA_STRIHU = 500;
//doba po kterou pojede posun po skonceni strihu
int DOBA_POSUNU = 2000;

//------------------------------------------
//promenne

// stavove indikatory : 
bool striha1 = false; //jestli se zrovna striha
bool striha2 = false;
bool posouva1 = false;//jestli se zrovna posouva
bool posouva2 = false;
bool laser1 = false;//ve ktere pozici je strih
bool laser2 = false; 


//citace prodlev. 
unsigned long cas_strih1;
unsigned long cas_strih2;
unsigned long cas_posun1;
unsigned long cas_posun2;
unsigned long cas_dioda1;
unsigned long cas_dioda2;


//------------------------------------------


void loguj(const char* msg) { 
    if (LOGOVAT){
      Serial.println(msg);      
    }
}

void setup() {
   Serial.begin(115200);
   delay(3);//mozna je potreba nechat chvilku na init. mozna ne, nevim. 
  
   loguj("Start. Nastavuju piny. ");
   pinMode(PIN_LASER1, INPUT);
   pinMode(PIN_LASER1, INPUT);
   pinMode(PIN_STRIH1, OUTPUT);
   pinMode(PIN_STRIH2, OUTPUT);
   pinMode(PIN_POSUN1, OUTPUT);
   pinMode(PIN_POSUN2, OUTPUT);
   pinMode(PIN_ODSUN1, INPUT);
   pinMode(PIN_ODSUN2, INPUT);
   pinMode(PIN_DIODA1, OUTPUT);
   pinMode(PIN_DIODA2, OUTPUT);

   if (BLIKAT_NA_STARTU) {
   //zablikej diodama aby vedeli ze ses restartl 
    loguj("Blikam diodama sekundu");
    for (int i = 0; i <=  10; i++) {
     digitalWrite(PIN_DIODA1, HIGH);
     digitalWrite(PIN_DIODA2, LOW);
     delay(100);
     digitalWrite(PIN_DIODA1, LOW);
     digitalWrite(PIN_DIODA2, HIGH);
    }//vypni diody a pokracuj 
    digitalWrite(PIN_DIODA2, LOW);
    digitalWrite(PIN_DIODA1, LOW);
   }

   //srovnej si strihy do zname pozice. 
   digitalWrite(PIN_STRIH1, LOW);
   digitalWrite(PIN_STRIH2, LOW);

   
   //a deme na to jyrko.
   
}

void loop() {
  //pokud je v laseru 1 hmota a zrovna nestrihas, spusti strih, rozni diodu a uloz si kdy to bylo. 
  if (!striha1 and digitalRead(PIN_LASER1) ==  LASER_PRERUSENY) {
    cas_strih1 = millis();
    digitalWrite(PIN_DIODA1, HIGH);
    striha1 = true;
    if (!laser1) {
      loguj("Zjistena hmota v laseru 1, spinam strih 1 na HIGH.");
      digitalWrite(PIN_STRIH1, HIGH);  
      laser1 = true;
    }else {
      loguj("Zjistena hmota v laseru 1, spinam strih 1 na LOW.");
      digitalWrite(PIN_STRIH1, LOW);  
      laser1 = false;
    }
  }
  //to same pro laser 2 
  if (!striha2 and digitalRead(PIN_LASER2) ==  LASER_PRERUSENY ) {
    cas_strih2 = millis();
    digitalWrite(PIN_DIODA2, HIGH);
    striha2 = true;
    if (!laser2) {
      loguj("Zjistena hmota v laseru 2, spinam strih 2 na HIGH.");
      digitalWrite(PIN_STRIH2, HIGH);  
      laser2 = true;
    }else {
      loguj("Zjistena hmota v laseru 2, spinam strih 2 na LOW.");
      digitalWrite(PIN_STRIH2, LOW);  
      laser2 = false;
    }
  }

  //pokud tedka striham a cas po strihu nektereho z laseru prekrocil nastaveny limit, spusti posun a zhasni strihaci diodu. 
  if (striha1 and millis() > cas_strih1 + DOBA_STRIHU) {
    loguj("Laser 1 dosahl casu strihu. Spoustim odsun 1.");
    cas_posun1 = millis();
    posouva1 = true;
    striha1 = false;
    digitalWrite(PIN_POSUN1, HIGH);
    digitalWrite(PIN_DIODA1, LOW);
  }
  if (striha2 and millis() > cas_strih2 + DOBA_STRIHU) {
    loguj("Laser 2 dosahl casu strihu. Spoustim odsun 2.");
    cas_posun2 = millis();
    posouva2 = true;
    striha2 = false;
    digitalWrite(PIN_POSUN2, HIGH);
    digitalWrite(PIN_DIODA2, LOW);
  }

  //pokud cas po zapnuti posunu prekrocil nastavenou konstantu nebo je cidlo odsunuti v tom stavu kde uz je odsunuto, vypni posun
  if (posouva1 and (millis() > cas_posun1 + DOBA_POSUNU or digitalRead(PIN_ODSUN1) == ODSUNUTO)) {
    loguj ("Doba posunu 1 dosahla casu posunu. Vypinam posun 1.");
    posouva1 = false;  
    digitalWrite(PIN_POSUN1, LOW);
  }
  if (posouva2 and (millis() > cas_posun2 + DOBA_POSUNU or digitalRead(PIN_ODSUN2) == ODSUNUTO)) {
    loguj ("Doba posunu 2 dosahla casu posunu. Vypinam posun 2.");
    posouva1 = false;
    digitalWrite(PIN_POSUN2, LOW);
  }

  
  //blikani diodama behem posunu. pokud nestriham ani neposouvam, diody vypni.
  if (posouva1 and (millis() - cas_posun1 ) % 400 > 200) {
    digitalWrite(PIN_DIODA1, HIGH);
  }else if (!striha1) {
    digitalWrite(PIN_DIODA1, LOW);
  }
  if (posouva2 and (millis() - cas_posun2) % 400 > 200) {
    digitalWrite(PIN_DIODA2, HIGH);
  }else if (!striha2){
    digitalWrite(PIN_DIODA2, LOW);
  }

  

  
  
  delay(CYKLYCKA_PRODLEVA);

}
