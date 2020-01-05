    //Includo le librerie dei sensori, del ricevitore infrarossi e del display LCD
    #include <Adafruit_Sensor.h> 
    #include <DHT.h>
    #include <DHT_U.h>
    #include <Wire.h>
    #include <LiquidCrystal_I2C.h>
    #include <IRremote.h>
    
    LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //imposto il percorso del display e i suoi pin per utilizzarlo tramite modulo i2c

    //imposto il sensore e il suo pin (temperatura e umidità)
    
    #define DHTPIN            A0         // Pin al quale è connesso il sensore
    #define DHTTYPE           DHT22     // DHT 22 (AM2302) 
    
    
    float um,temp; //variabili di umidità e temperatura
    
    //array da scorrere per i valori massimi e minimi settabili di temperatura e umidità
    int tempmodmax[16]={15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30},ummodmax[26]={50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100};
    int tempmodmin[16]={15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30},ummodmin[26]={50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100};
    
    //settaggio dei relè
    int in1 = 7;
    int in2 = 6;
    int in3 = 5;
    int in4 = 3;
    
    int FLAG=0,FLAG2=0; //FLAG per l'utilizzo dei cicli nella modalità a comandi manuali
    
    int i=10,g=7,e=12,f=18,k; //valori iniziali (negli array) per umidità e temperatura minima e massima
    
    int RECV_PIN = 2; //pin alla quale è collegato il ricevitore ad infrarossi
    
    unsigned long Ti,Ta; //variabili per la temporizzazione
    
    IRrecv irrecv(RECV_PIN); //Pulsante che ha inviato il dato al ricevitore
    decode_results results; //Valore ricevuto dal sensore
     
    
    //imposto il sensore e il suo pin (temperatura e umidità)
    
    #define DHTPIN            A0         // Pin al quale è connesso il sensore
    #define DHTTYPE           DHT22     // DHT 22 (AM2302)
    
    //imposto ogni pulsante del telecomando 
    #define CH1   0xFFA25D
    #define CH    0xFF629D
    #define CH2   0xFFE21D
    #define prev  0xFF22DD
    #define avan  0xFF02FD
    #define pausa 0xFFC23D
    #define men   0xFFE01F
    #define piu   0xFFA857
    #define EQ    0xFF906F
    #define P0    0xFF6897
    #define P100  0xFF9867
    #define P200  0xFFB04F
    #define P1    0xFF30CF
    #define P2    0xFF18E7
    #define P3    0xFF7A85
    #define P4    0xFF10EF
    #define P5    0xFF38C7
    #define P6    0xFF5AA5
    #define P7    0xFF42BD
    #define P8    0xFF4AB5
    #define P9    0xFF52AD
    DHT_Unified dht(DHTPIN, DHTTYPE);
   
    //Setup del programma iniziale
    void setup() {
       
       irrecv.enableIRIn(); //abilito la ricezione dei segnali da parte del ricevitore
       
       dht.begin(); //Abilito il sensore di temperatura e umidità
       sensor_t sensor; //Variabile del sensore
       dht.temperature().getSensor(&sensor); //controllo il sensore di temperatura
       dht.humidity().getSensor(&sensor); //controllo il sensore di umidità
      
       lcd.begin(16,2); //inizializzo il display LCD
       lcd.backlight(); //accendo la luce di sfondo del display LCD
       
       
       //imposto i relè
       pinMode(in1, OUTPUT);  //ventole
       digitalWrite(in1, LOW);
       pinMode(in2, OUTPUT);   //led
       digitalWrite(in2, HIGH);
       pinMode(in3, OUTPUT);   //pompa
       pinMode(in4, OUTPUT);   //riscaldamento
       digitalWrite(in4, LOW);
       
       Ti=millis(); //Metto nella variabile Ti quanti millisecondi sono passati dall'accensione del programma
       //ciclo di start per mandare la serra a regime
       while(1)
        { 
          lcd.setCursor(2,0);
          lcd.print("Indipendent");
          lcd.setCursor(2,1);
          lcd.print("GreenHouse");
          //accendo la pompa
          digitalWrite(in3, HIGH); 
          //aspetto il tempo necessario per mandare a regime la serra
          delay(5000);
          //esco dal ciclo di preparazione
          break;   
        }
       digitalWrite(in3,LOW);  //spengo la pompa
       lcd.clear();  
    }
    //Programma
    void loop() {
        FLAG=0;FLAG2=0;
        Ta=millis(); //controllo il tempo attuale
        /*controllo quanto tempo è passato
         se sono passati X minuti allora è tempo del ciclo notturno
         */
        if((Ta-Ti)>=10000)       // 46800000 ms -> 13 ore
            {digitalWrite(in2, LOW); //spengo le luci
            }
        //controllo il tempo attuale
        Ta=millis(); 
        /*controllo quanto tempo è passato
        se sono passati X minuti allora è tempo del ciclo giornaliero*/
        if((Ta-Ti)>=15000)    //39600000 ms -> 11 ore
          {
            Ti=Ta;   //metto Ti=Ta per avere sempre lo stesso tempo da misurare
            digitalWrite(in2, HIGH); //accendo le luci
          }
        /*controllo se è tempo dell'irrigazione giornaliera*/
        if(((Ta-Ti)>=11000)&&(Ta-Ti)<=12500)
          {
            digitalWrite(in3,HIGH); //attivo la pompa
            delay(2000); //aspetto il tempo necessario per irrigare
            digitalWrite(in3,LOW); //spengo la pompa
          }
      
        sensors_event_t event; 
      
        dht.temperature().getEvent(&event); //controllo la temperatura
        temp=event.temperature-2; //?da mettere a confronto con un termometro? metto in una variabile float il dato ricevuto dal sensore
        if(isnan(event.temperature)) { //se non riceve nulla scrive sul display "errore"
           
            lcd.setCursor(0,0);
            lcd.print("Errore");
          }
        else{//altrimenti scrivo la temperatura
       
            lcd.setCursor(0,0);
            lcd.print("Temp: ");
            lcd.print(temp);
          }
        dht.humidity().getEvent(&event); //controllo l'umidità
        um=event.relative_humidity; //metto il dato in una variabile float
        if(isnan(event.relative_humidity)) { //se non ricevo il dato scrivo sul display "errore"
            
            lcd.setCursor(0,1);
            lcd.print("Errore");
          }
        else {//altrimenti scrivo l'umidità
        
            lcd.setCursor(0,1);
            lcd.print("Umidita': ");
            lcd.print(um);
            lcd.print("%");
          }
     
        if((temp<=tempmodmin[g])||(um<=ummodmin[e])) //se la temperatura e/o l'umidità sono minori di quelle stabilite accendo il riscaldamento e spengo la ventola
          {
            digitalWrite(in4, HIGH); //riscaldamento ON
            digitalWrite(in1, LOW); // ventola OFF
          }
        else{ //altrimenti sono a temperatura e umidità valide
            digitalWrite(in4, LOW); //riscaldamento OFF
            digitalWrite(in1, LOW); // ventola OFF
          }
     
        if((temp>=tempmodmax[i])||(um>=ummodmax[f])) //se la temperatura e/o l'umidità sono maggiori di quelle stabilite spengo il riscaldamento e accendo la ventola
          {
            digitalWrite(in4, LOW); //riscaldamento OFF
            digitalWrite(in1, HIGH); // Ventola ON
          }
        else{ //altrimenti sono a temperatura e umidità valide
            digitalWrite(in4, LOW); //riscaldamento OFF
            digitalWrite(in1, LOW); // ventola OFF
          }
    
        //parte manuale
    
        if (irrecv.decode(&results))  //se ricevo qualcosa dal telecomando infrarossi:
          { lcd.clear(); 
            //visualizzo sul display in che modalità mi trovo
            lcd.setCursor(0,0); 
            lcd.print("Mod. Comandi");
            lcd.setCursor(0,1);
            lcd.print("Manuali");
            delay(1000); //aspetto un secondo
            lcd.clear();
            lcd.setCursor(0,0);
            //visualizzo il range di temperatura e umidità attuali
            lcd.print("T range: ");  
            lcd.print(tempmodmin[g]);
            lcd.print("-");
            lcd.print(tempmodmax[i]);    
            lcd.setCursor(0,1);
            lcd.print("Um range: ");
            lcd.print(ummodmin[e]);
            lcd.print("-");
            lcd.print(ummodmax[f]);
            lcd.print("%");
            //entro in un ciclo per la gestione dei pulsanti
            while(FLAG!=1)
                {FLAG2=0;
                 if (irrecv.decode(&results)) //controllo il pulsante che è stato premuto
                    {
                      if (results.value == CH1) //se viene premuto il pulsante CH1 finisce il ciclo e torno in modalità automatica
                        {lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Modalita'");
                         lcd.setCursor(0,1);
                         lcd.print("Automatica");
                         delay(1000);
                         FLAG=1;}
                      if (results.value == P0) //se viene premuto il pulsante P0 accendo la ventola per un tempo determinato e poi la spengo
                        {lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Ventola ON");
                         digitalWrite(in1,HIGH);
                         delay(5000);
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Ventola OFF");    
                         digitalWrite(in1,LOW);
                         delay(500);
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Mod. Comandi");
                         lcd.setCursor(0,1);
                         lcd.print("Manuali");            
                        }
                      if (results.value == P100) //se viene premuto il pulsante P100 entro nel ciclo di modifica della temperatura
                        { 
                         //visualizzo sul display che sono in modalità Temperature Settings
                         lcd.clear();
                         lcd.setCursor(0,0);
                         lcd.print("Temperature");
                         lcd.setCursor(0,1);
                         lcd.print("Settings");
                         while(FLAG2!=1) //entro nel ciclo di modifica della temperatura
                          {
                           if (irrecv.decode(&results)) //controllo che pulsante viene premuto
                            {
                             if (results.value == prev) //se viene premuto il p. prev diminuisco la temperatura minima 
                              { 
                               if (g>=0)
                                {
                                 g--;      
                                 lcd.clear();
                                 lcd.setCursor(0,0);
                                 lcd.print("Tmin set= " );
                                 lcd.print(tempmodmin[g]);
                                }      
                              }      
                             if (results.value == avan) //se viene premuto il pulsante avan aumento la temperatura minima
                              { 
                               if (g<=16)
                                {
                                 g++;      
                                 lcd.clear();
                                 lcd.setCursor(0,0);
                                 lcd.print("Tmin set= " );
                                 lcd.print(tempmodmin[g]);
                                }     
                              }
                             if (results.value == men) //se viene premuto il pulsante men diminuisco la temperatura massima
                              {
                               if (i>=0)
                                {
                                 i--;      
                                 lcd.clear();
                                 lcd.setCursor(0,0);
                                 lcd.print("Tmax set= " );
                                 lcd.print(tempmodmax[i]);
                                }      
                              }      
                             if (results.value == piu) //se viene premuto il pulsante piu aumento la temperatura massima
                              { //tempmax i
                               if (i<=16)
                                {
                                 i++;     
                                 lcd.clear();
                                 lcd.setCursor(0,0);
                                 lcd.print("Tmax set= " );
                                 lcd.print(tempmodmax[i]);
                                }  
                              }
                             if (results.value == CH2) //se viene premuto il pulsante CH2 esco dall'impostazione manuale della temperatura
                              {                               
                               lcd.clear();
                               lcd.setCursor(0,0);
                               lcd.print("Mod. Comandi'");
                               lcd.setCursor(0,1);
                               lcd.print("Manuali");
                               delay(1000);
                               lcd.clear();
                               lcd.setCursor(0,0);
                               lcd.print("T range: ");  
                               lcd.print(tempmodmin[g]);
                               lcd.print("-");
                               lcd.print(tempmodmax[i]);    
                               lcd.setCursor(0,1);
                               lcd.print("Um range: ");
                               lcd.print(ummodmin[e]);
                               lcd.print("-");
                               lcd.print(ummodmax[f]);
                               lcd.print("%");
                               FLAG2=1;
                              }
                             irrecv.resume(); 
                           }
                          }
                         }   
                        if (results.value == P200) //se viene premuto il pulsante P200 entro nel ciclo di modifica dell'umidità
                         { 
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Humidity");
                          lcd.setCursor(0,1);
                          lcd.print("Settings");
                          while(FLAG2!=1)
                              {
                               if (irrecv.decode(&results)) //controllo se viene premuto un pulsante
                                {
                                 if (results.value == prev)  //se viene premuto il p. prev diminuisco l'umidità minima
                                  { 
                                   if (e>=0)
                                    {
                                     e--;     
                                     lcd.clear();
                                     lcd.setCursor(0,0);
                                     lcd.print("Umin set= " );
                                     lcd.print(ummodmin[e]);
                                    }      
                                  }      
                                 if (results.value == avan)    //se viene premuto il pulsante avan aumento l'umidità minima
                                  { 
                                   if (e<=26)
                                    {
                                     e++;      
                                     lcd.clear();
                                     lcd.setCursor(0,0);
                                     lcd.print("Umin set= " );
                                     lcd.print(ummodmin[e]);
                                    }     
                                  }
                                 if (results.value == men)   //se viene premuto il pulsante men diminuisco l'umidità massima
                                  { 
                                   if (f>=0)
                                    {
                                     f--;      
                                     lcd.clear();
                                     lcd.setCursor(0,0);
                                     lcd.print("Umax set= " );
                                     lcd.print(ummodmax[f]);
                                    }     
                                  }      
                                 if (results.value == piu)   //se viene premuto il pulsante piu aumento l'umidità massima
                                  { 
                                   if (f<=26)
                                    {
                                     f++;      
                                     lcd.clear();
                                     lcd.setCursor(0,0);
                                     lcd.print("Umax set= " );
                                     lcd.print(ummodmax[f]);
                                    }
                                  }
                                 if (results.value == CH2)   //se viene premuto il pulsante CH2 esco dall'impostazione manuale dell'umidità
                                  { 
                                   lcd.clear();
                                   lcd.setCursor(0,0);
                                   lcd.print("Mod. Comandi'");
                                   lcd.setCursor(0,1);
                                   lcd.print("Manuali");
                                   delay(1000);
                                   FLAG2=1;
                                   lcd.clear();
                                   lcd.setCursor(0,0);
                                   lcd.print("T range: ");  
                                   lcd.print(tempmodmin[g]);
                                   lcd.print("-");
                                   lcd.print(tempmodmax[i]);    
                                   lcd.setCursor(0,1);
                                   lcd.print("Um range: ");
                                   lcd.print(ummodmin[e]);
                                   lcd.print("-");
                                   lcd.print(ummodmax[f]);
                                   lcd.print("%");}
                                   irrecv.resume(); 
                                }
                              }
                          }    
          
                        if (results.value == P1)    //se viene premuto il pulsante P1 accendo l'illuminazione per un tempo determinato e poi la spengo
                         { 
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("LED ON");
                          digitalWrite(in2,HIGH);
                          delay(5000);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("LED OFF");
                          digitalWrite(in2,LOW);
                          delay(500);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Mod. Comandi");
                          lcd.setCursor(0,1);
                          lcd.print("Manuali");     
                         }
                        if (results.value == P2)  //se viene premuto il pulsante P2 accendo la pompa per un tempo determinato e poi la spengo
                         {
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Pompa ON");
                          digitalWrite(in3,HIGH);
                          delay(5000);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Pompa OFF");                                
                          digitalWrite(in3,LOW);
                          delay(500);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Mod. Comandi");
                          lcd.setCursor(0,1);
                          lcd.print("Manuali");         
                         }
                        if (results.value == P3) //se viene premuto il pulsante P3 accendo il riscaldamento per un tempo determinato e poi lo spengo
                         { 
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Riscaldamento ");
                          digitalWrite(in4,HIGH);
                          lcd.setCursor(0,1);
                          lcd.print("ON");
                          delay(5000);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Riscaldamento");
                          lcd.setCursor(0,1);
                          lcd.print("OFF");
                          digitalWrite(in4,LOW);
                          delay(500);
                          lcd.clear();
                          lcd.setCursor(0,0);
                          lcd.print("Mod. Comandi");
                          lcd.setCursor(0,1);
                          lcd.print("Manuali");      
                         }
                        irrecv.resume(); 
                     }
                 }
              irrecv.resume(); 
           } 
        }
    
