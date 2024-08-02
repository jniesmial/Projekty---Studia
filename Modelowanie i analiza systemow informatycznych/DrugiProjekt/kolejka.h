
#ifndef __DRUGIPROJEKT_KOLEJKA_H_
#define __DRUGIPROJEKT_KOLEJKA_H_

#include <omnetpp.h>

using namespace omnetpp;

class Kolejka : public cSimpleModule
{
  protected:
    virtual void activity();
  public:
    Kolejka(): cSimpleModule(16000){}
};

#endif
