#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "src_ip.h"
#include "src_mask.h"
#include "src_port_number.h"
#include "dest_ip.h"
#include "dest_mask.h"
#include "dest_port_number.h"
#include "protocol_number.h"
#include "filter.h"
#include "traffic_class.h"

using namespace ns3;
using namespace std;

int main() {
    // Step 1: Create headers
    Ipv4Address srcIp("10.1.1.1");
    Ipv4Address destIp("10.1.1.2");
    Ipv4Mask mask("255.255.255.0");

    Ipv4Address destIp2("8.8.8.8");
    Ipv4Mask mask2("255.255.255.255");

    // Step 2: Create filter elements
    //SrcIPAddress* srcElem = new SrcIPAddress(srcIp);
    //SrcMask* srcMask = new SrcMask(srcIp, mask);
    //SrcPortNumber* srcPort = new SrcPortNumber(50000);
    //DestIPAddress* destElem = new DestIPAddress(destIp);
    //DestMask* destMask = new DestMask(destIp, mask);
    DestPortNumber* destPort = new DestPortNumber(80);
    ProtocolNumber* proto = new ProtocolNumber(6); // TCP

    //SrcIPAddress* srcElem2 = new SrcIPAddress(srcIp);
    //SrcMask* srcMask2 = new SrcMask(srcIp, mask);
    //SrcPortNumber* srcPort2 = new SrcPortNumber(50000);
    DestIPAddress* destElem2 = new DestIPAddress(destIp2);
    DestMask* destMask2 = new DestMask(destIp2, mask2); //Different mask 
    //DestPortNumber* destPort2 = new DestPortNumber(80);
    //ProtocolNumber* proto2 = new ProtocolNumber(6); // TCP

    // Step 3: Create a Filter and put it in a vector
    Filter* f = new Filter();
    f->addElement(proto);
    f->addElement(destPort);

    Filter* f2 = new Filter();
    f2->addElement(destElem2);
    f2->addElement(destMask2);

    vector<Filter*> fv;
    fv.push_back(f);
    fv.push_back(f2);

    // Step 4: Construct TrafficClass with the filter
    TrafficClass* tc = new TrafficClass(10, 300, new uint32_t(0), false, srcIp, mask, 50000, destIp, mask, 80, 6);
    
    // Manually override filter_vector (bypassing constructor for test flexibility)
    tc->filter_vector = fv;

    // Step 5: Create a packet and attach headers
    //Pkt 1
    Ptr<Packet> packet = Create<Packet>(100);
    Ipv4Header ipHeader;
    ipHeader.SetSource(srcIp);
    ipHeader.SetDestination(destIp);
    ipHeader.SetProtocol(6); // TCP

    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(50000);
    tcpHeader.SetDestinationPort(81);

    packet->AddHeader(tcpHeader);
    packet->AddHeader(ipHeader);

    //Pkt 2
    Ptr<Packet> packet2 = Create<Packet>(100);
    Ipv4Header ipHeader2;
    ipHeader2.SetSource(srcIp);
    ipHeader2.SetDestination(destIp2);
    ipHeader2.SetProtocol(6); // TCP

    TcpHeader tcpHeader2;
    tcpHeader2.SetSourcePort(50000);
    tcpHeader2.SetDestinationPort(70);

    packet2->AddHeader(tcpHeader2);
    packet2->AddHeader(ipHeader2);

    // Step 6: Test Enqueue and Dequeue

    cout << "Packet 1 " << endl;
    if (tc->Enqueue(packet)) {
        cout << "Enqueue: Success" << endl;
    } else {
        cout << "Enqueue: Failed" << endl;
    }

    Ptr<Packet> out = tc->Dequeue();
    if (out != nullptr) {
        cout << "Dequeue: Success" << endl;
    } else {
        cout << "Dequeue: Failed" << endl;
    }

    //For packet 2

    cout << "**********************" << endl;

    cout << "Packet 2 " << endl;

    if (tc->Enqueue(packet2)) {
        cout << "Enqueue: Success" << endl;
        cout << "Queue size after enqueue: " << tc->getQueueSize() << endl;
    } else {
        cout << "Enqueue: Failed" << endl;
    }

    Ptr<Packet> out2 = tc->Dequeue();
    if (out2 != nullptr) {
        cout << "Dequeue: Success" << endl;
        cout << "Queue size after dequeue: " << tc->getQueueSize() << endl;
    } else {
        cout << "Dequeue: Failed" << endl;
    }

    return 0;
}