#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"

class GBNRdtReceiver : public RdtReceiver {
   private:
    enum {
        GBN_N = 8,
    };

    int expectSeqNumRecv;
    Packet lastAckPkt;

   public:
    GBNRdtReceiver();
    ~GBNRdtReceiver();

   public:
    void receive(const Packet& packet);
};

#endif