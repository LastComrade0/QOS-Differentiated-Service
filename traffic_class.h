#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "ns3/packet.h"
#include "filter.h"

#include <queue>
#include <vector>

using namespace ns3;
using namespace std;

class TrafficClass{
    public:

        vector<Filter*> filter_vector;

        TrafficClass();

        //DRR
        TrafficClass(uint32_t max_packets, uint32_t quantum_size, uint32_t *deficit_counter, bool isDefault,
            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number);
        
        //SPQ
        TrafficClass(uint32_t maxPackets, double_t weight, uint32_t priority_level, bool isDefault,
            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number);
        
        ~TrafficClass();

        bool Enqueue(Ptr<Packet> packet);

        Ptr<Packet> Dequeue();

        bool match(Ptr<Packet> packet);


    private:
        uint32_t packets;
        uint32_t maxPackets;
        double_t weight;
        uint32_t priority_level;
        uint32_t quantum_size;
        uint32_t *deficit_counter;
        bool isDefault;
        queue<Ptr<Packet>> m_queue;
        
};

#endif

