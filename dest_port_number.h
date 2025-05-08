#ifndef DEST_PORT_NUMBER_H
#define DEST_PORT_NUMBER_H

#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 

using namespace ns3;

class DestPortNumber : public FilterElement{
    public:
        static TypeId GetTypeId(void);

        explicit DestPortNumber(uint32_t dest_port);

        bool match(Ptr<Packet> packet) const override;

        ~DestPortNumber();

    private:
        uint32_t my_dest_port;
};

#endif