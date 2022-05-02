//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _VoIPSenderUe_H_
#define _VoIPSenderUe_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include "apps/voip/VoipPacket_m.h"

using namespace omnetpp;

class VoIPSenderUe : public omnetpp::cSimpleModule
{
    inet::UdpSocket socket;

    //source
    omnetpp::simtime_t durTalk_;
    omnetpp::simtime_t durSil_;
    double scaleTalk_;
    double shapeTalk_;
    double scaleSil_;
    double shapeSil_;
    bool isTalk_;
    omnetpp::cMessage* selfSource_;
    //sender
    int iDtalk_;
    int nframes_;
    int iDframe_;
    int nframesTmp_;
    int size_;
    omnetpp::simtime_t sampling_time;

    omnetpp::simtime_t endTime_;
    omnetpp::simtime_t batteryTime_;
    cOutVector batteryVector;
    double battery_;
    bool isNewPkt_;

    bool silences_;

    unsigned int totalSentBytes_;
    omnetpp::simtime_t warmUpPer_;

    omnetpp::simsignal_t voIPGeneratedThroughtput_;
    // ----------------------------

    omnetpp::cMessage *selfSender_;

    omnetpp::cMessage *initTraffic_;

    omnetpp::simtime_t timestamp_;
    int localPort_;
    int destPort_;
    inet::L3Address destAddress_;

    void initTraffic();
    void talkspurt(omnetpp::simtime_t dur);
    void selectPeriodTime();
    void sendVoIPPacket();
    void batteryCalculator();

  public:
    ~VoIPSenderUe();
    VoIPSenderUe();

  protected:

    virtual int numInitStages() const  override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
    void finish() override;

};

#endif

