#ifndef KOMUNIKAT_H_
#define KOMUNIKAT_H_

#include <omnetpp.h>

using namespace omnetpp;

class Komunikat : public cMessage {
private:
    int wielkoscZadania;

public:
    Komunikat(const char *name=nullptr) : cMessage(name), wielkoscZadania(0) {}
    virtual ~Komunikat() {}

    void setWielkoscZadania(int wielkosc) { wielkoscZadania = wielkosc; }
    int getWielkoscZadania() const { return wielkoscZadania; }
};

#endif
