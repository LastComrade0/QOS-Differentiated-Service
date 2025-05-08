#ifndef SRC_MASK_H
#define SRC_MASK_H


#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 

using namespace ns3;

class SrcMask : public FilterElement{
    public:

        static TypeId GetTypeId(void);

        explicit SrcMask(Ipv4Address address, Ipv4Mask mask);

        bool match(Ptr<Packet> packet) const override;

        ~SrcMask();

    private:
        Ipv4Mask src_mask;
        Ipv4Address src_address;
};

#endif