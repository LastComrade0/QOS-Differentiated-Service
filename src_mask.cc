#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 
#include "src_mask.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("SrcMask");

NS_OBJECT_ENSURE_REGISTERED(SrcMask);

TypeId SrcMask::GetTypeId(void){
    TypeId tid = TypeId("ns3::SrcMask").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}


SrcMask::SrcMask(Ipv4Address address, Ipv4Mask mask) : src_mask(mask), src_address(address){}

bool SrcMask::match(Ptr<Packet> packet) const{
    Ptr<Packet> copy = packet->Copy();
    PppHeader pppHeader;
    Ipv4Header ipv4Header;

    copy->RemoveHeader(pppHeader);
    copy->PeekHeader(ipv4Header);

    Ipv4Address extracted_src = ipv4Header.GetSource();

    cout << "Packet src combined mask: " << extracted_src.CombineMask(src_mask) << endl;
    cout << "Criteria src combined mask: " << src_address << endl;
    //cout << "Criteria src combined mask: " << src_address.CombineMask(src_mask) << endl;
            
    return extracted_src.CombineMask(src_mask) == src_address;
}

SrcMask::~SrcMask(){}