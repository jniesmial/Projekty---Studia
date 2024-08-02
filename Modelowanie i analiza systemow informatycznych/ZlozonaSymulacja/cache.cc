
#include "cache.h"
#include "komunikat_m.h"

Define_Module(Cache);

void Cache::initialize()
{
    // TODO - Generated method body
}

void Cache::handleMessage(cMessage *msg)
{
   Komunikat* komunikat = (Komunikat*)msg;
   //jesli komunikat przychodzi od proc to sprawdz(wylosuj) czy dane sa tutaj
   if(komunikat->arrivedOn("cacheWeWy1$i")){
       if(rand()%10<8){
           //jesli dane sa to wyslis w strone proc
           send(komunikat,"cacheWeWy1$o");
       }
       else{
           //jesli danych nie ma to wyslij poszukac na dysku
           send(komunikat,"cacheWeWy2$o");
       }
   }
   else{
       //jesli komunikat przychodzi od dysku to wyslij go w strone proc
       send(komunikat,"cacheWeWy1$o");
   }
}
