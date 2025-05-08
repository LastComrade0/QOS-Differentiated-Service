#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/type-id.h" 
#include "src_ip.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("SrcIPAddress");

NS_OBJECT_ENSURE_REGISTERED(SrcIPAddress);

TypeId SrcIPAddress::GetTypeId(void){
    TypeId tid = TypeId("ns3::SrcIPAddress").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}

SrcIPAddress::SrcIPAddress(Ipv4Address address) : src_address(address){}

bool SrcIPAddress::match(Ptr<Packet> packet) const {

    Ptr<Packet> copy = packet->Copy();

    PppHeader pppHeader;
    Ipv4Header ipv4Header;

    copy->RemoveHeader(pppHeader);
    copy->PeekHeader(ipv4Header); 

    Ipv4Address extracted_src_address = ipv4Header.GetDestination();

    cout << "Packet src ip: " << extracted_src_address << endl;
    cout << "Criteria src ip: " << src_address << endl;

    return extracted_src_address == src_address;
}

SrcIPAddress::~SrcIPAddress(){}


