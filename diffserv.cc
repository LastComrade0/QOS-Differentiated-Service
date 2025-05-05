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

template <typename Packet>
DiffServ<Packet>::DiffServ() {
    // Default init; leave empty if needed
}

template <typename Packet>
DiffServ<Packet>::~DiffServ(){}

template <typename Packet>
void DiffServ<Packet>::AddTrafficClass(TrafficClass* tc) {
    q_class.push_back(tc);
}

template <typename Packet>
bool DiffServ<Packet>::Enqueue(Ptr<Packet> packet) {
    int class_count = 1;
    
    for(TrafficClass *tc : q_class){
        cout << "Iterating enqueue class " << class_count << endl;

        if(tc->match(packet)){
            cout << "***Successfully enqueued, priority level: " << tc->getPrioirtyLvl() << endl;
            tc->Enqueue(packet);
            return true;
        }
        class_count += 1;
    }

    cout << "Packet does not match any filter, triggering default..." << endl;
    class_count = 0;
    for(TrafficClass *tc2 : q_class){
        cout << "Iterating enqueue class for default: " << class_count << endl;
        if(tc2->isDefaultCheck()){
            cout << "***Successfully enqueued default, priority level: " << tc2->getPrioirtyLvl() << endl;
            tc2->EnqueueDefault(packet);
            return true;
        }
        class_count += 1;
    }

    cout << "Failed enqueueing, should be defaulted" << endl;
    return false;
}

template <typename Packet>
Ptr<Packet> DiffServ<Packet>::Dequeue() {

    return this->Schedule();
}

template <typename Packet>
Ptr<const Packet> DiffServ<Packet>::Peek() const {
    for(TrafficClass *tc: q_class){
        if(!tc->isEmpty()){
            return tc->peek();
        }
    }

    cout << "Unable to peek: q_class empty" << endl;
    return nullptr;
}

template <typename Packet>
Ptr<Packet> DiffServ<Packet>::Remove() {
    for(TrafficClass *tc: q_class){
        if(!tc->isEmpty()){
            return tc->Dequeue();
        }
    }
    return nullptr;
}

template <typename Packet>
bool DiffServ<Packet>::DoEnqueue(Ptr<Packet> packet){
    return Enqueue(packet);
}

template <typename Packet>
Ptr<Packet> DiffServ<Packet>::DoDequeue(){
    return Dequeue();
}

template <typename Packet>
Ptr<Packet> DiffServ<Packet>::DoRemove(){
    return Remove();
}

template <typename Packet>
Ptr<const Packet> DiffServ<Packet>::DoPeek() const{
    return Peek();
}

template class DiffServ<Packet>;