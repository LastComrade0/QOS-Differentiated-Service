#include "udp_application.h"

using namespace ns3;
using namespace std;

NS_OBJECT_ENSURE_REGISTERED(UdpApplication);
NS_LOG_COMPONENT_DEFINE("UdpApplication");



UdpApplication::UdpApplication() :
    m_socket(nullptr), m_packet_size(0), m_packet_num(0), m_packet_sent(0), m_running(false){}

UdpApplication::~UdpApplication(){
    if(m_socket){
        m_socket->Close();
        m_socket = nullptr;
    }
}

void UdpApplication::Setup(Ptr<Socket> socket, Address address, uint32_t packet_size, uint32_t packet_num, Time interval){
    if (!socket) {
        NS_LOG_ERROR("Socket is null in UdpApplication::Setup");
        return;
    }    
    
    m_socket = socket;
    m_peer = address;
    m_packet_size = packet_size;
    m_packet_num = packet_num;
    m_interval = interval;
}

void UdpApplication::StartApplication(){
    if (!m_socket) {
        NS_LOG_ERROR("Socket is not set up correctly in UdpApplication::StartApplication");
        return;
    }

    m_running = true;
    m_packet_sent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void UdpApplication::StopApplication(){
    // m_running = false;

    // if (m_send_event.IsRunning()) {
    //     Simulator::Cancel(m_send_event);
    // }

    // if (m_socket) {
    //     m_socket->Close();
    //     m_socket = nullptr;
    // }

    // NS_LOG_INFO("UdpApplication stopped. Total Packets Sent: " << m_packet_sent);
    m_running = false;

    if(m_send_event.IsPending()){
        Simulator::Cancel(m_send_event);
    }

    if(m_socket){
        m_socket->Close();
    }
}

void UdpApplication::SendPacket(){

    if(m_packet_sent < m_packet_num){
        Ptr<Packet> packet = Create<Packet>(m_packet_size);

        m_socket->SendTo(packet, 0, m_peer);

        ++m_packet_sent;
        m_send_event = Simulator::Schedule(m_interval, &UdpApplication::SendPacket, this);

    }

    
}
