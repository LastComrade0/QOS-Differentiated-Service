#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "filter.h"

using namespace ns3;
using namespace std;



NS_LOG_COMPONENT_DEFINE("FilterTest");

int main() {

    LogComponentEnable("FilterTest", LOG_LEVEL_INFO);

    // Create a packet and set IP + TCP headers
    Ptr<Packet> packet = Create<Packet>();

    

    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(5555);
    tcpHeader.SetDestinationPort(80);
    packet->AddHeader(tcpHeader); // Stack order: TCP first, then IP

    Ipv4Header ipHeader;
    ipHeader.SetSource(Ipv4Address("192.168.1.1"));
    ipHeader.SetDestination(Ipv4Address("10.0.0.1"));
    ipHeader.SetProtocol(6); // TCP
    packet->AddHeader(ipHeader);

    packet->PeekHeader(ipHeader);
    std::cout << "Top header source IP: " << ipHeader.GetSource() << std::endl;

    // Create filter (all elements constructed here)
    Filter filter(
        new SrcIPAddress(Ipv4Address("192.168.1.1")),
        new SrcMask(Ipv4Address("192.168.1.1"), Ipv4Mask("255.255.255.255")),
        new SrcPortNumber(5555),
        new DestIPAddress(Ipv4Address("10.0.0.1")),
        new DestMask(Ipv4Address("10.0.0.1"), Ipv4Mask("255.255.255.255")),
        new DestPortNumber(80),
        new ProtocolNumber(6)
    );

    // Run match test
    bool result = filter.match(packet);
    NS_LOG_INFO("Filter match result: " << (result ? "Matched" : "Not Matched"));

    return 0;
}