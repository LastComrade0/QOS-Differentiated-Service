#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ppp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

#include "spq.h"
//#include "udp_application.h"
#include <map>
#include <vector>
#include <queue>

#include <math.h>

using namespace ns3;
using namespace std;

template<typename Packet>
SPQ<Packet>::SPQ(){}

template<typename Packet>
SPQ<Packet>::~SPQ(){}

template <typename Packet>
bool SPQ<Packet>::DoEnqueue(Ptr<Packet> packet){
    cout << "[SPQ] DoEnqueue() called" << endl;
    uint32_t classId = this->Classify(packet);
    cout << "Class id to enqueue: " << classId << endl;
    if (classId < this->q_class.size()) {
        return DiffServ<Packet>::Enqueue(packet);
    } else {
        std::cerr << "Classification failed — no valid traffic class. Packet dropped." << std::endl;
        return false;
    }
}

template <typename Packet>
Ptr<Packet> SPQ<Packet>::DoDequeue(){
    cout << "[SPQ] DoDequeue() called" << endl;
    return DiffServ<Packet>::Dequeue();
}

template<typename Packet>
Ptr<Packet> SPQ<Packet>::Schedule(){
    cout << "[SPQ] Schedule() called" << endl;
    for(TrafficClass *tc : this->q_class){
        if(!tc->isEmpty()){
            cout << "[SPQ] Dequeued from traffic class priority: " << tc->getPriorityLvl() << endl;
            return tc->Dequeue();
        }
    }
    return nullptr;
}

template<typename Packet>
uint32_t SPQ<Packet>::Classify(Ptr<Packet> packet){
    cout << "[SPQ]Classifying Packet" << endl;
    const uint32_t INVALID_CLASS_ID = std::numeric_limits<uint32_t>::max();
    
    for(uint32_t traffic_id = 0; traffic_id < this->q_class.size(); traffic_id += 1){
        if(this->q_class[traffic_id]->match(packet)){
            return traffic_id;
        }
    }

    cout << "Classifying Default... " << endl;
    for(uint32_t traffic_default = 0; traffic_default < this->q_class.size(); traffic_default += 1){
        if(this->q_class[traffic_default]->isDefaultCheck()){
            cout << "Classifying at default traffic calss with priority: " << traffic_default << endl;
            return traffic_default;
        }
    }

    return INVALID_CLASS_ID;
}


template<typename Packet>
void SPQ<Packet>::CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector){
    this->q_class = set_traffic_vector;
}

//template<typename Packet>
void insertFilter(TrafficClass* tc, Filter *filter){

    tc->addFilter(filter);

}

void insertTrafficClass(vector<TrafficClass*>& traffic_class_vector_local, TrafficClass* tc){
    auto it = traffic_class_vector_local.begin();
    cout << "Priority to insert: " << tc->getPriorityLvl() << endl;
    while (it != traffic_class_vector_local.end() && (*it)->getPriorityLvl() > tc->getPriorityLvl()) {
        size_t index = std::distance(traffic_class_vector_local.begin(), it);
        cout << "To insert index: " << index << endl;
        ++it;
    }
    traffic_class_vector_local.insert(it, tc);
}

template<typename Packet>
void SPQ<Packet>::CheckQueue(){
    for(TrafficClass* it_traffic: this->q_class){
        cout << "Iterate priority: " << it_traffic->getPriorityLvl() << ", isDefault? " << it_traffic->isDefaultCheck() << endl;
    }
}

template<typename Packet>
vector<TrafficClass*> SPQ<Packet>::getTrafficVector(){
    return this->q_class;
}

template<typename Packet>
bool SPQ<Packet>::testEnqueue(Ptr<Packet> packet){
    return this->DoEnqueue(packet);
}

template<typename Packet>
Ptr<Packet> SPQ<Packet>::testDequeue(){
    return this->DoDequeue();
}

string Trim(const string& s){
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    
    if(start == std::string::npos){
        return "";
    }
    else{
        size_t length = end - start + 1;
        //cout << "Trimmed sub string: " << s.substr(start, length) << endl;
        return s.substr(start, length);
    }

    cout << "Trimmed failed" << endl;
    return "";
}

