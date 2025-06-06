#include <hFramework.h>
#include <DistanceSensor.h>
#include <math.h>

using namespace hModules;

int tick=10;
const int absTickLim=720;
int m=0;
int pull=0;
DistanceSensor sensor(hSens1);


int SilaNaciagu(){
    int distance = 0;
    int modul=0;
    //DistanceSensor sensor(hSens1);

    while(true){
        distance=sensor.getDistance();
        if (distance<0)
        {
            Serial.printf("Czujnik zwraca wartości ujemne !!!\r\n");
        }
        if (distance < 50 || distance > 150) {
            Serial.printf("Odległość poza zakresem: %d cm\r\n", distance);
        }
        else{
            //modul=distance%10;
            //distance-=modul;
            pull = ((distance - 50) / 5) + 22;

            Serial.printf("Dystans: %d cm\r\nObliczony naciąg: %d tików\r\n", distance, pull);

            return pull;
        }
        sys.delay(100);

    }
}

void Up(int up){
    /* zwiększanie nachylenia */
    hMot4.resetEncoderCnt();
    Serial.printf("automatyczne zwiekszanie kata!\r\n");
    if(up>=18 && up<32){
        hMot4.rotAbs(-800*tick,1000,false,5000);
        Serial.printf("Podniesiono na 8\r\n");
    }
    else if(up>=32 && up<40){
        hMot4.rotAbs(-900*tick,1000,false,5000);
        Serial.printf("Podniesiono na 9\r\n");
    }
    else if(up>=35){
        hMot4.rotAbs(-1000*tick,1000,false,5000);
        Serial.printf("Podniesiono na 10\r\n");
    }

}

void Down(){
    hMot4.rotAbs(0,1000,false,5000);
    Serial.printf("Opuszczono wiezyczke!\r\n");
}


void automatic(){
    //DistanceSensor sensor(hSens1);
    int naciag=0;
    int dis=0;
    int k=0;
    Serial.printf("Automatyczny tryb katapulty!\r\n");

    //podnoszenie zalezne od odleglosci
    naciag=SilaNaciagu();
    Up(naciag);

    //naciag zalezny od odleglosci
    int l=0;
    sys.delay(2000);

    for(int f=0; f<=1; f++){    //powoduje to dwa wystrzały na raz
        while(l<naciag)
        {   //ilość obrotow silnika naciagającego
            hMot2.rotRel(50,700,false,1000 ); //
            sys.delay(300);
            Serial.printf("...\r\n");
            l++;
        }
        Serial.printf("Przygotowano do wystrzału\r\n");
        k=1;

        /* kod do strzału */
        if (k!=1)
        {
            Serial.printf("Cos poszlo nie tak :(\r\n");
        }
        else
        {
            hMot1.rotAbs(-100, 600, false, 1000);
            Serial.printf("Wystrzelono pocisk\r\n");
            sys.delay(1000);
            hMot1.rotAbs(0,600,false,1000);
            k=0;
        }
    }

    /*opuszczanie*/
    Down();
    m=0;
    Serial.printf("Powrot do menu, jezeli chcesz strzelic jeszcze raz wroc tu!!\r\n");
    sys.delay(1000);
}

