#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"

#include "spq.h"

#include <math.h>

using namespace ns3;
using namespace std;

template<typename Packet>
SPQ<Packet>::SPQ(){}

template<typename Packet>
SPQ<Packet>::~SPQ(){}

template<typename Packet>
Ptr<Packet> SPQ<Packet>::Schedule(){
    for(TrafficClass *tc : this->q_class){
        if(!tc->isEmpty()){
            return tc->Dequeue();
        }
    }
    return nullptr;
}

template<typename Packet>
uint32_t SPQ<Packet>::Classify(Ptr<Packet> packet){
    for(uint32_t traffic_id = 0; traffic_id < this->q_class.size(); traffic_id += 1){
        if(this->q_class[traffic_id]->match(packet)){
            return traffic_id;
        }
    }

    return -1;
}


template<typename Packet>
void SPQ<Packet>::CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector){
    this->q_class = set_traffic_vector;
}

//template<typename Packet>
void insertFilter(TrafficClass* tc, Filter *filter){

    tc->addFilter(filter);

}



int main(){
    Ptr<SPQ<Packet>> spq = CreateObject<SPQ<Packet>>();
    vector<TrafficClass*> traffic_vectors;

    //Create Traffic Class 0
    TrafficClass *tc0 = new TrafficClass();

    tc0->setDefault(false);
    tc0->setMaxPackets(20);
    tc0->setPriorityLvl(0);

    Filter *f0_0 = new Filter();
    f0_0->addElement(new DestPortNumber(80));
    f0_0->addElement(new ProtocolNumber(6));
    insertFilter(tc0, f0_0);

    Filter *f0_1 = new Filter();
    f0_1->addElement(new DestIPAddress("127.0.0.1"));
    insertFilter(tc0, f0_1);

    traffic_vectors.push_back(tc0);

    //Create Traffic Class 1
    TrafficClass *tc1 = new TrafficClass();

    tc1->setDefault(false);
    tc1->setMaxPackets(30);
    tc1->setPriorityLvl(1);

    Filter *f1_0 = new Filter();
    f1_0->addElement(new DestPortNumber(80));
    f1_0->addElement(new ProtocolNumber(1));
    f1_0->addElement(new DestIPAddress("10.0.0.0"));
    insertFilter(tc1, f1_0);

    Filter *f1_1 = new Filter();
    f1_1->addElement(new DestIPAddress("8.8.8.8"));
    f1_1->addElement(new ProtocolNumber(2));
    insertFilter(tc1, f1_1);

    Filter *f1_2 = new Filter();
    f1_2->addElement(new DestIPAddress("10.0.0.1"));
    insertFilter(tc1, f1_2);

    traffic_vectors.push_back(tc1);

    spq->CreateTrafficClassesVector(traffic_vectors);
}