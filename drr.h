#ifndef DRR_H
#define DRR_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

#include "diffserv.h"
#include "traffic_class.h"
#include "filter.h"
#include "dest_ip.h"
#include "dest_port_number.h"
#include "protocol_number.h"

using namespace ns3;
using namespace std;

struct DRRConfig{
    /* data */
    uint32_t class_id;
    uint32_t max_packets;
    uint32_t quantum_size;
    bool isDefault;
    vector<map<string, string>> raw_filter_vector;

};


template<typename Packet>
class DRR : public DiffServ<Packet>{
    public:
        DRR();
        //DRR(vector<MyConfig> configs);
        ~DRR();

        Ptr<Packet> Schedule() override;
        uint32_t Classify(Ptr<Packet> p) override;

        void CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector);

        void CheckQueue();

        vector<TrafficClass*> getTrafficVector();

        bool testEnqueue(Ptr<Packet> packet);

        Ptr<Packet> testDequeue();
        
        bool DoEnqueue(Ptr<Packet> packet) override;

        Ptr<Packet> DoDequeue() override;

    private:
        uint32_t current_robin;
    
};

void insertFilter(TrafficClass* tc, Filter *filter);

FilterElement* set_element(string key, string value);

#endif