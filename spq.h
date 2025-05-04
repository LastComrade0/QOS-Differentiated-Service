#ifndef SPQ_H
#define SPQ_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"

#include "diffserv.h"
#include "traffic_class.h"
#include "filter.h"
#include "dest_ip.h"
#include "dest_port_number.h"
#include "protocol_number.h"

using namespace ns3;
using namespace std;

template<typename Packet>
class SPQ : public DiffServ<Packet>{
    public:
        SPQ();

        ~SPQ();

        Ptr<Packet> Schedule() override;
        uint32_t Classify(Ptr<Packet> p) override;

        void CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector);
    
};

void insertFilter(TrafficClass* tc, Filter *filter);


#endif