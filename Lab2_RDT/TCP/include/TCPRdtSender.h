#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include <list>

#include "RdtSender.h"

class TCPRdtSender : public RdtSender {
   private:
    enum {
        TCP_N = 8,           // 序号空间大小
        WINDOW_SIZE_MAX = 4  // 窗口长度最大值
    };

    int base;                  // 基序号
    int nextSeqNum;            // 下一个发送的包序号
    std::list<Packet> window;  // 滑动窗口

    int dupCnt;
    int lastAckNum;

    bool inWindow(int seqNum);  // 用来判断seqNum是否在当前窗口内
    void printWindow();

   public:
    TCPRdtSender();
    ~TCPRdtSender();

   public:
    bool getWaitingState();
    bool send(const Message& msg);
    void receive(const Packet& ackPkt);
    void timeoutHandler(int seqNum);
};

#endif