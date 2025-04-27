#include "../filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"


using namespace ns3;

class DestPortNumber : public FilterElement{
    public:
        explicit DestPortNumber(uint32_t dest_port) : my_dest_port(dest_port) {};

        bool match(Ptr<Packet> packet) const override{
            TcpHeader tcpHeader;

            packet->PeekHeader(tcpHeader);

            uint16_t extracted_dest_port = tcpHeader.GetSourcePort();

            return static_cast<uint32_t>(extracted_dest_port) == my_dest_port;
        }

    private:
        uint32_t my_dest_port;
};