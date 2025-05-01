  /* CS621: Lab 3 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 
#include <memory>
#include <vector>


#include "filter_element.h"
#include "src_ip.h"
#include "src_port_number.h"
#include "dest_ip.h"
#include "dest_port_number.h"
#include "dest_mask.h"
#include "src_mask.h"
#include "protocol_number.h"
#include "filter.h"
//#include "traffic_class.cc"
//#include "diffserv.cc"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestFilterComponent");

int main (int argc, char *argv[]){

    //Test

    Ptr<Packet> p = Create<Packet>(100); // 100 bytes payload

    // Step 2: Create and add IPv4 Header
    Ipv4Header ipv4Header;
    ipv4Header.SetSource(Ipv4Address("10.1.1.1"));
    ipv4Header.SetDestination(Ipv4Address("10.1.1.2"));
    ipv4Header.SetProtocol(6); // TCP
    ipv4Header.SetPayloadSize(100);
  
    p->AddHeader(ipv4Header);
  
    // Step 3: Create your SrcIPAddress filter
    SrcIPAddress filter(Ipv4Address("10.1.1.1"));
  
    // Step 4: Call match and check
    bool result = filter.match(p);
  
    if (result) {
        std::cout << "Test PASSED: Packet matches SrcIPAddress filter." << std::endl;
    } else {
        std::cout << "Test FAILED: Packet does NOT match SrcIPAddress filter." << std::endl;
    }

  /*
  // Fill IP + TCP headers
    Ipv4Header ipHeader;
    ipHeader.SetSource(Ipv4Address("10.1.1.1"));
    ipHeader.SetDestination(Ipv4Address("10.1.1.2"));
    ipHeader.SetProtocol(6); // TCP

    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(1234);
    tcpHeader.SetDestinationPort(80);

    // Add headers to packet
    packet->AddHeader(tcpHeader);
    packet->AddHeader(ipHeader);

    // Construct filters
    SrcIPAddress* src_ip = new SrcIPAddress(Ipv4Address("10.1.1.1"));
    SrcMask* src_mask = new SrcMask(Ipv4Mask("255.255.255.0")); // Assuming matches 10.1.1.x
    SrcPortNumber* src_port = new SrcPortNumber(1234);
    DestIPAddress* dest_ip = new DestIPAddress(Ipv4Address("10.1.1.2"));
    DestMask* dest_mask = new DestMask(Ipv4Mask("255.255.255.0")); // Assuming matches 10.1.1.x
    DestPortNumber* dest_port = new DestPortNumber(80);
    ProtocolNumber* proto = new ProtocolNumber(6); // TCP

    // Construct Filter
    Filter filter(src_ip, src_mask, src_port, dest_ip, dest_mask, dest_port, proto);

    // Run test
    bool result = filter.match(packet);
    std::cout << "Packet match result: " << (result ? "PASS" : "FAIL") << std::endl;*/

    return 0;


}
  
  