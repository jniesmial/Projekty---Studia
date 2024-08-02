
#ifndef __ZLOZONASYMULACJA_KARTASIECIOWA_H_
#define __ZLOZONASYMULACJA_KARTASIECIOWA_H_

#include <omnetpp.h>

using namespace omnetpp;

class KartaSieciowa : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void activity();

  public:
    KartaSieciowa():cSimpleModule(16000){}
};

#endif
