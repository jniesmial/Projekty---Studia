
#include "kartaSieciowa.h"
#include "komunikat_m.h"

Define_Module(KartaSieciowa);

void KartaSieciowa::initialize()
{
    // TODO - Generated method body
}

void KartaSieciowa::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void KartaSieciowa::activity(){
    Komunikat* komunikat;
    cQueue queue;

    for(;;){
        if(queue.isEmpty()){
            komunikat=(Komunikat*)receive();
        }
        else{
            komunikat=(Komunikat*)queue.pop();
        }

        cChannel *channel = gate("KSwyjscie")->getTransmissionChannel();
        simtime_t transFinishTime = channel->getTransmissionFinishTime();

        if(transFinishTime<=simTime()){
            send(komunikat,"KSwyjscie");
        }
        else{
            waitAndEnqueue(transFinishTime-simTime(), &queue);
            send(komunikat,"KSwyjscie");
        }
    }
}
