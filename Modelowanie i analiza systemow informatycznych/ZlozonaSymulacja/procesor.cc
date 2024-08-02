
#include "procesor.h"
#include "komunikat_m.h"

Define_Module(Procesor);

void Procesor::initialize()
{
    // TODO - Generated method body
}

void Procesor::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void Procesor::activity(){
    Komunikat *komunikat;
    cQueue queue;
    double czasTransmiji = par("czasTransmiji").doubleValue();
    double czasObslugi = par("czasObslugi").doubleValue();

    for(;;){

        //sprawdzanie kolejki
        if(queue.isEmpty()){
            komunikat = (Komunikat*)receive();
        }
        else{
            komunikat = (Komunikat*)queue.pop();
        }

        //sprawdzanie skad jest komunikat i wyslij go w dobra strone
        if(komunikat->arrivedOn("procWeWy$i")){
            double time = czasObslugi + komunikat->getWielkoscZadania() * czasTransmiji/512.0;
            waitAndEnqueue(time, &queue);
            if(komunikat->getAdresNadawczy()==1){
                send(komunikat, "procWyjscie1");
            }
            else{
                send(komunikat, "procWyjscie2");
            }
        }
        else{
            send(komunikat, "procWeWy$o");
        }
    }//end for
}
