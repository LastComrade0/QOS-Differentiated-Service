#include "filter_element.h"
#include "protocol_number.h"
#include "ns3/type-id.h" 
#include "ns3/log.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("ProtocolNumber");

NS_OBJECT_ENSURE_REGISTERED(ProtocolNumber);

TypeId ProtocolNumber::GetTypeId(void){
    TypeId tid = TypeId("ns3::ProtocolNumber").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}

ProtocolNumber::ProtocolNumber(uint32_t protocol_number) : my_protocol_number(protocol_number) {}

bool ProtocolNumber::match(Ptr<Packet> packet) const{
    Ipv4Header ipv4Header;

    packet->PeekHeader(ipv4Header);

    uint8_t extracted_protocol = ipv4Header.GetProtocol();

    cout << "Packet protocol number: " <<  static_cast<uint32_t>(extracted_protocol) << endl;
    cout << "Criteria protocol number: " << my_protocol_number << endl;

    return static_cast<uint32_t>(extracted_protocol) == my_protocol_number;

}

ProtocolNumber::~ProtocolNumber(){}

