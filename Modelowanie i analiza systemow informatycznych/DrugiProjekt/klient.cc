#include "klient.h"
#include "Komunikat_m.h"

Define_Module(Klient);

void Klient::activity() {
    Komunikat *komunikat;
    double czasObslugi;

    for (;;) {
        komunikat = new Komunikat();
        send(komunikat, "wyjscie");

        czasObslugi = par("czasObslugi").doubleValue();
        wait(czasObslugi);
    }
}
