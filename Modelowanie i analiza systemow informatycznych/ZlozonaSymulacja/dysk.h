
#ifndef __ZLOZONASYMULACJA_DYSK_H_
#define __ZLOZONASYMULACJA_DYSK_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Dysk : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void activity();
  public:
    Dysk():cSimpleModule(16000){}
};

#endif
