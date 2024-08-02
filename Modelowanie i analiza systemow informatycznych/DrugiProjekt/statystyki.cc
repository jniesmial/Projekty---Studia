#include "statystyki.h"
#include "Komunikat_m.h"

Define_Module(Statystyki);

void Statystyki::activity() {
    Komunikat *komunikat;

    for (;;) {
        komunikat = check_and_cast<Komunikat *>(receive());
        delete komunikat;
    }
}