void automaticEdge()
{

    bool isDetecting=false;
    float dist1;
    float startPosition;
    float endPosition;
    float distance;
    float centerPosition;

    float lowerDetectionDistance=  50;
    float upperDetectionDistance= 150;

    while(true){
        distance= sensor.getDistance();
        hMot3.rotRel(tick,1000,false,500);
        Serial.printf("Odleglosc: %2f\r\n",distance);
        if (distance>=lowerDetectionDistance && distance<=upperDetectionDistance)
        {
            if (isDetecting==false){

            isDetecting=true;
            startPosition = hMot3.getEncoderCnt();
            dist1 = sensor.getDistance();
            Serial.printf("Wykryto krawędź, odległość: %2f, pozycja: %2f\r\n", dist1,startPosition);
            }

            endPosition = hMot3.getEncoderCnt();
            Serial.printf("Obiekt\r\n");
        }
        else
        {
            if (isDetecting==true)
            {
                isDetecting=false;
                centerPosition = (startPosition + endPosition)/2;
                hMot3.rotAbs(centerPosition, 500, true, INFINITE);
                distance= sensor.getDistance();
                Serial.printf("Katapulta została wycentrowana. start: %2f end:%2f odległość:%2f \r\n",startPosition, endPosition, distance);
                Serial.printf("Włączam tryb automatyczny wystrzału");
                automatic();
                break;
            }
            else
            {
                Serial.printf("Wykroczono poza zasięg odległości\r\n");
            }

        }
        sys.delay(500);
    }
}



//funkcja do manualnego przejęcia kontroli
void manual()
{
    int i=0;
    char steering;
    Serial.printf("Sterowanie manualne włączone \r\n");
    sys.delay(50);
    int Dist;

    while(true){
        if (Serial.available() > 0)
        {
            steering = Serial.getch();
            if (steering=='a')
            {
                /* obrót w lewo */
                Serial.printf("Obrót w lewo\r\n");
                hMot3.rotRel(tick,1000,false,500);
                continue;
            }
            else if (steering=='d')
            {
                /* obrót w prawo */
                Serial.printf("Obrót w prawo\r\n");
                hMot3.rotRel(-tick,1000,false,500);
                continue;
            }
            else if (steering=='w')
            {
                /* zwiększanie nachylenia */
                if(m!=11){
                Serial.printf("Zwiększanie nachylenia\r\n");
                hMot4.rotRel(-100*tick,1000,false,1000);
                m++;
                }
                else
                {
                    Serial.printf("Maksymalne nachylenie osiągnięte\r\n");
                    continue;
                }
            }
            else if (steering=='s')
            {
                if(m!=0)
                {
                    /* obniżanie nachylenia */
                    Serial.printf("Zmniejszanie nachylenia\r\n");
                    hMot4.rotRel(100*tick,1000,false,1000);
                    m--;
                }
                else
                {
                    Serial.printf("Minimalne nachylenie osiągnięte\r\n");
                    continue;
                }
            }

            else if (steering=='n')
            {   /* przygotowanie naciągu katapulty pod dokładne dane */
                if(i==1)
                {
                    Serial.printf("Naciąg jest już gotowy do wystrzału, wciśnij 'f' jeśli chcesz strzelić\r\n");
                }
                else
                {
                    int j=0;
                    hMot1.rotAbs(0,300,false,1000);

                    sys.delay(1000);

                    while(j<25){        //ilość obrotow silnika naciagającego
                        hMot2.rotRel(50,700,false,1000 ); //
                        sys.delay(300);
                        Serial.printf("...\r\n");
                        j++;
                    }
                    Serial.printf("Przygotowano do wystrzału\r\n");
                    i=1;
                }
            }

            else if (steering=='x')
            {   /* naciag o obrot 50 tickow */
                int g=0;
                if(i==1)
                {
                    Serial.printf("Naciąg jest już gotowy do wystrzału, wciśnij 'f' jeśli chcesz strzelić\r\n");
                }
                else
                {
                    bool stay=true;
                    Serial.printf("Manulane ustawienie naciągu\r\n");
                    Serial.printf("wciskaj 'x' aby zwiększać naciąg\r\n");

                    while(stay==true)
                    {
                        if(Serial.available() > 0)
                        {
                            steering=Serial.getch();
                            if(steering=='x')
                            {
                                if(g<80)
                                {
                                hMot2.rotRel(50,700,false,1000 ); //
                                sys.delay(100);
                                Serial.printf("...\r\n");
                                g++;
                                Serial.printf("Licznik :%d",g);
                                }
                            }
                            else if(steering=='q')
                            {
                                stay=false;
                                Serial.printf("Ustawianie naciągu zakończone\r\n");
                                i=1;
                            }
                        }
                    }
                }
            }

            else if (steering=='f')
            {   /* kod do strzału */
                if (i!=1)
                {
                    Serial.printf("Naciąg nie został przygotowany, naciśnij 'n' lub 'x' jeśli chcesz to zrobić\r\n");
                }
                else
                {
                hMot1.rotAbs(-110, 600, false, 1000);
                Serial.printf("Wystrzelono pocisk\r\n");
                sys.delay(1000);
                hMot1.rotAbs(0,600,false,1000);
                i=0;
                }

            }
            else if (steering=='l')
            {   /* pobieranie odległości */
                Dist=sensor.getDistance();
                if (Dist<0)
                {
                    Serial.printf("Czujnik zwraca wartości ujemne !!!\r\n");
                    continue;
                }
                else{
                    Serial.printf("Dystans: %d\r\n", Dist);
                }
            }
            else if (steering=='q')
            {   /* opuszczanie trybu */
                Serial.printf("Opuszczanie trybu manualnego...\r\n");
                sys.delay(500);
                Serial.printf("Wciśnij 'm' aby przejąć sterowanie manualnie lub 'a' do sterowania automatycznego:\r\n");
                break;
            }
        }
        else
            continue;

        }
        sys.delay(500);
}



