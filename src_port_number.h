#ifndef SRC_PORT_NUMBER_H
#define SRC_PORT_NUMBER_H

#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 

using namespace ns3;

class SrcPortNumber : public FilterElement{
    public:

        static TypeId GetTypeId(void);

        explicit SrcPortNumber(uint32_t src_port);

        bool match(Ptr<Packet> p) const override;
        
        ~SrcPortNumber();


    private:
        uint32_t my_src_port;
};

#endif