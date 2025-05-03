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

void DiffServ::AddTrafficClass(TrafficClass* tc) {
    q_class.push_back(tc);
}

bool DiffServ::Enqueue(Ptr<Packet> packet) {
    int class_count = 1;
    
    for(TrafficClass *tc : q_class){
        cout << "Iterating enqueue class " << class_count << endl;
        if(tc->match(packet)){
            tc->Enqueue(packet);
            return true;
        }
        class_count += 1;
    }

    return false;
}

Ptr<Packet> DiffServ::Dequeue() {

    return Schedule();
}

Ptr<const Packet> DiffServ::Peek() const {
    for(TrafficClass *tc: q_class){
        if(!tc->isEmpty()){
            return tc->peek();
        }
    }

    cout << "Unable to peek: q_class empty" << endl;
    return nullptr;
}

Ptr<Packet> DiffServ::Remove() {
    for(TrafficClass *tc: q_class){
        if(!tc->isEmpty()){
            return tc->Dequeue();
        }
    }
    return nullptr;
}