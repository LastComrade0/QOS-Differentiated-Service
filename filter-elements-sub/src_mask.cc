#include "../filter_element.h"
#include "ns3/ipv4-address.h"

using namespace ns3;

class SrcMask : public FilterElement{
    public:
        explicit SrcMask(Ipv4Mask mask, Ipv4Address address) : src_mask(mask), src_address(address){}

        bool match(Ptr<Packet> p) const override{
            Ipv4Header ipv4Header;

            p->PeekHeader(ipv4Header);

            Ipv4Address src = ipv4Header.GetSource();
            
            return src.CombineMask(src_mask) == src_address.CombineMask(src_mask);
        }

    private:
        Ipv4Mask src_mask;
        Ipv4Address src_address;
};