#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/type-id.h" 
#include "src_port_number.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SrcPortNumber");

NS_OBJECT_ENSURE_REGISTERED(SrcPortNumber);

TypeId SrcPortNumber::GetTypeId(void){
    TypeId tid = TypeId("ns3::SrcPortNumber").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}

SrcPortNumber::SrcPortNumber(uint32_t src_port) : my_src_port(src_port) {};

bool SrcPortNumber::match(Ptr<Packet> p) const{
    TcpHeader tcpHeader; //TCP or UDP?

    p->PeekHeader(tcpHeader);

    uint16_t extracted_src_port = tcpHeader.GetSourcePort();

    return  static_cast<uint32_t>(extracted_src_port) == my_src_port;
}

SrcPortNumber::~SrcPortNumber(){}



