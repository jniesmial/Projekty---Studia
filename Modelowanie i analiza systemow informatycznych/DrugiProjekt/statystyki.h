#ifndef __DRUGIPROJEKT_STATYSTYKI_H_
#define __DRUGIPROJEKT_STATYSTYKI_H_

#include <omnetpp.h>

using namespace omnetpp;


class Statystyki : public cSimpleModule
{
  protected:
    virtual void activity();
  public:
    Statystyki(): cSimpleModule(16000){}
};

#endif
