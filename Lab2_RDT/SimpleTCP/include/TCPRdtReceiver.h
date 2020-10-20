#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H
#include "RdtReceiver.h"

class TCPRdtReceiver : public RdtReceiver {
   private:
    enum {
        TCP_N = 8,
    };

    int expectSeqNumRecv;
    Packet lastAckPkt;

   public:
    TCPRdtReceiver();
    ~TCPRdtReceiver();

   public:
    void receive(const Packet& packet);
};

#endif