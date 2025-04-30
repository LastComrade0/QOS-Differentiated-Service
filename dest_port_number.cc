#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/type-id.h" 
#include "dest_port_number.h"



using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DestPortNumber");

NS_OBJECT_ENSURE_REGISTERED(DestPortNumber);

TypeId DestPortNumber::GetTypeId(void){
    TypeId tid = TypeId("ns3::DestPortNumber").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}
    
DestPortNumber::DestPortNumber(uint32_t dest_port) : my_dest_port(dest_port) {};

bool DestPortNumber::match(Ptr<Packet> packet) const{
    TcpHeader tcpHeader; //Or UDP?

    packet->PeekHeader(tcpHeader);

    uint16_t extracted_dest_port = tcpHeader.GetDestinationPort();

    return static_cast<uint32_t>(extracted_dest_port) == my_dest_port;
}

DestPortNumber::~DestPortNumber(){}

    
