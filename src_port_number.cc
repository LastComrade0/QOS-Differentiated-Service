#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/type-id.h" 
#include "src_port_number.h"


using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("SrcPortNumber");

NS_OBJECT_ENSURE_REGISTERED(SrcPortNumber);

TypeId SrcPortNumber::GetTypeId(void){
    TypeId tid = TypeId("ns3::SrcPortNumber").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}

SrcPortNumber::SrcPortNumber(uint32_t src_port) : my_src_port(src_port) {};

bool SrcPortNumber::match(Ptr<Packet> packet) const{

    Ptr<Packet> copy = packet->Copy(); //Create copy of entire packet

    PppHeader pppHeader;
    
    Ipv4Header ipv4Header;

    copy->RemoveHeader(pppHeader);

    copy->RemoveHeader(ipv4Header); //Remove the ip header from that copied packet
    
    UdpHeader udpHeader; //Or UDP?
    copy->PeekHeader(udpHeader);

    uint16_t extracted_src_port = udpHeader.GetSourcePort();

    cout << "Packet src port number: " << static_cast<uint32_t>(extracted_src_port) << endl;
    cout << "Criteria srcc port number: " << my_src_port << endl;

    return  static_cast<uint32_t>(extracted_src_port) == my_src_port;
}

SrcPortNumber::~SrcPortNumber(){}



