#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include <list>

#include "RdtSender.h"
const int GBN_N = 8;        // 序号空间大小
const int WINDOW_SIZE_MAX = 7;  // 窗口大小

class GBNRdtSender : public RdtSender {
   private:
    int base;                  // 基序号
    int nextSeqNum;            // 下一个发送的包序号
    std::list<Packet> window;  // 滑动窗口

    bool inWindow(int seqNum);  // 用来判断seqNum是否在当前窗口内

   public:
    GBNRdtSender();
    virtual ~GBNRdtSender();

   public:
    bool getWaitingState();
    bool send(const Message& msg);
    void receive(const Packet& ackPkt);
    void timeoutHandler(int seqNum);
};

#endif