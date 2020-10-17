#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H

#include <vector>

#include "RdtSender.h"

class SRRdtSender : public RdtSender {
   private:
    enum {
        SR_N = 8,
        WINDOW_SIZE_MAX = 4,
    };
    // [base, base + WINDOW_SIZE_MAX - 1]
    int base;
    int nextSeqNum;
    std::vector<Packet> sendWindow;
    bool hasAck[SR_N];
    int cnt;

    bool inCurWindow(int seqNum);

   public:
    SRRdtSender();
    ~SRRdtSender();

   public:
    bool send(const Message& msg);
    void receive(const Packet& ackPkt);
    void timeoutHandler(int seqNum);
    bool getWaitingState();
};

#endif