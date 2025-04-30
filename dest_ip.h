#ifndef DEST_IP_ADDRESS_H
#define DEST_IP_ADDRESS_H

#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 

using namespace ns3;

class DestIPAddress : public FilterElement {
public:

    static TypeId GetTypeId(void);

    explicit DestIPAddress(Ipv4Address address);
    
    bool match(Ptr<Packet> packet) const override;

    ~DestIPAddress();

private:
    Ipv4Address dest_address;
};

#endif // DEST_IP_ADDRESS_H