#include "SRRdtSender.h"

#include "Global.h"

SRRdtSender::SRRdtSender() : sendWindow(SR_N) {
    base = nextSeqNum = 0;
    memset(hasAck, false, sizeof(hasAck));
    cnt = 0;
}

SRRdtSender::~SRRdtSender() {}

bool SRRdtSender::getWaitingState() {
    return cnt == WINDOW_SIZE_MAX;
}

void SRRdtSender::printWindow() {
    std::cout << std::endl
              << "发送方：" << std::endl
              << "base = " << base << std::endl
              << "nextSeqNum = " << nextSeqNum << std::endl
              << "发送窗口内容：" << std::endl;
    if (cnt > 0) {
        if (base < nextSeqNum) {
            for (int i = base; i < nextSeqNum; ++i) {
                if (hasAck[i])
                    std::cout << "已收到确认，";
                else
                    std::cout << "未收到确认，";
                pUtils->printPacket("报文内容", sendWindow[i]);
            }
        } else {
            for (int i = base; i < SR_N; ++i) {
                if (hasAck[i])
                    std::cout << "已收到确认，";
                else
                    std::cout << "未收到确认，";
                pUtils->printPacket("报文内容", sendWindow[i]);
            }
            for (int i = 0; i < nextSeqNum; ++i) {
                if (hasAck[i])
                    std::cout << "已收到确认，";
                else
                    std::cout << "未收到确认，";
                pUtils->printPacket("报文内容", sendWindow[i]);
            }
        }
    }
    std::cout << std::endl;
}

bool SRRdtSender::send(const Message& msg) {
    if (getWaitingState()) return false;

    Packet pkt;
    pkt.acknum = -1;
    pkt.seqnum = nextSeqNum;
    memcpy(pkt.payload, msg.data, sizeof(msg.data));
    pkt.checksum = pUtils->calculateCheckSum(pkt);

    pUtils->printPacket("发送方发送报文", pkt);

    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
    pns->sendToNetworkLayer(RECEIVER, pkt);

    sendWindow[nextSeqNum] = pkt;
    hasAck[nextSeqNum] = false;
    nextSeqNum = (nextSeqNum + 1) % SR_N;
    cnt++;

    printWindow();

    return true;
}

bool SRRdtSender::inCurWindow(int seqNum) {
    if (base <= nextSeqNum) {  // [base, nextSeqNum)
        return seqNum >= base && seqNum < nextSeqNum;
    } else {  // [0, nextSeqNum) || [base, WINDOW_SIZE_MAX)
        return seqNum >= base || seqNum < nextSeqNum;
    }
}

void SRRdtSender::receive(const Packet& ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if (checkSum == ackPkt.checksum && inCurWindow(ackPkt.acknum)) {
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        pns->stopTimer(SENDER, ackPkt.acknum);
        hasAck[ackPkt.acknum] = true;
        if (ackPkt.acknum == base) {
            while (hasAck[base]) {
                hasAck[base] = false;
                base = (base + 1) % SR_N;
                cnt--;
            }
        }
        printWindow();
    }
}

void SRRdtSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, seqNum);
    pUtils->printPacket("发送方定时器时间到，重发未发送成功的报文", sendWindow[seqNum]);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    pns->sendToNetworkLayer(RECEIVER, sendWindow[seqNum]);
}