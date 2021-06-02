#include <SPI.h>// includo le varie librerie necessarie
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Buzzer.h>
                              //definisco i pin dei due lettori RFID
#define RST_PIN         10    
#define SS_1_PIN        9        
#define SS_2_PIN        8        
#define NR_OF_READERS   2     //indico il numero dei lettori collegati


MFRC522 mfrc522[NR_OF_READERS];             
Buzzer buzzer (A4);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int ref[4]= {121,151,02,160 };    //Array di grandezza 4 con il primo byte del codice dei tag rf
byte pstanza1 [4];                //Array che salva con quale tag qualcuno è entrato nella stanza 1
byte pstanza2 [4];                //Array che salva con quale tag qualcuno è entrato nella stanza 1
int reader;
byte ssPins[] = {SS_1_PIN, SS_2_PIN};

int luci1=A3;           //I pin dei led delle stanze
int luci2=A2;
int count;              //il contatore utilizzato per le persone totali nell'ufficio
int count1=0;           //contatore delle persone nella stanza 1
int count2=0;           //contatore delle persone nella stanza 2
int temp;               //la temperatura dell'altoforno
int bt;                 //la variabile che indica quante volte è sato premuto il pulsante
int oldstate=1;         //variabile necessaria per pulire l'lcd quando si cambia schermata

void dump_byte_array(byte *buffer, byte bufferSize) { 
/* for (byte i = 0; i < bufferSize; i++) {          //La funzione legge i dati dal tag
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");    //Questa ciclo for stampa sulla seriale il codice del tag   
    Serial.print(buffer[i]);                        //È stato eliminato in quanto necessario solo in fase di sviluppo
 
    
  }*/
if(reader==1){                          //l'if controlla di quale lettore rf si tratta
    for(int i=0;i<4;i++){
      
        if(buffer[0]==pstanza1[i]){         //in questo ciclo se il primo byte letto dall'rfid è già presente 
        pstanza1[i]=0;                      //nell'array della stanza 1 allora viene eliminato
        //Serial.print(pstanza1[i]);
       
    }
     else if(buffer[0]==ref[i]&&pstanza1[i]==0){
        pstanza1[i]=buffer[0];}             //altrimenti viene salvato nell'array
       // Serial.print(pstanza1[i]);
      }
  }
else if(reader==0){                         //Stesso procedimento eseguito però per la stanza 2
    for(int i=0;i<4;i++){
      
        if(buffer[0]==pstanza2[i]){
        pstanza2[i]=0;
        
        //   Serial.print(pstanza2[i]);
       
    }
     else if(buffer[0]==ref[i]&&pstanza2[i]==0){
        pstanza2[i]=buffer[0];}
        //Serial.print(pstanza2[i]);
      }
  }
 
}


void bip(){              //Questa funzione serve semplicemente per generare un suonodal buzzer
  buzzer.begin(10);      //con precisione un Sol della 6 ottava

  buzzer.sound(NOTE_G6,70);
    }


void temperatura(){           //Questa funzione si occupa della comunicazione seriale e della lettura e scrittura della temperatura sul display
    temp=Serial.read();       //Legge il valore dalla seriale e lo assegna alla variabile temp
    lcd.setCursor(3, 0);
    lcd.print("Temperatura");
    lcd.setCursor(0, 1);
    lcd.print(temp*80 );      //Scrive la temperatura sul display
    Serial.println(temp);
    delay(100);//È stata moltiplicata per 80 per dare valori più realisitci ad un altoforno
  }
  
 void Lavoratori(){           //La funzione scrive il numero di lavoratori presenti sul display lcd
    lcd.setCursor(0,0);
    lcd.print("Lavoratori");
    lcd.setCursor(0,1);
    lcd.print("Presenti");
    lcd.setCursor(11,1);
    lcd.print(count1+count2);
    }










void setup() {
  
  Serial.begin(9600);              //inizializzo la comunicazione seriale
  SPI.begin();                  
  lcd.begin(16, 2);
  buzzer.begin(0);
  
  pinMode(A1,INPUT_PULLUP);        //il pin A1, pin collegato al pulsante, è settato inputpullup per comodità nella realizzazione del circuito

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {      //il ciclo controlla se ci sono collegati dei lettori RF 
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);              //nel caso sono collegati male lo indica
   // Serial.print(F("Reader ")); 
   // Serial.print(reader);
    //Serial.print(F(": "));
    //mfrc522[reader].PCD_DumpVersionToSerial();   
  }
}


void loop() {
  for (reader = 0; reader < NR_OF_READERS; reader++) {                      //il ciclo controlla se sono presenti nuove carte
                                                                            //e nel caso richiama la funzione per leggere le informazioni

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
    bip();
     // Serial.print(F("Reader "));                                                         //I serial print cancellati semplicemente stampavano sul monitor seriale le informazioni lette
     // Serial.print(reader);   // Show some details of the PICC (that is: the tag/card)    //cosa non necessaria a progetto completo
     // Serial.println(F(": Card UID:"));
      
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      //Serial.println();
 
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
      
delay(500);
    }

for(int i=0;i<4;i++){                       //il ciclo controlla se sono presenti dei valori, quindi persone, nell'array della stanza 1
  if(pstanza1[i]!=0&& count1<4){            //nel caso ci siano aumenta il count1
  count1=count1+1;
 }

  else if(pstanza1[i]==0 && count1!=0){     //altrimenti lo diminuisce
    count1=count1;}
}
if(count1!=0){                              //L'if accende o spegne la luce nella stanza 1 se è presente qualcuno
  digitalWrite(A2,HIGH);
  }
  else{
  digitalWrite(A2,LOW);
  }

for(int i=0;i<4;i++){                      //Stesso procedimento per la stanza 2
  if(pstanza2[i]!=0&& count2<4){
  count2=count2+1;

 }

  else if(pstanza2[i]==0 && count2!=0){
    count2=count2;
    }
}
if(count2!=0){
  digitalWrite(A3,HIGH);
  }
  else{
    digitalWrite(A3,LOW);
  }


switch (bt){                  //Per evitare di utilizzare molti if è stato utilizzato uno switch con il valore bt
  case 0:                    
     if(oldstate==3){         //questo if controlla se il display ha appena cambiato schermata e nel caso lo pulisce
      lcd.clear();            //per evitare glitch grafici
      oldstate=1;
    
     }
    temperatura();            //viene richiamata la funzione per la temperatura
    break;                    //la prima schermata è la temperatura dell'altoforno

  case 1:                     
    if(oldstate==1){        
      lcd.clear();
      oldstate=2;
     }    
    lcd.setCursor(2,0);           //La seconda schermata scrive sul display la quantità di carbon coke rimanente
    lcd.print("CC Restante");     //anche se non viene effettivamente misurata è presente per riempire spazio 
    lcd.setCursor(0,1);           //e potrebbe essere in futuro utilizzata per altri scopi
    lcd.print("4576 kg");

    break;

  case 2:   
      if(oldstate==2){
      lcd.clear();
      oldstate=3;
     }
     Lavoratori();              //Viene richiamata la funzione Lavoratori che scrive i lavoratori presenti nell'ufficio
    break;  
   
    
  case 3:
  
    bt=0;                        //reset valore di bt            
    break;
  
}
  
   if(digitalRead(A1)==LOW){    //L'if controlla se il pulsante viene premuto e, nel caso, aumenta il valore di bt
    bt=bt+1;
    delay(1000);

}
count1=0;                   //reset del contatora stanza 1 e 2
count2=0;                   

}
}