void set_config(SPQConfig *cfg, string current_line, vector<map<string, string>> *raw_filters, map<string, string> &current_filter){
    if(current_line.empty() || current_line[0] == '#'){
        //cout << "Ignore # or empty line" << endl;
        return;
    }
    if(current_line.find("queueId:") == 0){
        cfg->class_id = stoi(current_line.substr(8));
        cout << "class id: " << cfg->class_id << endl;
    }
    else if(current_line.find("maxPackets") == 0){
        cfg->max_packets = stoi(current_line.substr(11));
        cout << "Max packets: " << cfg->max_packets << endl;
    }
    else if(current_line.find("priorityLevel:") == 0){
        cfg->priority_level = stoi(current_line.substr(14));
        cout << "Priority level: " << cfg->priority_level << endl;
    }
    else if(current_line.find("isDefault") == 0){
        cfg->isDefault = (current_line.substr(11) == "true");
        cout << "Is default: " << cfg->isDefault << endl;
    }
    else if (current_line.find("Filter:") == 0) {
        if (!current_filter.empty()) {
            raw_filters->push_back(current_filter);
            current_filter.clear();
        }
        istringstream iss(current_line);
        string key, value;
        iss >> key >> value;
        current_filter[key] = value;
        for (size_t i = 0; i < raw_filters->size(); ++i) {
            cout << "Filter " << i << ":" << endl;
            for (const auto& pair : (*raw_filters)[i]) {
                cout << "  " << pair.first << ": " << pair.second << endl;
            }
        }
    }
    
}

FilterElement* set_element(string key, string value){
    key.pop_back();
    cout << "key: " << key << endl;
    cout << "value: " << value << endl;

    if(Trim(key) == "DestPortNumber"){
        cout << "Dest Port value: " << static_cast<uint32_t>(stoul(value)) << endl;
        return new DestPortNumber(static_cast<uint32_t>(stoul(value)));
        
    }

    if(key == "ProtocolNumber"){
        return new ProtocolNumber(static_cast<uint32_t>(stoul(value)));
        
    }

    if(key == "DestIPAddress"){
        return new DestIPAddress(Ipv4Address(value.c_str()));
        
    }

    if(key == "SrcIPAddress"){
        return new SrcIPAddress(Ipv4Address(value.c_str()));
    }

    cout << "Error parsing to actual element" << endl;
    
    return nullptr;
}

void ReceivePacketCallback(Ptr<Socket> socket) {
    Ptr<Packet> packet;
    while ((packet = socket->Recv())) {
        cout << "Packet received: " << packet->GetSize() << " bytes" << endl;
    }
}