void hMain()
{
    char reading;
    char reading2;
    bool stay=true;

    hMot1.setEncoderPolarity(Polarity::Reversed); // silnik 1 to sprzegło
    hMot1.setMotorPolarity(Polarity::Normal);

    hMot2.setEncoderPolarity(Polarity::Reversed); // silnik 2 to naciąg
    hMot2.setMotorPolarity(Polarity::Normal);

    hMot3.setMotorPolarity(Polarity::Normal);  // silnik 3 to obrót
    hMot3.setEncoderPolarity(Polarity::Reversed);

    hMot4.setMotorPolarity(Polarity::Normal);  // silnik 4 to kąt wieżyczki
    hMot4.setEncoderPolarity(Polarity::Reversed);

    hMot1.resetEncoderCnt();
    hMot2.resetEncoderCnt();
    hMot3.resetEncoderCnt();
    hMot4.resetEncoderCnt();


    while(true)
    {
        if (Serial.available() > 0)
        {
            Serial.printf("System wystrzeliwania pocisków\r\n");
            Serial.printf("Wciśnij 'm' aby przejąć sterowanie manualnie lub 'a' do sterowania automatycznego:\r\n");

            reading = Serial.getch();
            if (reading=='m' || reading=='M')
            {
                Serial.printf("Przechodzę w tryb manualny...\r\n");
                sys.delay(500);
                manual();
            }
            else if (reading=='a' || reading=='A')
            {
                Serial.printf("Przechodzę w tryb automatyczny...\r\n");
                sys.delay(500);
                automatic();
            }
             else if(reading=='b' || 'B')
            {
                Serial.printf("Przechodzę w tryb automatycznego wykrywania krawędzi...\r\n");
                sys.delay(500);
                automaticEdge();

                Serial.printf("Chcesz kontynuować wystrzał? Wciśnij 'y'. Jeśli nie, wciśnij 'q'\r\n");
                while(stay==true)
                {
                    if (Serial.available() > 0)
                    {
                        reading2 = Serial.getch();
                        if (reading2=='q')
                        {
                            stay=false;
                            Serial.printf("Opuszczanie trybu automatycznego wykrywania krawędzi...\r\n");
                            sys.delay(500);
                        }
                        else if (reading2=='y')
                        {
                            Serial.printf("Przechodzę do wystrzału...\r\n");
                            sys.delay(500);
                            automatic();
                            stay=false;
                        }
                        else
                        {
                            Serial.printf("Nie rozpoznano komendy, wciśnij 'y' lub 'q'\r\n");
                            continue;
                        }
                    }

                }
            }
            else
            {
                continue;
            }

        }
    }
}