#include "TCPRdtSender.h"

#include "Global.h"

TCPRdtSender::TCPRdtSender()
    : base(0), nextSeqNum(0), dupCnt(0), lastAckNum(-1) {}

TCPRdtSender::~TCPRdtSender() {}

bool TCPRdtSender::getWaitingState() {
    // 获取状态，窗口满了则表示处于等待状态
    return (window.size() == WINDOW_SIZE_MAX);
}

void TCPRdtSender::printWindow() {
    std::cout << std::endl
              << "发送方：" << std::endl
              << "窗口大小：" << window.size() << std::endl
              << "base = " << base << std::endl
              << "nextSeqNum = " << nextSeqNum << std::endl
              << "窗口内容：";
    if (window.empty())
        std::cout << "空" << std::endl;
    else
        std::cout << "[";
    for (auto& i : window) {
        std::cout << i.seqnum;
        if (i == window.back()) {
            std::cout << "]" << std::endl;
        } else {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

bool TCPRdtSender::send(const Message& msg) {
    if (getWaitingState()) return false;

    Packet pkt;                                       // 封装当前数据包
    pkt.acknum = -1;                                  // 忽略ack字段
    pkt.seqnum = nextSeqNum;                          // 写入当前序号
    memcpy(pkt.payload, msg.data, sizeof(msg.data));  // 写入message
    pkt.checksum = pUtils->calculateCheckSum(pkt);    // 计算校验和
    // 发送至网络层
    pUtils->printPacket("发送方发送报文", pkt);
    if (nextSeqNum == base) {  // 如果窗口长度为0, 即基序号即为下一个要发送的包序号，则开启计时器
        pns->startTimer(SENDER, Configuration::TIME_OUT, base);
    }
    pns->sendToNetworkLayer(RECEIVER, pkt);

    window.push_back(pkt);                  // 加入窗口缓冲区
    nextSeqNum = (nextSeqNum + 1) % TCP_N;  // 取模

    printWindow();

    return true;
}

// 判断seqNum是否在窗口中
bool TCPRdtSender::inWindow(int seqNum) {
    if (base <= nextSeqNum) {  // [base, nextSeqNum)
        return seqNum > base && seqNum <= nextSeqNum;
    } else {  // [0, nextSeqNum) || [base, WINDOW_SIZE_MAX)
        return seqNum > base || seqNum <= nextSeqNum;
    }
}

void TCPRdtSender::receive(const Packet& ackPkt) {
    int checksum = pUtils->calculateCheckSum(ackPkt);
    if (checksum == ackPkt.checksum) {  // 检验和没错
        pns->stopTimer(SENDER, base);   // 关计时器，因为至少收到了ack

        if (inWindow(ackPkt.acknum)) {  // 是窗口中的ack
            pUtils->printPacket("发送方正确收到确认", ackPkt);
            base = ackPkt.acknum;  // base和窗口左侧区间向前进
            while (!window.empty() && window.front().seqnum != base) {
                window.pop_front();
            }
        } else {
            pUtils->printPacket("发送方收到确认，但不是发送方期待的确认", ackPkt);
        }

        if (ackPkt.acknum == lastAckNum) {
            dupCnt++;
        } else {
            dupCnt = 0;
        }
        lastAckNum = ackPkt.acknum;

        if (dupCnt >= 4) {
            std::cout << std::endl;
            pUtils->printPacket("发送方收到冗余Ack，进入快速重传", window.front());
            pns->sendToNetworkLayer(RECEIVER, window.front());
            pns->startTimer(SENDER, Configuration::TIME_OUT, base);
            std::cout << std::endl;
            return;
        }

        if (base != nextSeqNum) {  // 窗口中的ack没有收全，因此重新设置计时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, base);
        }
    } else {
        pUtils->printPacket("发送方没有正确收到发送方的确认，数据校验错误", ackPkt);
    }
    printWindow();
}
void TCPRdtSender::timeoutHandler(int seqNum) {
    // 重传之前重设计时器
    pns->stopTimer(SENDER, base);
    pns->startTimer(SENDER, Configuration::TIME_OUT, base);
    pUtils->printPacket("发送方定时器时间到，重发未发送成功的报文", window.front());
    pns->sendToNetworkLayer(RECEIVER, window.front());
}