#include "../filter_element.h"
#include "ns3/ipv4-address.h"

using namespace ns3;

class DestIPAddress : public FilterElement{
    public:
        explicit DestIPAddress(Ipv4Address address) : dest_address(address) {}
        
        bool match(Ptr<Packet> p) const override{
            Ipv4Header ipv4Header;

            p->PeekHeader(ipv4Header);

            return ipv4Header.GetSource() == dest_address;
        }

    private:

        Ipv4Address dest_address;
};