#include "../filter_element.h"
#include "ns3/ipv4-address.h"
#include "ns3/tcp-header.h"

using namespace ns3;

class SrcPortNumber : public FilterElement{
    public:
        explicit SrcPortNumber(uint32_t src_port) : my_src_port(src_port) {};

        bool match(Ptr<Packet> p) const override{
            TcpHeader tcpHeader;

            p->PeekHeader(tcpHeader);

            uint16_t extracted_src_port =  tcpHeader.GetSourcePort();

            return  static_cast<uint32_t>(extracted_src_port) == my_src_port;
        }

    private:
        uint32_t my_src_port;
};