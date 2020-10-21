#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H

#include <vector>

#include "RdtReceiver.h"

class TCPRdtReceiver : public RdtReceiver {
   private:
    enum {
        TCP_N = 8,
        WINDOW_SIZE_MAX = 4,
    };
    // [base, base + WINDOW_SIZE_MAX - 1]
    int base;
    int nextSeqNum;
    std::vector<Packet> rcvWindow;
    bool isCached[TCP_N];

    void updateWindowBound();
    bool inCurWindow(int seqNum);
    void printWindow();
    void deliver();

   public:
    TCPRdtReceiver();
    virtual ~TCPRdtReceiver();

   public:
    void receive(const Packet& pkt);
};

#endif