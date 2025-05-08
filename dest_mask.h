#ifndef DEST_MASK_H
#define DEST_MASK_H

#include "filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"
#include "ns3/type-id.h" 


using namespace ns3;

class DestMask : public FilterElement{
    public:

        static TypeId GetTypeId(void);

        explicit DestMask(Ipv4Address address, Ipv4Mask mask);

        bool match(Ptr<Packet> packet) const override;

        ~DestMask();


    private:
        Ipv4Mask dest_mask;
        Ipv4Address dest_address;
        
};

#endif