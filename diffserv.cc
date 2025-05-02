#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "diffserv.h"
#include <iostream>
#include <vector>
#include <memory>

using namespace ns3;
using namespace std;

DiffServ::DiffServ() {
    // Default init; leave empty if needed
}

DiffServ::~DiffServ(){}

bool DiffServ::Enqueue(Ptr<Packet> packet) {
    for(TrafficClass *tc : q_class){
        if(tc->match(packet)){
            tc->Enqueue(packet);
            return true;
        }
    }

    return false;
}

Ptr<Packet> DiffServ::Dequeue() {
    return nullptr;
}

Ptr<const Packet> DiffServ::Peek() const {
    return nullptr;
}

Ptr<Packet> DiffServ::Remove() {
    return nullptr;
}