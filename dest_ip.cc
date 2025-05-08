#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ppp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/type-id.h" 
#include "dest_ip.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("DestIPAddress");

NS_OBJECT_ENSURE_REGISTERED(DestIPAddress);

TypeId DestIPAddress::GetTypeId(void){
    TypeId tid = TypeId("ns3::DestIPAddress").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}


DestIPAddress::DestIPAddress(Ipv4Address address) : dest_address(address){}

bool DestIPAddress::match(Ptr<Packet> packet) const{

    Ptr<Packet> copy = packet->Copy();

    PppHeader pppHeader;
    Ipv4Header ipv4Header;

    copy->RemoveHeader(pppHeader);
    copy->PeekHeader(ipv4Header); 
    
    Ipv4Address extracted_dest_address = ipv4Header.GetDestination();

    cout << "Packet destination address: " << extracted_dest_address << endl;
    cout << "Criteria destination address: " << dest_address << endl;
    
    return extracted_dest_address == dest_address;
}

DestIPAddress::~DestIPAddress(){}