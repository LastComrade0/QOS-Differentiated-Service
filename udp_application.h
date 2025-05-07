#ifndef UDP_APPLICATION_H
#define UDP_APPLICATION_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include <cstdint>

using namespace ns3;
using namespace std;

class UdpApplication : public Application{
    public:
        UdpApplication();
        virtual ~UdpApplication();

        void Setup(Ptr<Socket> socket, Address address, uint32_t packet_size, uint32_t packet_num, Time interval);
    
    private:
        virtual void StartApplication(void) override;
        virtual void StopApplication(void) override;
        void SendPacket(void);

        Ptr<Socket> m_socket;
        Address m_peer;
        uint32_t m_packet_size;
        uint32_t m_packet_num;
        uint32_t m_packet_sent;
        Time m_interval;
        EventId m_send_event;
        bool m_running;
};

#endif