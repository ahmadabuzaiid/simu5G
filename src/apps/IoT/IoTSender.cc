//
//                  O-RAN Simulator
//
// Authors: Ahmad Samir (The German University of Cairo)
//
//
// Packet used by the ~IoTSender and ~IoTReceiver modules.
//

#include <cmath>
#include <string>
#include <inet/common/TimeTag_m.h>
#include "apps/IoT/IoTSender.h"

#define round(x) floor((x) + 0.5)

Define_Module(IoTSender);
using namespace inet;



IoTSender::IoTSender()
{
    selfSource_ = nullptr;
    selfSender_ = nullptr;
}

IoTSender::~IoTSender()
{
    cancelAndDelete(selfSource_);
    cancelAndDelete(selfSender_);
}

void IoTSender::initialize(int stage)
{
    EV << "IoT Sender initialize: stage " << stage << endl;

    cSimpleModule::initialize(stage);

    // avoid multiple initializations
    if (stage!=inet::INITSTAGE_APPLICATION_LAYER)
        return;

    durSil_ = par("durSil");
    scaleSen_ = par("scale_sen");
    shapeSen_ = par("shape_sen");
    state_ = 0;
    selfSource_ = new cMessage("selfSource");
    iDtalk_ = 0;
    nframes_ = 0;
    nframesTmp_ = 0;
    iDframe_ = 0;
    timestamp_ = 0;
    sampling_time = par("sampling_time");
    size_ = par("PacketSize");
    nPkts_ = par("nPkts");
    selfSender_ = new cMessage("selfSender");
    localPort_ = par("localPort");
    destPort_ = par("destPort");

    batteryVector.setName("Battery Level");
    batteryTimeVector.setName("Battery Time Consumption");
    batteryTime_ = 172800;
    battery_ = 240.0;

    //timeHistogram.setName("Time per State Histogram");
    //timeHistogram.setRangeAutoUpper(0, 10, 1.5);

    factSil_ = par("factSil");
    factSen_ = par("factSen");
    factTalk_ = par("factTalk");
    factHeat_ = par("factHeat");

    totalSil_ = 0;
    totalSen_ = 0;
    totalTalk_ = 0;

    totalSentBytes_ = 0;
    warmUpPer_ = getSimulation()->getWarmupPeriod();
    IoTGeneratedThroughtput_ = registerSignal("IoTGeneratedThroughtput");

    initTraffic_ = new cMessage("initTraffic");
    initTraffic();
}

void IoTSender::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        if (!strcmp(msg->getName(), "selfSender")){
            sendIoTPacket();
        }
        else if (!strcmp(msg->getName(), "selfSource"))
            chooseState();
        else
            initTraffic();
    }
}

void IoTSender::initTraffic()
{
    std::string destAddress = par("destAddress").stringValue();
    cModule* destModule = findModuleByPath(par("destAddress").stringValue());
    if (destModule == nullptr)
    {
        // this might happen when users are created dynamically
        EV << simTime() << "IoTSender::initTraffic - destination " << destAddress << " not found" << endl;

        simtime_t offset = 0.01; // TODO check value
        scheduleAt(simTime()+offset, initTraffic_);
        EV << simTime() << "IoTSender::initTraffic - the node will retry to initialize traffic in " << offset << " seconds " << endl;
    }
    else
    {
        delete initTraffic_;

        socket.setOutputGate(gate("socketOut"));
        socket.bind(localPort_);

        int tos = par("tos");
        if (tos != -1)
            socket.setTos(tos);

        EV << simTime() << "IoTSender::initialize - binding to port: local:" << localPort_ << " , dest: " << destAddress_.str() << ":" << destPort_ << endl;

        // calculating traffic starting time
        simtime_t startTime = par("startTime");

        scheduleAt(simTime()+startTime, selfSource_);
        EV << "\t starting traffic in " << startTime << " seconds " << endl;
    }
}

void IoTSender::talkspurt(simtime_t dur)
{
    iDtalk_++;
    nframes_ = (ceil(dur / sampling_time));

    // a talkspurt must be at least 1 frame long
    if (nframes_ == 0)
        nframes_ = 1;

    EV << "IoTSender::talkspurt - TALKSPURT[" << iDtalk_-1 << "] - Will be created[" << nframes_ << "] frames\n";

    iDframe_ = 0;
    nframesTmp_ = nframes_;
    scheduleAt(simTime(), selfSender_);
}

