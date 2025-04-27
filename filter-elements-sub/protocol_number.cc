#include "../filter_element.h"
#include "ns3/ipv4-address.h"

using namespace ns3;

class ProtocolNumber : public FilterElement{
    public:
        explicit ProtocolNumber(uint32_t protocol_number) : my_protocol_number(protocol_number) {}

        bool match(Ptr<Packet> packet) const override{
            Ipv4Header ipv4Header;

            packet->PeekHeader(ipv4Header);

            uint8_t extracted_protocol = ipv4Header.GetProtocol();

            return static_cast<uint32_t>(extracted_protocol) == my_protocol_number;

        }

    private:
        uint32_t my_protocol_number;
};