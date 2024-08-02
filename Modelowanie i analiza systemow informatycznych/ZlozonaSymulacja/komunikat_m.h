//
// Generated file, do not edit! Created by opp_msgtool 6.0 from komunikat.msg.
//

#ifndef __KOMUNIKAT_M_H
#define __KOMUNIKAT_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class Komunikat;
/**
 * Class generated from <tt>komunikat.msg:2</tt> by opp_msgtool.
 * <pre>
 * packet Komunikat
 * {
 *     int wielkoscZadania;
 *     int adresNadawczy;
 * }
 * </pre>
 */
class Komunikat : public ::omnetpp::cPacket
{
  protected:
    int wielkoscZadania = 0;
    int adresNadawczy = 0;

  private:
    void copy(const Komunikat& other);

  protected:
    bool operator==(const Komunikat&) = delete;

  public:
    Komunikat(const char *name=nullptr, short kind=0);
    Komunikat(const Komunikat& other);
    virtual ~Komunikat();
    Komunikat& operator=(const Komunikat& other);
    virtual Komunikat *dup() const override {return new Komunikat(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getWielkoscZadania() const;
    virtual void setWielkoscZadania(int wielkoscZadania);

    virtual int getAdresNadawczy() const;
    virtual void setAdresNadawczy(int adresNadawczy);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Komunikat& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Komunikat& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

template<> inline Komunikat *fromAnyPtr(any_ptr ptr) { return check_and_cast<Komunikat*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __KOMUNIKAT_M_H
