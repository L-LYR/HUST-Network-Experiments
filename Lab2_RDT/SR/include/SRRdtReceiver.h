#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include <vector>

#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver {
   private:
    enum {
        SR_N = 8,
        WINDOW_SIZE_MAX = 4,
    };
    // [base, base + WINDOW_SIZE_MAX - 1]
    int base;
    int upperBound;
    std::vector<Packet> rcvWindow;
    bool isCached[SR_N];

    void updateWindowBound();
    bool inCurWindow(int seqNum);
    void deliver();

   public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();

   public:
    void receive(const Packet& pkt);
};

#endif