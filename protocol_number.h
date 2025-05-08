#ifndef PROTOCOL_NUMBER_H
#define PROTOCOL_NUMBER_H

#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"
#include "ns3/type-id.h" 
#include "ns3/log.h"

using namespace ns3;

class ProtocolNumber : public FilterElement{
    public:

        static TypeId GetTypeId(void);

        explicit ProtocolNumber(uint32_t protocol_number);

        bool match(Ptr<Packet> packet) const override;

        ~ProtocolNumber();

    private:
        uint32_t my_protocol_number;
};

#endif