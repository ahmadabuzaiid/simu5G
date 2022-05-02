//
//                  O-RAN Simulator
//
// Authors: Ahmad Samir (The German University of Cairo)
//
//
// Packet used by the ~IoTSender and ~IoTReceiver modules.
//

#ifndef _LTE_IoTReceiver_H_
#define _LTE_IoTReceiver_H_

#include <list>
#include <string.h>

#include <omnetpp.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>

#include "apps/IoT/IoTPacket_m.h"

class IoTReceiver : public omnetpp::cSimpleModule
{
    inet::UdpSocket socket;

    ~IoTReceiver();

    int emodel_Ie_;
    int emodel_Bpl_;
    int emodel_A_;
    double emodel_Ro_;

    typedef std::list<IoTPacket*> PacketsList;
    PacketsList mPacketsList_;
    PacketsList mPlayoutQueue_;
    unsigned int mCurrentTalkspurt_;
    unsigned int mBufferSpace_;
    omnetpp::simtime_t mSamplingDelta_;
    omnetpp::simtime_t mPlayoutDelay_;

    bool mInit_;

    unsigned int totalRcvdBytes_;
    omnetpp::simtime_t warmUpPer_;

    omnetpp::simsignal_t voIPFrameLossSignal_;
    omnetpp::simsignal_t voIPFrameDelaySignal_;
    omnetpp::simsignal_t voIPPlayoutDelaySignal_;
    omnetpp::simsignal_t voIPMosSignal_;
    omnetpp::simsignal_t voIPTaildropLossSignal_;
    omnetpp::simsignal_t voIPPlayoutLossSignal_;
    omnetpp::simsignal_t voIPJitterSignal_;
    omnetpp::simsignal_t voIPReceivedThroughput_;

    virtual void finish() override;

  protected:

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
    double eModel(omnetpp::simtime_t delay, double loss);
    void playout(bool finish);
};

#endif

