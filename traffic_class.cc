#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
//#include "filter.cc"
#include "diffserv.cc"
#include <iostream>
#include <vector>
#include <memory>

using namespace std;
using namespace ns3;

class Filter;

class TrafficClass : public DiffServ{
    public:
        vector<Filter*> filters;
        
        bool Enqueue(Ptr<Packet> packet){
            return false;
        }

        Ptr<Packet> Dequeue(){
            return nullptr;
        }

        bool match(Ptr<Packet> packet){
            return false;
        }

    private:
        uint32_t packets;
        uint32_t maxPackets;
        double_t priority_level;
        bool isDefault;
        queue<Ptr<Packet>> m_queue;
};