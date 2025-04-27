#include "../filter_element.h"
#include "ns3/ipv4-address.h"

using namespace ns3;

class DestMask : public FilterElement{
    public:
        explicit DestMask(Ipv4Address address, Ipv4Mask mask) : dest_mask(mask), dest_address(address) {}

        bool match(Ptr<Packet> packet) const override{
            Ipv4Header ipv4Header;
            
            packet->PeekHeader(ipv4Header);

            Ipv4Address extracted_dest = ipv4Header.GetSource();

            return extracted_dest.CombineMask(dest_mask) == dest_address.CombineMask(dest_mask);

        }


    private:
        Ipv4Mask dest_mask;
        Ipv4Address dest_address;
        
};