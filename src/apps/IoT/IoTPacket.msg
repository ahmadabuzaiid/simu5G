//
//                  O-RAN Simulator
//
// Authors: Ahmad Samir (The German University of Cairo)
//
//
// Packet used by the ~IoTSender and ~IoTReceiver modules.
//

import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

class IoTPacket extends inet::FieldsChunk {
    unsigned int IDtalk;
    unsigned int nframes;
    unsigned int IDframe;
    simtime_t arrivalTime;
    simtime_t playoutTime;
    simtime_t payloadTimestamp;
    unsigned int payloadSize;
}