int main(int argc, char* argv[]){
    cout << "Running SPQ..." << endl;

    LogComponentEnable("UdpApplication", LOG_LEVEL_INFO);


    Ptr<SPQ<Packet>> spq = CreateObject<SPQ<Packet>>();
    vector<TrafficClass*> traffic_vectors;

    //./ns3 run "test_spq_config --spq=scratch/QOS-Differentiated-Service/spq_config" 
    //Config file path
    string file_name;

    CommandLine cmd(__FILE__);
    cmd.AddValue("spq", "file name", file_name);
    cmd.Parse(argc, argv);

    //Read config
    ifstream configFile(file_name);

    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << file_name << std::endl;
        return 1;  // or handle error
    }

    cout << "File name: " << file_name << endl;

    vector<SPQConfig> spq_config_vector;
    vector<string> block;
    string line;

    //Parse config
    while(getline(configFile, line)){
        string trimmed = Trim(line);
        //cout << trimmed << endl;

        if(trimmed == "---"){
            //cout << "***End of Traffic, parsing current traffic***" << endl;

            SPQConfig current_traffic_config;
            vector<map<string, string>> raw_filters;
            map<string, string> current_filter;
            bool inFilter = false;

            for(string block_line: block){
                //cout << "***Processing line: " << block_line << endl;

                if(block_line.empty() || block_line[0] == '#'){
                    //cout << "Ignore # or empty line" << endl;
                    continue;
                }
                if(block_line.find("queueId:") == 0){
                    current_traffic_config.class_id = stoi(block_line.substr(8));
                    //cout << "class id: " << current_traffic_config.class_id << endl;
                }
                else if(block_line.find("maxPackets") == 0){
                    current_traffic_config.max_packets = stoi(block_line.substr(11));
                    //cout << "Max packets: " << current_traffic_config.max_packets << endl;
                }
                else if(block_line.find("priorityLevel:") == 0){
                    current_traffic_config.priority_level = stoi(block_line.substr(14));
                    //cout << "Priority level: " << current_traffic_config.priority_level << endl;
                }
                else if(block_line.find("isDefault") == 0){
                    current_traffic_config.isDefault = (block_line.substr(11) == "true");
                    //cout << "Is default: " << current_traffic_config.isDefault << endl;
                }
                else if (block_line.find("Filter:") == 0) {
                    //cout << "Filter parse: " << endl;
                    if (!current_filter.empty()) {
                        raw_filters.push_back(current_filter);
                        current_filter.clear();
                    }

                    inFilter = true;
                    
                }
                else if(inFilter){

                    if(block_line.find("END_FILTER") == 0){
                        inFilter = false;
                        if (!current_filter.empty()) {
                            raw_filters.push_back(current_filter);
                            // cout << "Filter " << raw_filters.size() - 1 << ":" << endl;
                            // for (const auto& pair : current_filter) {
                            //     cout << "  " << pair.first  << pair.second << endl;
                            // }
                            current_filter.clear();
                        }
                        continue;
                    }
                    
                    istringstream iss(block_line);
                    string key, value;
                    iss >> key >> value;
                    current_filter[key] = value;
                    // for (size_t i = 0; i < raw_filters.size(); ++i) {
                    //     cout << "Filter " << i << ":" << endl;
                    //     for (const auto& pair : raw_filters[i]) {
                    //         cout << "  " << pair.first << ": " << pair.second << endl;
                    //     }
                    // }
                             
                }
            }

            if(!current_filter.empty()){
                raw_filters.push_back(current_filter);
            }
            
            current_traffic_config.raw_filter_vector = raw_filters;

            spq_config_vector.push_back(current_traffic_config);
            block.clear();

            //cout << "******" << endl;
        }
        else{
            
            block.push_back(trimmed);
        }
    }

    //Deal with parsed config struct
    for(SPQConfig spq_config: spq_config_vector){
        TrafficClass *tc = new TrafficClass();

        cout << "Class id: " << spq_config.class_id << endl;
        cout << "Max packets: " << spq_config.max_packets << endl;
        tc->setMaxPackets(spq_config.max_packets);

        cout << "Priority level: " << spq_config.priority_level << endl;
        tc->setPriorityLvl(spq_config.priority_level);

        cout << "Is Default: " << spq_config.isDefault << endl;
        tc->setDefault(spq_config.isDefault);

        for (size_t i = 0; i < spq_config.raw_filter_vector.size(); ++i) {
            cout << "Filter " << i << ":" << endl;
            Filter *filter = new Filter();
            for (const auto& pair : spq_config.raw_filter_vector[i]) {
                
                cout << "  " << pair.first << pair.second << endl;
                const string& key = pair.first;
                const string& value = pair.second;
                
                FilterElement *filter_element = set_element(key, value);

                filter->addElement(filter_element);
                
            }

            tc->addFilter(filter);
        }

        insertTrafficClass(traffic_vectors, tc);
        //traffic_vectors.push_back(tc);
        cout << "---End of Traffic---" << endl;

        
    }

    spq->CreateTrafficClassesVector(traffic_vectors);

    spq->CheckQueue();

    // === ns-3 Topology Setup ===
    NodeContainer nodes;

    nodes.Create(3);

    NodeContainer n0r = NodeContainer(nodes.Get(0), nodes.Get(1));
    NodeContainer n1r = NodeContainer(nodes.Get(1), nodes.Get(2));

    PointToPointHelper p2p_1, p2p_2;
    p2p_1.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    p2p_1.SetChannelAttribute("Delay", StringValue("2ms"));

    p2p_2.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p_2.SetChannelAttribute("Delay", StringValue("2ms"));

    InternetStackHelper internet;
    internet.Install(nodes);

    NetDeviceContainer p2p_device1, p2p_device2;
    p2p_device1 = p2p_1.Install(nodes.Get(0), nodes.Get(1));
    p2p_device2 = p2p_2.Install(nodes.Get(1), nodes.Get(2));

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.255");
    Ipv4InterfaceContainer interfaces1 = address.Assign(p2p_device1);

    address.SetBase("10.1.2.2", "255.255.255.255");
    Ipv4InterfaceContainer interfaces2 = address.Assign(p2p_device2);

    //Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    //Set Custom SPQ to router
    Ptr<PointToPointNetDevice> router_device = p2p_device2.Get(0)->GetObject<PointToPointNetDevice>();
    if(router_device){
        router_device->SetQueue(spq);
    }
    else{
        cerr << "Failed to apply custom SPQ to router." << endl;
        return 1;
    }
    
    //Set UDP Server on node 2
    uint16_t port1 = 80, port2 = 70;
    UdpEchoServerHelper server1(port1);
    ApplicationContainer apps1 = server1.Install(nodes.Get(2));
    apps1.Start(Seconds(1.0));
    apps1.Stop(Seconds(120.0));

    UdpEchoServerHelper server2(port2);
    ApplicationContainer apps2 = server2.Install(nodes.Get(2));
    apps2.Start(Seconds(1.0));
    apps2.Stop(Seconds(120.0));

    //High Priority App
    UdpEchoClientHelper client1(interfaces2.GetAddress(1), port1);
    client1.SetAttribute("MaxPackets", UintegerValue(10000));
    client1.SetAttribute("Interval", TimeValue(Seconds(0.001)));
    client1.SetAttribute("PacketSize", UintegerValue(512));
    
    ApplicationContainer clientApps1 = client1.Install(nodes.Get(0));
    clientApps1.Start(Seconds(4.0));
    clientApps1.Stop(Seconds(120.0));

    //Low Priority App
    UdpEchoClientHelper client2(interfaces2.GetAddress(1), port2);
    client2.SetAttribute("MaxPackets", UintegerValue(10000));
    client2.SetAttribute("Interval", TimeValue(Seconds(0.001)));
    client2.SetAttribute("PacketSize", UintegerValue(512));
    
    ApplicationContainer clientApps2 = client2.Install(nodes.Get(0));
    clientApps2.Start(Seconds(1.0));
    clientApps2.Stop(Seconds(120.0));
    


    // Enable PCAP capture
    // p2p_1.EnablePcap("router-incoming", p2p_device1.Get(1), true);  // Promiscuous mode
    // p2p_2.EnablePcap("router-outgoing", p2p_device2.Get(0), true);
    // p2p_1.EnablePcapAll("lab3-router-incoming");
    // p2p_2.EnablePcapAll("lab3-router-outgoing");

    // Capture incoming packets on the router (incoming from Node 0 to Node 1)
    p2p_1.EnablePcap("spq-router-incoming", p2p_device1.Get(0), false); 

    // Capture outgoing packets on the router (incoming from Node 0 to Node 1)
    p2p_1.EnablePcap("spq-router-outgoing", p2p_device2.Get(0), false); 


    cout << "Running Simulation" << endl;
    // Run simulation
    //Simulator::Stop(Seconds(12.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;



}

// NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));
//     NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));

//     InternetStackHelper stack;
//     stack.Install(nodes);

//     Ipv4AddressHelper address;
//     address.SetBase("8.8.8.8", "255.255.255.255");
//     Ipv4InterfaceContainer interfaces1 = address.Assign(devices1);

//     address.SetBase("10.1.2.0", "255.255.255.255");
//     Ipv4InterfaceContainer interfaces2 = address.Assign(devices2);

//     // Attach SPQ to Router Node (Node 1)
//     Ptr<PointToPointNetDevice> routerDevice = devices1.Get(1)->GetObject<PointToPointNetDevice>();
//     if (routerDevice == nullptr) {
//         std::cerr << "Failed to get PointToPointNetDevice on router." << std::endl;
//         return 1;
//     }

//     Ptr<Queue<Packet>> queue = spq;
//     routerDevice->SetQueue(queue);

//     // Set up UDP sender applications
//     Ptr<Socket> udpSocketA = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
//     Ptr<Socket> udpSocketB = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());

//     Address destAddress = InetSocketAddress(interfaces2.GetAddress(1), 8080);

//     // Application B (Low Priority) starts first
//     Ptr<UdpApplication> appB = CreateObject<UdpApplication>();
//     NS_OBJECT_ENSURE_REGISTERED(UdpApplication);
//     appB->Setup(udpSocketB, destAddress, 1000, 100, MilliSeconds(10));
//     nodes.Get(0)->AddApplication(appB);
//     appB->SetStartTime(Seconds(1.0));
//     appB->SetStopTime(Seconds(10.0));

