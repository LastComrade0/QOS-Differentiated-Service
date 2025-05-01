#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/type-id.h" 
#include "dest_port_number.h"



using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("DestPortNumber");

NS_OBJECT_ENSURE_REGISTERED(DestPortNumber);

TypeId DestPortNumber::GetTypeId(void){
    TypeId tid = TypeId("ns3::DestPortNumber").SetParent<FilterElement>().SetGroupName("trafficControl");
    return tid;

}
    
DestPortNumber::DestPortNumber(uint32_t dest_port) : my_dest_port(dest_port) {};

bool DestPortNumber::match(Ptr<Packet> packet) const{

    Ptr<Packet> copy = packet->Copy(); //Create copy of entire packet
    
    Ipv4Header ipv4Header;
    copy->RemoveHeader(ipv4Header); //Remove the ip header from that copied packet
    
    TcpHeader tcpHeader; //Or UDP?
    copy->PeekHeader(tcpHeader);

    uint16_t extracted_dest_port = tcpHeader.GetDestinationPort();

    cout << "Packet dest port number : " << static_cast<uint32_t>(extracted_dest_port) << endl;
    cout << "Criteria dest port number: " << my_dest_port << endl;

    return static_cast<uint32_t>(extracted_dest_port) == my_dest_port;
}

DestPortNumber::~DestPortNumber(){}

    
