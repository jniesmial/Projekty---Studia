
#ifndef __ZLOZONASYMULACJA_PROCESOR_H_
#define __ZLOZONASYMULACJA_PROCESOR_H_

#include <omnetpp.h>

using namespace omnetpp;

class Procesor : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void activity();
  public:
    Procesor():cSimpleModule(16000){}

};

#endif
