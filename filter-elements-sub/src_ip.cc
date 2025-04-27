#include "../filter_element.h"
#include "ns3/ipv4-address.h"

using namespace ns3;

class SrcIPAddress : public FilterElement{
    public:
        explicit SrcIPAddress(Ipv4Address address) : src_address(address){}

        bool match(Ptr<Packet> p) const override{
            Ipv4Header ipv4Header;
            
            p->PeekHeader(ipv4Header);
            return ipv4Header.GetSource() == src_address;
        }
        

    private:
        Ipv4Address src_address;
};

