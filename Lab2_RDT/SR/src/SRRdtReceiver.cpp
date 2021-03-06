#include "SRRdtReceiver.h"

#include "Global.h"

SRRdtReceiver::SRRdtReceiver() : base(0), rcvWindow(SR_N) {
    updateWindowBound();
    memset(isCached, false, sizeof(isCached));
}

SRRdtReceiver::~SRRdtReceiver() {}

void SRRdtReceiver::updateWindowBound() {
    upperBound = (base + WINDOW_SIZE_MAX - 1) % SR_N;
}

void SRRdtReceiver::printWindow() {
    std::cout << std::endl
              << "接收方：" << std::endl
              << "base = " << base << std::endl
              << "upperBound = " << upperBound << std::endl
              << "接收窗口内容：" << std::endl;
    if (base <= upperBound) {
        for (int i = base; i <= upperBound; ++i) {
            if (isCached[i])
                pUtils->printPacket("报文内容", rcvWindow[i]);
            else
                std::cout << "未收到该报文" << std::endl;
        }
    } else {
        for (int i = base; i < SR_N; ++i) {
            if (isCached[i])
                pUtils->printPacket("报文内容", rcvWindow[i]);
            else
                std::cout << "未收到该报文" << std::endl;
        }
        for (int i = 0; i <= upperBound; ++i) {
            if (isCached[i])
                pUtils->printPacket("报文内容", rcvWindow[i]);
            else
                std::cout << "未收到该报文" << std::endl;
        }
    }
    std::cout << std::endl;
}

bool SRRdtReceiver::inCurWindow(int seqNum) {
    if (base <= upperBound) {
        return seqNum >= base && seqNum <= upperBound;
    } else {
        return seqNum >= base || seqNum <= upperBound;
    }
}

void SRRdtReceiver::deliver() {
    while (isCached[base]) {
        // base不断向upperBound靠近，同时向上交付数据
        Message msg;
        memcpy(msg.data, rcvWindow[base].payload, sizeof(rcvWindow[base].payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        isCached[base] = false;  // 交付后，位置空出

        base++;
        if (base >= SR_N) base %= SR_N;
    }
    updateWindowBound();  // 更新upperBound
}

void SRRdtReceiver::receive(const Packet& pkt) {
    int checkSum = pUtils->calculateCheckSum(pkt);
    if (checkSum == pkt.checksum) {  // 检查和正确
        pUtils->printPacket("接收方正确收到发送方的报文", pkt);
        Packet ackPkt;
        ackPkt.seqnum = -1;
        ackPkt.acknum = pkt.seqnum;
        for (int i = 0; i < Configuration::PAYLOAD_SIZE; ++i) {
            ackPkt.payload[i] = '.';
        }
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("接收方发送确认报文", ackPkt);
        pns->sendToNetworkLayer(SENDER, ackPkt);
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
        }     // 在上一窗口
    } else {  // 其余情况报告但不处理
        pUtils->printPacket("接收方没有正确收到发送方的报文，数据校验错误", pkt);
    }
}