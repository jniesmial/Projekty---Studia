
#include "dysk.h"
#include "komunikat_m.h"

Define_Module(Dysk);

void Dysk::initialize()
{
    // TODO - Generated method body
}

void Dysk::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void Dysk::activity(){
    Komunikat* komunikat;
    cQueue queue;
    double czasTransmiji = par("czasTransmiji").doubleValue();
    double czasSzukania = par("czasSzukania").doubleValue();

    for(;;){
        if(queue.isEmpty()){
            komunikat = (Komunikat*) receive();
        }
        else{
            komunikat = (Komunikat*) queue.pop();
        }

        double time = czasSzukania + komunikat->getWielkoscZadania()*czasTransmiji / 4096.0;
        waitAndEnqueue(time, &queue);
        send(komunikat,"dyskWeWy$o");
    }


}
