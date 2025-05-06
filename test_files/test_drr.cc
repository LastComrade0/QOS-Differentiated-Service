#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"

#include "drr.h"

#include <math.h>

using namespace ns3;
using namespace std;

template<typename Packet>
DRR<Packet>::DRR(){
    current_robin = -1;
}

template<typename Packet>
DRR<Packet>::~DRR(){}

template<typename Packet>
Ptr<Packet> DRR<Packet>::Schedule(){
    uint32_t class_count = this->q_class.size();

    cout << "Class count: " << class_count << endl;

    for(uint32_t i = 0; i < class_count; i += 1){
        current_robin = (current_robin + 1) % class_count;
        cout << "Current robin: " << current_robin << endl;

        TrafficClass *tc = this->q_class[current_robin];
        cout << "Current Traffic Class: " << endl;

        if(!tc->isEmpty()){
            tc->setDeficitCounter(tc->getDeficitCounter() + tc->getQuantumSize());
            

            while(!tc->isEmpty()){
                cout << "[Schedule] Checking class " << current_robin << ", queue size: " << tc->getQueueSize() << endl;
                Ptr<Packet> packet = tc->peek();

                cout << "[Schedule] Updated deficitCounter: " << tc->getDeficitCounter() << ", quantum: " << tc->getQuantumSize() << endl;

                if (!packet) {
                    cout << "[Schedule] NULL packet from peek(), breaking inner loop" << endl;
                    break; // nothing to process
                }

                uint32_t size = packet->GetSize();
                cout << "[Schedule] Packet size: " << size << ", deficit: " << tc->getDeficitCounter() << endl;

                if(size <= tc->getDeficitCounter()){
                    tc->setDeficitCounter(tc->getDeficitCounter() - size);
                    cout << "[Schedule] Sending packet..." << endl;
                    return tc->Dequeue();
                }
                else{
                    cout << "[Schedule] Deficit too small, skipping..." << endl;
                    break;
                }
            }


            if(tc->isEmpty()){
                tc->setDeficitCounter(0);
            }
        }
    }
    return nullptr;
}

template<typename Packet>
uint32_t DRR<Packet>::Classify(Ptr<Packet> packet){
    const uint32_t INVALID_CLASS_ID = std::numeric_limits<uint32_t>::max();
    
    for(uint32_t traffic_id = 0; traffic_id < this->q_class.size(); traffic_id += 1){
        if(this->q_class[traffic_id]->match(packet)){
            return traffic_id;
        }
    }

    cout << "Classifying Default... " << endl;
    for(uint32_t traffic_default = 0; traffic_default < this->q_class.size(); traffic_default += 1){
        if(this->q_class[traffic_default]->isDefaultCheck()){
            cout << "Classifying at default traffic calss with priority: " << traffic_default << endl;
            return traffic_default;
        }
    }

    return INVALID_CLASS_ID;
}


template<typename Packet>
void DRR<Packet>::CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector){
    this->q_class = set_traffic_vector;
}

//template<typename Packet>
void insertFilter(TrafficClass* tc, Filter *filter){

    tc->addFilter(filter);

}

void insertTrafficClass(vector<TrafficClass*>& traffic_class_vector_local, TrafficClass* tc){
    auto it = traffic_class_vector_local.begin();
    cout << "Priority to insert: " << tc->getPriorityLvl() << endl;
    while (it != traffic_class_vector_local.end() && (*it)->getPriorityLvl() > tc->getPriorityLvl()) {
        size_t index = std::distance(traffic_class_vector_local.begin(), it);
        cout << "To insert index: " << index << endl;
        ++it;
    }
    traffic_class_vector_local.insert(it, tc);
}

template<typename Packet>
void DRR<Packet>::CheckQueue(){
    for(TrafficClass* it_traffic: this->q_class){
        cout << "Iterate priority: " << it_traffic->getPriorityLvl() << ", isDefault? " << it_traffic->isDefaultCheck() << endl;
    }
}

template<typename Packet>
vector<TrafficClass*> DRR<Packet>::getTrafficVector(){
    return this->q_class;
}

template<typename Packet>
bool DRR<Packet>::testEnqueue(Ptr<Packet> packet){
    return this->DoEnqueue(packet);
}

template<typename Packet>
Ptr<Packet> DRR<Packet>::testDequeue(){
    return this->DoDequeue();
}


