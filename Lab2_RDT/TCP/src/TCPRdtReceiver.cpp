#include "TCPRdtReceiver.h"

#include "Global.h"

TCPRdtReceiver::TCPRdtReceiver() : base(0), rcvWindow(TCP_N) {
    updateWindowBound();
    memset(isCached, false, sizeof(isCached));
}

TCPRdtReceiver::~TCPRdtReceiver() {}

void TCPRdtReceiver::updateWindowBound() {
    nextSeqNum = (base + WINDOW_SIZE_MAX) % TCP_N;
}

void TCPRdtReceiver::printWindow() {
    std::cout << std::endl
              << "接收方：" << std::endl
              << "base = " << base << std::endl
              << "nextSeqNum = " << nextSeqNum << std::endl
              << "接收窗口内容：" << std::endl;
    if (base < nextSeqNum) {
        for (int i = base; i < nextSeqNum; ++i) {
            if (isCached[i])
                pUtils->printPacket("报文内容", rcvWindow[i]);
            else
                std::cout << "未收到该报文" << std::endl;
        }
    } else {
        for (int i = base; i < TCP_N; ++i) {
            if (isCached[i])
                pUtils->printPacket("报文内容", rcvWindow[i]);
            else
                std::cout << "未收到该报文" << std::endl;
        }
        for (int i = 0; i < nextSeqNum; ++i) {
            if (isCached[i])
                pUtils->printPacket("报文内容", rcvWindow[i]);
            else
                std::cout << "未收到该报文" << std::endl;
        }
    }
    std::cout << std::endl;
}

bool TCPRdtReceiver::inCurWindow(int seqNum) {
    if (base <= nextSeqNum) {
        return seqNum >= base && seqNum < nextSeqNum;
    } else {
        return seqNum >= base || seqNum < nextSeqNum;
    }
}

void TCPRdtReceiver::deliver() {
    while (isCached[base]) {
        // base不断向upperBound靠近，同时向上交付数据
        Message msg;
        memcpy(msg.data, rcvWindow[base].payload, sizeof(rcvWindow[base].payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        isCached[base] = false;  // 交付后，位置空出

        base++;
        if (base >= TCP_N) base %= TCP_N;
    }
    updateWindowBound();  // 更新upperBound
}

void TCPRdtReceiver::receive(const Packet& pkt) {
    int checkSum = pUtils->calculateCheckSum(pkt);
    if (checkSum == pkt.checksum) {  // 检查和正确
        pUtils->printPacket("接收方正确收到发送方的报文", pkt);

        if (inCurWindow(pkt.seqnum)) {            // 在当前窗口内
            if (isCached[pkt.seqnum] == false) {  // 之前没收到，缓存
                rcvWindow[pkt.seqnum] = pkt;
                isCached[pkt.seqnum] = true;
                printWindow();
            }
            if (pkt.seqnum == base) {  // 是基序号对应的包，连续向上交付
                deliver();
                printWindow();
            }
        }

        Packet ackPkt;
        ackPkt.seqnum = -1;
        for (int i = 0; i < Configuration::PAYLOAD_SIZE; ++i) {
            ackPkt.payload[i] = '.';
        }
        ackPkt.acknum = base;
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("接收方发送确认报文", ackPkt);
        pns->sendToNetworkLayer(SENDER, ackPkt);
    } else {  // 其余情况报告但不处理
        pUtils->printPacket("接收方没有正确收到发送方的报文，数据校验错误", pkt);
    }
}