//     // Application A (High Priority) starts later
//     Ptr<UdpApplication> appA = CreateObject<UdpApplication>();
//     appA->Setup(udpSocketA, destAddress, 1000, 100, MilliSeconds(10));
//     nodes.Get(0)->AddApplication(appA);
//     appA->SetStartTime(Seconds(3.0));
//     appA->SetStopTime(Seconds(10.0));

//     // Receiver (Server)
//     Ptr<Socket> sink = Socket::CreateSocket(nodes.Get(2), UdpSocketFactory::GetTypeId());
//     sink->Bind(InetSocketAddress(Ipv4Address::GetAny(), 8080));
//     sink->Listen();
//     sink->SetRecvCallback(MakeCallback(&ReceivePacketCallback));

//     PointToPointHelper pointToPoint0;
//     pointToPoint0.EnablePcapAll("udp");

//     cout << "Running NS3 Topology..." << endl;
//     Simulator::Run();
//     Simulator::Destroy();


// //Setup UDP Application for high priority
    // Ptr<Socket> udp_socket_1 = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
    // Address dest1 = InetSocketAddress(interfaces2.GetAddress(1), 80);
    // Ptr<UdpApplication> app1 = CreateObject<UdpApplication>();
    // app1->Setup(udp_socket_1, dest1, 512, 10000, MilliSeconds(1));
    // nodes.Get(0)->AddApplication(app1);
    // app1->SetStartTime(Seconds(4.0));
    // app1->SetStopTime(Seconds(10.0));

    // //Set UDP Application for low priority
    // Ptr<Socket> udp_socket_2 = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
    // Address dest2 = InetSocketAddress(interfaces2.GetAddress(1), 70);
    // Ptr<UdpApplication> app2 = CreateObject<UdpApplication>();
    // app2->Setup(udp_socket_2, dest2, 512, 10000, MilliSeconds(1));
    // nodes.Get(0)->AddApplication(app2);
    // app2->SetStartTime(Seconds(1.0));
    // app2->SetStopTime(Seconds(10.0));

        // Ptr<Packet> testPacket = Create<Packet>(500); // 500 bytes test packet
    // bool enqueueResult = spq->DoEnqueue(testPacket);
    // cout << enqueueResult << endl;
    //NS_LOG_INFO("Test Packet Enqueue Result: " << (enqueueResult ? "Success" : "Failure"));

        //Create Packet 1
    // --- Construct packet: UDP port 80 ---
    // Ptr<Packet> pkt = Create<Packet>(100);
    // Ipv4Header ipHeader;
    // ipHeader.SetProtocol(17); // UDP
    // ipHeader.SetDestination("0.0.0.0");
 
    // UdpHeader udpHeader;
    // udpHeader.SetDestinationPort(80);
 
    // pkt->AddHeader(udpHeader);
    // pkt->AddHeader(ipHeader);

    // //Create packet 2 
    // Ptr<Packet> pkt2 = Create<Packet>(100);
    // Ipv4Header ipHeader2;
    // ipHeader2.SetProtocol(17); // UDP
    // ipHeader2.SetDestination("0.0.0.1");
 
    // UdpHeader udpHeader2;
    // udpHeader2.SetDestinationPort(70);
 
    // pkt2->AddHeader(udpHeader2);
    // pkt2->AddHeader(ipHeader2);

    // //Create packet 3
    // Ptr<Packet> pkt3 = Create<Packet>(100);
    // Ipv4Header ipHeader3;
    // ipHeader3.SetProtocol(17); // UDP
    // ipHeader3.SetDestination("0.0.0.2");
 
    // UdpHeader udpHeader3;
    // udpHeader3.SetDestinationPort(80);
 
    // pkt3->AddHeader(udpHeader3);
    // pkt3->AddHeader(ipHeader3);


    // //Create packet 4
    // Ptr<Packet> pkt4 = Create<Packet>(100);
    // Ipv4Header ipHeader4;
    // ipHeader4.SetProtocol(17); // UDP
    // ipHeader4.SetDestination("10.0.0.1");
 
    // UdpHeader udpHeader4;
    // udpHeader4.SetDestinationPort(80);
 
    // pkt4->AddHeader(udpHeader4);
    // pkt4->AddHeader(ipHeader4);

    // //Classify packet 1
    // uint32_t classId = spq->Classify(pkt);
    // cout << "Classified into class: " << classId << endl;

    // if (classId >= spq->getTrafficVector().size()) {
    //     cerr << "Classification failed!" << endl;
    //     return 1;
    // }
    
    // cout << " - * - * - * - * - * - * - * - * - * - * - * - * - * - *" << endl;

    // // --- Enqueue packet 1 into correct class ---
    // bool success = spq->DoEnqueue(pkt);
    // cout << "Enqueue success: " << (success ? "true" : "false") << endl;

    // cout << "Current size for class " << classId << ": " << spq->getTrafficVector()[classId]->getQueueSize() << endl;

    // //Classify packet 2
    // uint32_t classId2 = spq->Classify(pkt2);
    // cout << "Classified into class: " << classId2 << endl;

    // if (classId2 >= spq->getTrafficVector().size()) {
    //     cerr << "Classification failed!" << endl;
    //     return 1;
    // }

    // // --- Enqueue packet 2 into correct class ---
    // bool success2 = spq->DoEnqueue(pkt2); //Enqueue direct call or under Classify()?
    // cout << "Enqueue success: " << (success2 ? "true" : "false") << endl;

    // cout << "Current size for class " << classId2 << ": " << spq->getTrafficVector()[classId2]->getQueueSize() << endl;

    // //Classify packet 3
    // uint32_t classId3 = spq->Classify(pkt3);
    // cout << "Classified into class: " << classId3 << endl;

    // if (classId3 >= spq->getTrafficVector().size()) {
    //     cerr << "Classification failed!" << endl;
    //     return 1;
    // }

    // // --- Enqueue packet 3 into correct class ---
    // bool success3 = spq->DoEnqueue(pkt3); //Enqueue direct call or under Classify()?
    // cout << "Enqueue success: " << (success3 ? "true" : "false") << endl;

    // cout << "Current size for class " << classId3 << ": " << spq->getTrafficVector()[classId3]->getQueueSize() << endl;

    // //Classify packet 4
    // uint32_t classId4 = spq->Classify(pkt4);
    // cout << "Classified into class: " << classId4 << endl;

    // if (classId4 >= spq->getTrafficVector().size()) {
    //     cerr << "Classification failed!" << endl;
    //     return 1;
    // }

    // // --- Enqueue packet 4 into correct class ---
    // bool success4 = spq->DoEnqueue(pkt4); //Enqueue direct call or under Classify()?
    // cout << "Enqueue success: " << (success4 ? "true" : "false") << endl;

    // cout << "Current size for class " << classId4 << ": " << spq->getTrafficVector()[classId3]->getQueueSize() << endl;
    

    // //Test Schedule
    // for(int pkt_count = 4; pkt_count > 0; pkt_count -= 1){
    //     cout << "-------------------" << endl;
    //     Ptr<Packet> scheduledPkt = spq->DoDequeue();

    //     Ipv4Header ipHeaderExtract;
    //     UdpHeader udpHeaderExtract;

    

    //     if (scheduledPkt) {
    //         scheduledPkt->RemoveHeader(ipHeaderExtract);
    //         scheduledPkt->RemoveHeader(udpHeaderExtract);

    //         cout << "Scheduled a packet successfully!" << endl;
    //         cout << "Destination IP: " << ipHeaderExtract.GetDestination() << endl;
    //         cout << "Protocol: " << (uint16_t)ipHeaderExtract.GetProtocol() << endl;
    //         cout << "Destination port: " << udpHeaderExtract.GetDestinationPort() << endl;
    //     } else {
    //         cout << "Schedule returned nullptr — all queues empty." << endl;
    //     }
    // }