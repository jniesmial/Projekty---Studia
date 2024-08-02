

#ifndef __DRUGIPROJEKT_KLIENT_H_
#define __DRUGIPROJEKT_KLIENT_H_

#include <omnetpp.h>

using namespace omnetpp;


class Klient : public cSimpleModule
{
  protected:
    virtual void activity();
  public:
    Klient(): cSimpleModule(16000){}
};

#endif