int main(){
    cout << "Running DRR..." << endl;

    Ptr<DRR<Packet>> drr = CreateObject<DRR<Packet>>();
    vector<TrafficClass*> traffic_vectors;

    //Create Traffic Class 0
    TrafficClass *tc0 = new TrafficClass();

    tc0->setDefault(false);
    tc0->setMaxPackets(20);
    tc0->setPriorityLvl(2);
    tc0->setDeficitCounter(0);
    tc0->setQuantumSize(500);

    Filter *f0_0 = new Filter();
    f0_0->addElement(new DestPortNumber(80));
    f0_0->addElement(new ProtocolNumber(6));
    insertFilter(tc0, f0_0);

    Filter *f0_1 = new Filter();
    f0_1->addElement(new DestIPAddress("127.0.0.1"));
    insertFilter(tc0, f0_1);

    insertTrafficClass(traffic_vectors, tc0);

    //Create Default Traffic Class 2
    TrafficClass *tc3 = new TrafficClass();

    tc3->setPriorityLvl(0);
    tc3->setDefault(true);
    tc3->setMaxPackets(30);
    tc3->setDeficitCounter(0);
    tc3->setQuantumSize(500);

    insertTrafficClass(traffic_vectors, tc3);

    //Create Traffic Class 1
    TrafficClass *tc1 = new TrafficClass();

    tc1->setDefault(false);
    tc1->setMaxPackets(30);
    tc1->setPriorityLvl(1);
    tc0->setDeficitCounter(0);
    tc3->setQuantumSize(500);

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

    insertTrafficClass(traffic_vectors, tc1);

    /*Testing correctness of inserting traffic class based on priority */
    /*
    TrafficClass *tc2 = new TrafficClass();

    tc2->setPriorityLvl(2);

    insertTrafficClass(traffic_vectors, tc2);

    TrafficClass *tc3 = new TrafficClass();

    tc3->setPriorityLvl(1);

    insertTrafficClass(traffic_vectors, tc3);
    */

    drr->CreateTrafficClassesVector(traffic_vectors);

    drr->CheckQueue();

    //Create Packet 1
    // --- Construct packet: TCP port 80 ---
    Ptr<Packet> pkt = Create<Packet>(100);
    Ipv4Header ipHeader;
    ipHeader.SetProtocol(6); // TCP
    ipHeader.SetDestination("0.0.0.0");
 
    TcpHeader tcpHeader;
    tcpHeader.SetDestinationPort(80);
 
    pkt->AddHeader(tcpHeader);
    pkt->AddHeader(ipHeader);

    //Create packet 2 
    Ptr<Packet> pkt2 = Create<Packet>(100);
    Ipv4Header ipHeader2;
    ipHeader2.SetProtocol(6); // TCP
    ipHeader2.SetDestination("0.0.0.1");
 
    TcpHeader tcpHeader2;
    tcpHeader2.SetDestinationPort(70);
 
    pkt2->AddHeader(tcpHeader2);
    pkt2->AddHeader(ipHeader2);

    //Classify packet 1
    uint32_t classId = drr->Classify(pkt);
    cout << "Classified into class: " << classId << endl;

    if (classId >= drr->getTrafficVector().size()) {
        cerr << "Classification failed!" << endl;
        return 1;
    }
    
    cout << " - * - * - * - * - * - * - * - * - * - * - * - * - * - *" << endl;

    // --- Enqueue packet 1 into correct class ---
    bool success = drr->testEnqueue(pkt);
    cout << "Enqueue success: " << (success ? "true" : "false") << endl;

    cout << "Current size for class " << classId << ": " << drr->getTrafficVector()[classId]->getQueueSize() << endl;

    //Classify packet 2
    uint32_t classId2 = drr->Classify(pkt2);
    cout << "Classified into class: " << classId2 << endl;

    if (classId2 >= drr->getTrafficVector().size()) {
        cerr << "Classification failed!" << endl;
        return 1;
    }

    // --- Enqueue packet 2 into correct class ---
    bool success2 = drr->testEnqueue(pkt2); //Enqueue direct call or under Classify()?
    cout << "Enqueue success: " << (success2 ? "true" : "false") << endl;

    cout << "Current size for class " << classId2 << ": " << drr->getTrafficVector()[classId2]->getQueueSize() << endl;

    //Test Schedule

    for(int pkt_count = 2; pkt_count > 0; pkt_count -= 1){
        cout << "-------------------" << endl;
        Ptr<Packet> scheduledPkt = drr->testDequeue();

        Ipv4Header ipHeaderExtract;
        TcpHeader tcpHeaderExtract;

    

        if (scheduledPkt) {
            scheduledPkt->RemoveHeader(ipHeaderExtract);
            scheduledPkt->RemoveHeader(tcpHeaderExtract);

            cout << "Scheduled a packet successfully!" << endl;
            cout << "Destination IP: " << ipHeaderExtract.GetDestination() << endl;
            cout << "Protocol: " << (uint16_t)ipHeaderExtract.GetProtocol() << endl;
            cout << "Destination port: " << tcpHeaderExtract.GetDestinationPort() << endl;
        } else {
            cout << "Schedule returned nullptr — all queues empty." << endl;
        }
    }


}