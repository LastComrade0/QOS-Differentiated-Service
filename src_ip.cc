#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/type-id.h" 
#include "src_ip.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SrcIPAddress");

NS_OBJECT_ENSURE_REGISTERED(SrcIPAddress);

TypeId SrcIPAddress::GetTypeId(void){
    TypeId tid = TypeId("ns3::SrcIPAddress").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}

SrcIPAddress::SrcIPAddress(Ipv4Address address) : src_address(address){}

bool SrcIPAddress::match(Ptr<Packet> packet) const {
    Ipv4Header ipv4Header;
    
    packet->PeekHeader(ipv4Header);

    return ipv4Header.GetSource() == src_address;
}

SrcIPAddress::~SrcIPAddress(){}


