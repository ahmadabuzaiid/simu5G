//
//                  O-RAN Simulator
//
// Authors: Ahmad Samir (The German University of Cairo)
//
//
// Packet used by the ~IoTSender and ~IoTReceiver modules.
//

package simu5g.apps.IoT;

import inet.applications.contract.IApp;


simple IoTReceiver like IApp
{
    parameters:
        int localPort = default(3000);
        int emodel_Ie_ = default(5);
        int emodel_Bpl_ = default(10);
        int emodel_A_ = default(5);
        double emodel_Ro_ = default(93.2);
        double playout_delay @unit("s") = default(0s);
        int dim_buffer = default(20);
        double sampling_time @unit("s") = default(0.02s);


//		  consider added the statisitical part here!
//		  @signal[voIPFrameLoss];
//        @statistic[voIPFrameLoss](title="VoIP Frame Loss"; unit="ratio"; source="voIPFrameLoss"; record=mean);
//        @signal[voIPFrameDelay];
//        @statistic[voIPFrameDelay](title="VoIP Frame Delay"; unit="s"; source="voIPFrameDelay"; record=mean,vector);
//        @signal[voIPPlayoutDelay];
//        @statistic[voIPPlayoutDelay](title="VoIP Playout Delay"; unit="s"; source="voIPPlayoutDelay"; record=mean,);
//        @signal[voIPPlayoutLoss];
//        @statistic[voIPPlayoutLoss](title="VoIP Playout Loss"; unit="ratio"; source="voIPPlayoutLoss"; record=mean);
//        @signal[voIPJitter];
//        @statistic[voIPJitter](title="VoIP Playout Loss"; unit="s"; source="voIPJitter"; record=mean);
//        @signal[voIPMos];
//        @statistic[voIPMos](title="VoIP Mos Signal"; unit="MOS"; source="voIPMos"; record=mean);
//        @signal[voIPTaildropLoss];
//        @statistic[voIPTaildropLoss](title="VoIP Tail Drop Loss"; unit="ratio"; source="voIPTaildropLoss"; record=mean);
//        @signal[voIPReceivedThroughput];
//        @statistic[voIPReceivedThroughput](title="Throughput received at the application level"; unit="Bps"; source="voIPReceivedThroughput"; record=mean,vector);
        @display("i=block/source");
    gates:
        output socketOut;
        input socketIn;
}
