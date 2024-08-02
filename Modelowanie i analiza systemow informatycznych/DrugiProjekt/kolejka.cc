#include "kolejka.h"
#include "Komunikat_m.h"

Define_Module(Kolejka);

void Kolejka::activity() {
    double czasObslugi = par("czasObslugi").doubleValue();
    cQueue kolejka;

    for (;;) {
        Komunikat *komunikat = nullptr;
        int wielkoscZadania = 0;

        if (kolejka.isEmpty()) {
            komunikat = check_and_cast<Komunikat *>(receive());
            wielkoscZadania = komunikat->getWielkoscZadania();
        } else {
            komunikat = (Komunikat *) kolejka.pop();
            wielkoscZadania = komunikat->getWielkoscZadania();
        }

        double czas = wielkoscZadania * czasObslugi;
        waitAndEnqueue(czas, &kolejka);

        send(komunikat, "wyjscie");
    }
}
