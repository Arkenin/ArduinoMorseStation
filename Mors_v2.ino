const int nadajnik = 10;
static uint32_t oldtime=millis();
uint8_t mess1[] = {0};
//Wiadomości do nadania przez 3 różne stacje
char tex1[100] = "lubie placki";
char tex2[100] = "naprawde to ktos czyta";
char tex3[100] = "pisanie na sniadanie";


//Struktura stacji nadawczej morse'a - na każdym z pinów wiadomości będą nadawane jednocześnie
 typedef struct stacja {
  int pin = -1;					//Numer pinu wyjściowego
  
  uint8_t t=0;					//Czas utrzymania wiadomości
  bool aktywna=0;
  bool czyWysoki=0;
  
  char znak;
  char* wskZnak=0;				//Wskaźnik do aktualnie nadawanego znaku
  bool actZnak=0;
  uint8_t kropka;
  uint8_t* wskKropka=0;			//Wskaźnik do aktualnie nadawanej części znaku
  bool actKropka=0;

  uint8_t malyBufor[6]{};
  

 }Stacja;

uint8_t kod_mors[40][6]= {		//Tablica z zakodowanymi literkami, każy wiersz odpowiada literze, na samym dole zostały zakodowane znaki specjalne
1,2,0,0,0,0,					//0 - koniec wiadomości, 1 - kropka, 2 - kreska
2,1,1,1,0,0,
2,1,2,1,0,0,
2,1,1,0,0,0,
1,0,0,0,0,0,
1,1,2,1,0,0,
2,2,1,0,0,0,
1,1,1,1,0,0,
1,1,0,0,0,0,
1,2,2,2,0,0,
2,1,2,0,0,0,
1,2,1,1,0,0,
2,2,0,0,0,0,
2,1,0,0,0,0,
2,2,2,0,0,0,
1,2,2,1,0,0,
2,2,1,2,0,0,
1,2,1,0,0,0,
1,1,1,0,0,0,
2,0,0,0,0,0,
1,1,2,0,0,0,
1,1,1,2,0,0,
1,2,2,0,0,0,
2,1,1,2,0,0,
2,1,2,2,0,0,
2,2,1,1,0,0,

2,2,2,2,2,0,
1,2,2,2,2,0,
1,1,2,2,2,0,
1,1,1,2,2,0,
1,1,1,1,2,0,
1,1,1,1,1,0,
2,1,1,1,1,0,
2,2,1,1,1,0,
2,2,2,1,1,0,
2,2,2,2,1,0,

3,3,0,0,0,0
};


int dot = 300;			//czas trwania kropki
int dot3 = 3*dot;		//czas trwania kreski
int dot7 = 7*dot;		//czas trwania przerwy między słowami
bool state = 0;


void nadaj(uint8_t mess[]); //deklaracja funkcji nadaj
void tick();				//TODO


  Stacja niebieska;
  Stacja czerwona;
  Stacja zielona;
  
  Stacja mala;
  
  Stacja* wskNiebieska = &niebieska;
  Stacja* wskCzerwona = &czerwona;
  Stacja* wskZielona = &zielona;
  
  Stacja* wskMala = &mala;

