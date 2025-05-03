#ifndef DIFF_SERV_H
#define DIFF_SERV_H

#include "ns3/queue.h"
#include "traffic_class.h"

#include <vector>
#include <memory>

using namespace ns3;

class DiffServ : public Queue<Packet> {

    public:

        DiffServ();

        DiffServ(uint32_t max_packets, uint32_t priority_level, bool isDefault,
                Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
                Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number);

        ~DiffServ();

        void AddTrafficClass(TrafficClass* tc);

        bool Enqueue(Ptr<Packet> packet);

        Ptr<Packet> Dequeue();

        Ptr<Packet> Remove();

        Ptr<const Packet> Peek() const;

        virtual Ptr<Packet> Schedule() = 0;

        virtual uint32_t Classify(Ptr<Packet> p) = 0;


    protected:
        
        vector<TrafficClass*> q_class;

};


#endif