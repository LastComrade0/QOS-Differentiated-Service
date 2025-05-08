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

    if(m_socket){
        cout << "UDP Application properly initialized." << endl;
    }
    
    if (!m_socket) {
        NS_LOG_ERROR("Socket is not set up correctly in UdpApplication::StartApplication");
        return;
    }

    m_running = true;
    m_packet_sent = 0;

    int bindResult = m_socket->Bind();
    if (bindResult == -1) {
        cerr << "UdpApplication Error: Failed to bind socket." << endl;
        return;
    } else {
        cout << "UdpApplication: Socket successfully bound." << endl;
    }

    int connectResult = m_socket->Connect(m_peer);
    if (connectResult == -1) {
        cerr << "UdpApplication Error: Failed to connect socket." << endl;
        return;
    } else {
        cout << "UdpApplication: Socket successfully connected to " 
             << InetSocketAddress::ConvertFrom(m_peer).GetIpv4() 
             << " Port: " << InetSocketAddress::ConvertFrom(m_peer).GetPort() << endl;
    }

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
        m_socket = nullptr;

        cout << "UdpApplication Stopped. Total Packets Sent: " << m_packet_sent << endl;
    }
}

void UdpApplication::SendPacket(){

    if (!m_running) {
        cout << "UdpApplication: Not running. Packet send stopped." << endl;
        NS_LOG_INFO("UdpApplication is not running. Packet send stopped.");
        return;
    }

    if(m_packet_sent < m_packet_num){
        Ptr<Packet> packet = Create<Packet>(m_packet_size);

        int bytesSent = m_socket->SendTo(packet, 0, m_peer);

        if (bytesSent > 0) {
            ++m_packet_sent;
            cout << "Packet " << m_packet_sent << " sent successfully." << endl;
            NS_LOG_INFO("Packet " << m_packet_sent << " sent to " << InetSocketAddress::ConvertFrom(m_peer).GetIpv4() 
                << " Port: " << InetSocketAddress::ConvertFrom(m_peer).GetPort());
        } else {
            NS_LOG_ERROR("Failed to send packet.");
        }

        // Only reschedule if running
        if (m_running) {
            m_send_event = Simulator::Schedule(m_interval, &UdpApplication::SendPacket, this);
        }
        else{
            NS_LOG_INFO("All packets sent. Stopping application.");
            StopApplication();
        }

    }

    
}
