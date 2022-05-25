//
//                  O-RAN Simulator
//
// Authors: Ahmad Samir (The German University of Cairo)
//
//
// Packet used by the ~IoTSender and ~IoTReceiver modules.
//

#ifndef _IoTSender_H_
#define _IoTSender_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include "apps/IoT/IoTPacket_m.h"

using namespace omnetpp;

class IoTSender : public omnetpp::cSimpleModule
{
    inet::UdpSocket socket;

    //source
    omnetpp::simtime_t durSil_;
    omnetpp::cMessage* selfSource_;
    double mean_sen_;
    int state_;
    //sender
    int iDtalk_;
    int nframes_;
    int iDframe_;
    int nframesTmp_;
    int size_;
    int nPkts_;
    omnetpp::simtime_t totalSil_;
    omnetpp::simtime_t totalSen_;
    omnetpp::simtime_t totalTalk_;
    double spb_;

    omnetpp::simtime_t batteryTime_;
    omnetpp::simtime_t batteryTimeFull_;
    cOutVector batteryVector;
    cOutVector batteryTimeVector;
    double battery_;
    double batteryFull_;
    double reChargeAmp_;
    omnetpp::simtime_t reChargeTime_;

    omnetpp::simtime_t totalTime_;

    double factSil_;
    double factSen_;
    double factTalk_;
    double factHeat_;

    //cLongHistogram timeHistogram;

    unsigned int totalSentBytes_;
    omnetpp::simtime_t warmUpPer_;

    omnetpp::simsignal_t IoTGeneratedThroughtput_;
    // ----------------------------

    omnetpp::cMessage *selfSender_;

    omnetpp::cMessage *initTraffic_;

    omnetpp::simtime_t timestamp_;
    int localPort_;
    int destPort_;
    inet::L3Address destAddress_;

    void initTraffic();
    void talkspurt(omnetpp::simtime_t dur);
    void chooseState();
    void silMode();
    void senMode();
    void talkMode();
    void sendIoTPacket();
    void batteryCalculator(int state, omnetpp::simtime_t dur);
    void stopSim();

  public:
    ~IoTSender();
    IoTSender();

  protected:

    virtual int numInitStages() const  override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
    void finish() override;

};

#endif