void setup() {
  // Ustawienie pinów, które będą wykorzystywane do nadawania jako wyjścia
  pinMode(nadajnik, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  
  
  Serial.begin(57600);

  //wskNiebieska = &niebieska;
  //wskMala = &mala;
  
  //Przypisanie pinów do stacji
  mala.pin = 13;
  niebieska.pin = 10;
  czerwona.pin = 11;
  zielona.pin = 12;

  //nadaj(mess1);
  delay(1000);

}

void loop() {


    if (Serial.available()) {   //Fragment do debugowania, wyświetla na porcie szeregowym aktualnie nadawane literki
    int inByte = Serial.read();
    Serial.println();
    Serial.print("t: ");
    Serial.print(niebieska.t);
    Serial.print(" A: ");
    Serial.print(niebieska.aktywna);
    Serial.print(" W: ");
    Serial.println(niebieska.czyWysoki);
    Serial.print(" Znak: ");
    Serial.println(niebieska.znak);
    
    
    }
/* 
//W związku z tym, że nie jest pragmowane na czystym AVR,
przerwanie od czasu jest realizowane tutaj za pomocą funkcji
milis(). Czas został ustawiony na sztywno na wartość 300ms
*/


  if ( (millis()-oldtime) >= 300) 	
    {
        oldtime = millis();
  
        stacjaNadaj (wskNiebieska, tex1); //3 różne wskaźniki do 3 różnych stacji, każda nadaje inną wiadomość na innym pinie
        stacjaNadaj (wskCzerwona, tex2);
        stacjaNadaj (wskZielona, tex3);
              
       // stacjaNadaj (wskMala, tex2);
       digitalWrite(13,state);
       state=!state;
         
    }
}


/* 
Funkcja odpowiedzialna za nadawanie wiaodmości

*/


void stacjaNadaj (Stacja* sta, char mess[])
{

    if(sta->t>0)
    {
      sta->t = sta->t-1; //jeżeli czas t>0 nic nie rób > trwa nadawanie aktualnej kropki/kreski
      return;
    }

    if (sta->aktywna==0)
    {
      sta->aktywna = 1; 	//uruchomienie stacji
      sta->wskZnak=mess; 	//pobranie pierwszego znaku do nadania z wiadomości "mess"
    }
    
  if (!sta->actZnak)	//Sprawdzanie czy znak jest nadawany
  {
      if (sta->znak = *(sta->wskZnak++))	//Pobranie znaku i podniesienie wskaźnika na kolejny znak
      {
         
        sta->actZnak = 1;
        Serial.print(sta->znak);
        
        switch (sta->znak) //Case wybiera sekwencję do nadania z tablicy "kod_mors"
        {
          
          case 'a': sta->wskKropka = kod_mors[0]; break;
          case 'b': sta->wskKropka = kod_mors[1]; break;
          case 'c': sta->wskKropka = kod_mors[2]; break;
          case 'd': sta->wskKropka = kod_mors[3]; break;
          case 'e': sta->wskKropka = kod_mors[4]; break;
          case 'f': sta->wskKropka = kod_mors[5]; break;
          case 'g': sta->wskKropka = kod_mors[6]; break;
          case 'h': sta->wskKropka = kod_mors[7]; break;
          case 'i': sta->wskKropka = kod_mors[8]; break;
          case 'j': sta->wskKropka = kod_mors[9]; break;
          case 'k': sta->wskKropka = kod_mors[10]; break;
          case 'l': sta->wskKropka = kod_mors[11]; break;
          case 'm': sta->wskKropka = kod_mors[12]; break;
          case 'n': sta->wskKropka = kod_mors[13]; break;
          case 'o': sta->wskKropka = kod_mors[14]; break;
          case 'p': sta->wskKropka = kod_mors[15]; break;
          case 'q': sta->wskKropka = kod_mors[16]; break;
          case 'r': sta->wskKropka = kod_mors[17]; break;
          case 's': sta->wskKropka = kod_mors[18]; break;
          case 't': sta->wskKropka = kod_mors[19]; break;
          case 'u': sta->wskKropka = kod_mors[20]; break;
          case 'v': sta->wskKropka = kod_mors[21]; break;
          case 'w': sta->wskKropka = kod_mors[22]; break;
          case 'x': sta->wskKropka = kod_mors[23]; break;
          case 'y': sta->wskKropka = kod_mors[24]; break;
          case 'z': sta->wskKropka = kod_mors[25]; break;
          case '0': sta->wskKropka = kod_mors[26]; break;
          case '1': sta->wskKropka = kod_mors[27]; break;
          case '2': sta->wskKropka = kod_mors[28]; break;
          case '3': sta->wskKropka = kod_mors[29]; break;
          case '4': sta->wskKropka = kod_mors[30]; break;
          case '5': sta->wskKropka = kod_mors[31]; break;
          case '6': sta->wskKropka = kod_mors[32]; break;
          case '7': sta->wskKropka = kod_mors[33]; break;
          case '8': sta->wskKropka = kod_mors[34]; break;
          case '9': sta->wskKropka = kod_mors[35]; break;
          case ' ': sta->wskKropka = kod_mors[36]; break;
               
       }
      
    }
    else
    {
      sta->aktywna = 0;
      sta->actZnak = 0;
    }
  
  }


 

  
  if (sta->kropka = *(sta->wskKropka)) //Sprawdz czy wartosc wskaznika to 0 czy 1, jeżeli koniec nadawana jest jeszcze przerwa
  {

    if (sta->actKropka==0) //sprawdzanie czy skończyło się nadawanie aktualnej kropki
    {
    sta->wskKropka++; //pobranie kolejnej kropki do nadania
    sta->actKropka = 1; //Zmienna pokazuje że trwa nadawanie kropki
    }
    
    switch (sta->kropka) //w zależności od rodzaju kropki wykonaj różne sekwencje
      {
      case 1: //krótki syngał kropki, składa się z samej kropki i przerwy przewidzianej dla niej
      if (sta->czyWysoki)
      {
      digitalWrite(sta->pin,0);
      sta->czyWysoki = 0;
      sta->actKropka = 0;
      sta->t = 0;
      }
      else
      {
      digitalWrite(sta->pin,1);
      sta->czyWysoki = 1;
      sta->t = 0;        
      }
      break;
      
      
      case 2: //długi sygnał kreski
      if (sta->czyWysoki)
      {
      digitalWrite(sta->pin,0);
      sta->czyWysoki = 0;
      sta->actKropka = 0;
      sta->t = 0;
      }
      else
      {
      digitalWrite(sta->pin,1);
      sta->czyWysoki = 1;
      sta->t = 2;        
      }
      break;

      case 3: //sygnał specjalnej przerwy
      digitalWrite(sta->pin,0);
      sta->czyWysoki = 0;
      sta->actKropka = 0;
      sta->t = 3;
      break;


      }
  } //koniec if
  else
  {
    digitalWrite(sta->pin,0);
    sta->czyWysoki = 0;
    sta->actKropka = 0;
    sta->t = 2;
	sta->actKropka = 0;
	sta->actZnak = 0;
  }

  
}//koniec funkcji
