#ifndef SPQ_H
#define SPQ_H

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

#include <vector>
#include <map>

using namespace ns3;
using namespace std;

struct SPQConfig{
    /* data */
    uint32_t class_id;
    uint32_t max_packets;
    uint32_t priority_level;
    bool isDefault;
    vector<map<string, string>> raw_filter_vector;

};


template<typename Packet>
class SPQ : public DiffServ<Packet>{
    public:
        SPQ();
        //SPQ(vector<MyConfig> configs);
        ~SPQ();

        Ptr<Packet> Schedule() override;
        uint32_t Classify(Ptr<Packet> p) override;

        void CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector);

        void CheckQueue();

        vector<TrafficClass*> getTrafficVector();

        bool testEnqueue(Ptr<Packet> packet);

        Ptr<Packet> testDequeue();
    
};

void insertFilter(TrafficClass* tc, Filter *filter);

FilterElement* set_element(string key, string value);


#endif