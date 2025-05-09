#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "ns3/packet.h"
#include "filter.h"

#include <queue>
#include <vector>
#include <math.h>

using namespace ns3;
using namespace std;

class TrafficClass{
    public:

        vector<Filter*> filter_vector;

        TrafficClass();

        //DRR
        TrafficClass(uint32_t max_packets, uint32_t quantum_size, uint32_t deficit_counter, bool isDefault,
            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number);
        
        //SPQ
        TrafficClass(uint32_t maxPackets, double_t weight, uint32_t priority_level, bool isDefault,
            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number);
        
        ~TrafficClass();

        bool Enqueue(Ptr<Packet> packet);

        bool EnqueueDefault(Ptr<Packet> packet);

        Ptr<Packet> Dequeue();

        bool match(Ptr<Packet> packet);

        int getQueueSize();

        Ptr<Packet> peek() const;

        bool isEmpty();


        //Setters

        void addFilter(Filter* set_filter);

        void setPackets(uint32_t set_packets);

        void setMaxPackets(uint32_t max_packets);

        void setWeight(double_t set_weight);

        void setPriorityLvl(uint32_t set_priority_level);

        void setDefault(bool is_default);

        void setQuantumSize(uint32_t set_quantum_size);
        
        void setDeficitCounter(uint32_t set_deficit_counter);
        
        //Getters
        uint32_t getPriorityLvl();

        bool isDefaultCheck();

        uint32_t getPackets();

        uint32_t getMaxPackets();

        uint32_t getWeight();

        uint32_t getQuantumSize();

        uint32_t getDeficitCounter();

        queue<Ptr<Packet>> getMQueue();
        

    private:
        uint32_t packets;
        uint32_t maxPackets;
        double_t weight;
        uint32_t priority_level;
        uint32_t quantum_size;
        uint32_t deficit_counter;
        bool isDefault;
        queue<Ptr<Packet>> m_queue;
        
};

#endif

