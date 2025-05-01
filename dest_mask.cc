#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 
#include "dest_mask.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("DestMask");

NS_OBJECT_ENSURE_REGISTERED(DestMask);

TypeId DestMask::GetTypeId(void){
    TypeId tid = TypeId("ns3::DestMask").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}


DestMask::DestMask(Ipv4Address address, Ipv4Mask mask) : dest_mask(mask), dest_address(address) {}

bool DestMask::match(Ptr<Packet> packet) const{
    Ipv4Header ipv4Header;
            
    packet->PeekHeader(ipv4Header);

    Ipv4Address extracted_dest = ipv4Header.GetDestination();

    cout << "Packet combine mask: " << extracted_dest.CombineMask(dest_mask) << endl;
    cout << "Criteria combine mask: " << dest_address.CombineMask(dest_mask) << endl;

    return extracted_dest.CombineMask(dest_mask) == dest_address.CombineMask(dest_mask);

}

DestMask::~DestMask(){}

        
