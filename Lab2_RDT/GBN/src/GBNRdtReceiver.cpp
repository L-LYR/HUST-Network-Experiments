#include "GBNRdtReceiver.h"

#include "Global.h"

const int GBN_N = 8;

GBNRdtReceiver::GBNRdtReceiver() : expectSeqNumRecv(0) {
    lastAckPkt.acknum = -1;
    lastAckPkt.seqnum = -1;  // 忽略序号字段
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; ++i) {
        lastAckPkt.payload[i] = '.';
    }
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

GBNRdtReceiver::~GBNRdtReceiver() {}

void GBNRdtReceiver::receive(const Packet& packet) {
    // 获取检验和
    int checksum = pUtils->calculateCheckSum(packet);
    // 校验和正确，收到的报文序号与期待的报文序号一致
    if (checksum == packet.checksum && expectSeqNumRecv == packet.seqnum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);

        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);

        lastAckPkt.acknum = packet.seqnum;
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);

        expectSeqNumRecv = (expectSeqNumRecv + 1) % GBN_N;
    } else {
        if (checksum != packet.checksum) {
            pUtils->printPacket("接收方没有正确收到发送方的报文，数据校验错误", packet);
        } else {
            pUtils->printPacket("接收方没有正确收到发送方的报文，报文序号不对", packet);
        }
        pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);
    }
}