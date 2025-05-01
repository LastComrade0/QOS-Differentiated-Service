#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "filter.h"
#include "filter_element.h"
#include "src_ip.h"
#include "src_mask.h"
#include "src_port_number.h"
#include "dest_ip.h"
#include "dest_mask.h"
#include "dest_port_number.h"
#include "protocol_number.h"

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Filter");

NS_OBJECT_ENSURE_REGISTERED(Filter);


Filter::Filter(SrcIPAddress* src_ip, SrcMask* src_mask, SrcPortNumber* src_port,
    DestIPAddress* dest_ip, DestMask* dest_mask, DestPortNumber* dest_port,
    ProtocolNumber* protocol_number){

        filter_elements.push_back(src_ip);
        filter_elements.push_back(src_mask);
        filter_elements.push_back(src_port);
        filter_elements.push_back(dest_ip);
        filter_elements.push_back(dest_mask);
        filter_elements.push_back(dest_port);
        filter_elements.push_back(protocol_number);

}

bool Filter::match(Ptr<Packet> packet) const {
    for (const FilterElement* f : filter_elements) {
        if (!f->match(packet)) {
            return false;
        }
    }
    return true;
}

Filter::~Filter(){}
