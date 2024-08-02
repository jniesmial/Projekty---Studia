#include "klient.h"
#include "komunikat_m.h"

Define_Module(Klient);

void Klient::activity() {

    Komunikat* komunikat;
    int adresNadawczy = par("adresNadawczy").intValue();

    for (;;) {
        double czasMiedzyZad = par("czasMiedzyZad").doubleValue();
        double wielkoscZadania = par("wielkoscZadania").doubleValue();

        komunikat = new Komunikat();
        komunikat->setAdresNadawczy(adresNadawczy);
        komunikat->setWielkoscZadania(wielkoscZadania);

        send(komunikat,"klientWyjscie");

        wait(czasMiedzyZad);
    }
}
