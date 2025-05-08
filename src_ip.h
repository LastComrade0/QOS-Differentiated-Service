#ifndef SRC_IP_ADDRESS_H
#define SRC_IP_ADDRESS_H

#include "filter_element.h"
#include "ns3/ppp-header.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"
#include "ns3/type-id.h" 
#include "ns3/log.h"

using namespace ns3;

class SrcIPAddress : public FilterElement {
public:

    static TypeId GetTypeId(void);

    explicit SrcIPAddress(Ipv4Address address);
    
    bool match(Ptr<Packet> packet) const override;

    ~SrcIPAddress();

private:
    Ipv4Address src_address;
};

#endif // DEST_IP_ADDRESS_H