void IoTSender::chooseState()
{
    switch(state_){
    case(0) : silMode(); break;
    case(1) : senMode(); break;
    case(2) : talkMode(); break;
    }
}

void IoTSender::silMode(){
    //silence state
    //interarrival time

    //cancelEvent(selfSource_); //I had to canecl the prev timer.

    EV << "IoTSender::chooseState - Silence Period: " << "Duration[" << durSil_ << "] seconds\n";
    batteryCalculator(0, durSil_);
    totalSil_ += durSil_;
    state_ = 1;
    scheduleAt(simTime() + durSil_, selfSource_);
}

void IoTSender::senMode(){
    //sensing state

    //cancelEvent(selfSource_); //I had to canecl the prev timer.

    simtime_t durSen_ = weibull(scaleSen_, shapeSen_);
    double durSen2_ = round(SIMTIME_DBL(durSen_)*1000) / 1000;
    EV << "IoTSender::chooseState - Sensing Period: " << "Duration[" << durSen_ << "/" << durSen2_ << " seconds]\n";
    batteryCalculator(1, durSen_);
    totalSen_ += durSen_;
    state_ = 2;
    scheduleAt(simTime() + durSen_, selfSource_);
}

void IoTSender::talkMode(){
    //transimission state

    //cancelEvent(selfSource_); //I had to canecl the prev timer.

    simtime_t durTalk_ = 0.02*nPkts_;
    EV << "IoTSender::chooseState - Talks Purt: " << "Duration[" << durTalk_ << " seconds]\n";
    talkspurt(durTalk_);
    batteryCalculator(2, durTalk_);
    totalTalk_ += durTalk_;
    state_ = 0;
    scheduleAt(simTime() + durTalk_, selfSource_);
}

void IoTSender::sendIoTPacket()
{

    if (destAddress_.isUnspecified())
        destAddress_ = L3AddressResolver().resolve(par("destAddress"));

    Packet* packet = new inet::Packet("IoT");
    auto iot = makeShared<IoTPacket>();
    iot->setIDtalk(iDtalk_ - 1);
    iot->setNframes(nframes_);
    iot->setIDframe(iDframe_);
    iot->setPayloadTimestamp(simTime());
    iot->setChunkLength(B(size_));
    iot->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(iot);
    EV << "IoTSender::sendIoTPacket - Talkspurt[" << iDtalk_-1 << "] - Sending frame[" << iDframe_ << "]\n";

    socket.sendTo(packet, destAddress_, destPort_);
    --nframesTmp_;
    ++iDframe_;

    // emit throughput sample
    totalSentBytes_ += size_;
    double interval = SIMTIME_DBL(simTime() - warmUpPer_);
    if (interval > 0.0)
    {
        double tputSample = (double)totalSentBytes_ / interval;
        emit(IoTGeneratedThroughtput_, tputSample );
    }

    if (nframesTmp_ > 0)
        scheduleAt(simTime() + sampling_time, selfSender_);
}

void IoTSender::batteryCalculator(int state, simtime_t dur)
{
    simtime_t prev = batteryTime_;
    simtime_t tmp = 0;

    switch(state){
    case(0): tmp= dur * factSil_ * factHeat_; break;
    case(1): tmp = dur * factSen_ * factHeat_; break;
    case(2): tmp = dur * factTalk_ * factHeat_; break;
    }

    batteryTime_ = batteryTime_ - tmp;

    if(batteryTime_ <= 0){
        getSimulation()->getActiveEnvir()->alert("Battery is fully discharged");
        stopSim();
    }

    EV << "Delta is: " <<prev -  batteryTime_<< " Sec\n";

    battery_ = (SIMTIME_DBL(batteryTime_)*240)/172800;
    batteryVector.record(battery_);
    batteryTimeVector.record(tmp);

    EV << "Battery ETA left is: " <<batteryTime_ << " Sec\n";
    EV << "Battery value is now: " <<battery_ << " amp\n";

}

void IoTSender::stopSim(){
    while(true){}
}

void IoTSender::finish()
{
    recordScalar("Total Sil Time", totalSil_);
    recordScalar("Total Sen Time", totalSen_);
    recordScalar("Total Talk Time", totalTalk_);
}
