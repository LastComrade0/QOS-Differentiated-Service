#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/log.h"
//#include "traffic_class.cc"
#include <iostream>
#include <vector>
#include <memory>

using namespace ns3;
using namespace std;

class TrafficClass;

class DiffServ : public Queue<Packet> {

    public:
        bool Enqueue(Ptr<Packet> packet){
            return false;
        }

        bool Dequeue(Ptr<Packet> packet){
            return false;
        }

        Ptr<Packet> Remove(){
            return nullptr;
        }

        Ptr<const Packet> Peek(){
            return nullptr;
        }

        virtual Ptr<Packet> Schedule() = 0;

        virtual uint32_t Classify(Ptr<Packet> p) = 0;


    protected:
        
        vector<TrafficClass*> q_class;

        
};