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
#include "filter_element.h"
#include "dest_ip.h"
#include "dest_port_number.h"
#include "protocol_number.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("DiffServTest");

// Dummy scheduler: return first non-empty class
template <typename Packet>
class TestDiffServ : public DiffServ<Packet> {
public:
    Ptr<Packet> Schedule() override {
        for (TrafficClass* tc : this->q_class) {
            if (!tc->isEmpty()) {
                return tc->Dequeue();
            }
        }
        return nullptr;
    }

    uint32_t Classify(Ptr<Packet> p) override {
        return 0; // unused in this test
    }
};

// Create a TrafficClass with basic filter (e.g., TCP port 80)
TrafficClass* MakeClass(Ipv4Address destIp, uint16_t port, uint8_t proto, uint32_t priority, bool isDefault = false) {
    auto* tc = new TrafficClass(); //10, priority, isDefault
    tc->setMaxPackets(10);
    tc->setPriorityLvl(priority);
    tc->setDefault(isDefault);


    Filter* f = new Filter();
    f->addElement(new DestPortNumber(port));
    f->addElement(new ProtocolNumber(proto));
    tc->addFilter(f);

    Filter* f2 = new Filter();
    f2->addElement(new DestIPAddress(destIp));
    tc->addFilter(f2);

    return tc;
}

TrafficClass* MakeClass2(Ipv4Address destIp, uint16_t port, uint8_t proto, uint32_t priority, bool isDefault = false) {
    auto* tc2 = new TrafficClass(); //10, priority, isDefault
    tc2->setMaxPackets(10);
    tc2->setPriorityLvl(priority);
    tc2->setDefault(isDefault);


    Filter* f = new Filter();
    f->addElement(new DestPortNumber(port));
    f->addElement(new DestIPAddress(destIp));
    tc2->addFilter(f);

    Filter* f2 = new Filter();
    f2->addElement(new ProtocolNumber(proto));
    tc2->addFilter(f2);

    return tc2;
}

int main() {
    TestDiffServ<Packet> ds;

    // Add 2 classes: one for 8.8.8.8:80 and one fallback
    ds.AddTrafficClass(MakeClass("8.8.8.8", 80, 6, 0));
    ds.AddTrafficClass(MakeClass("0.0.0.0", 0, 6, 2)); // default

    ds.AddTrafficClass(MakeClass2("10.0.0.1", 70, 1, 1));

    TrafficClass* defaultTrafficClass = new TrafficClass();
    defaultTrafficClass->setDefault(true);
    defaultTrafficClass->setPriorityLvl(3);

    ds.AddTrafficClass(defaultTrafficClass);

    // Create matching packet for 8.8.8.8:80
    //Packet 1
    Ptr<Packet> pkt1 = Create<Packet>(100);
    Ipv4Header ip;
    ip.SetDestination("8.8.8.8");
    ip.SetProtocol(6); // TCP

    TcpHeader tcp;
    tcp.SetDestinationPort(70);

    pkt1->AddHeader(tcp);
    pkt1->AddHeader(ip);

    //Packet 2
    Ptr<Packet> pkt2 = Create<Packet>(100);
    Ipv4Header ip2;
    ip2.SetDestination("0.0.0.1");
    ip2.SetProtocol(6); // TCP

    TcpHeader tcp2;
    tcp2.SetDestinationPort(70);

    pkt2->AddHeader(tcp2);
    pkt2->AddHeader(ip2);

    // Enqueue
    bool enq = ds.Enqueue(pkt1);
    std::cout << "Enqueue: " << (enq ? "Success" : "Fail") << std::endl;

    // Peek
    Ptr<const Packet> peek = ds.Peek();
    std::cout << "Peek: " << (peek ? "Present" : "None") << std::endl;

    // Dequeue
    Ptr<Packet> dq = ds.Dequeue();
    std::cout << "Dequeue: " << (dq ? "Success" : "Fail") << std::endl;

    cout << "ooooooooooooooooooooooooooooooo" << endl;
    
    /*Packet 2 Enqueue */
    // Enqueue
    bool enq2 = ds.Enqueue(pkt2);
    std::cout << "Enqueue: " << (enq2 ? "Success" : "Fail") << std::endl;

    // Peek
    Ptr<const Packet> peek2 = ds.Peek();
    std::cout << "Peek: " << (peek2 ? "Present" : "None") << std::endl;

    // Dequeue
    Ptr<Packet> dq2 = ds.Dequeue();
    std::cout << "Dequeue: " << (dq2 ? "Success" : "Fail") << std::endl;

    return 0;
}