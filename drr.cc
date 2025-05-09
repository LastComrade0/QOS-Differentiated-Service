#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

#include "drr.h"
#include <map>
#include <vector>
#include <queue>

#include <math.h>

using namespace ns3;
using namespace std;

template<typename Packet>
DRR<Packet>::DRR(){
    current_robin = 0;
}

template<typename Packet>
DRR<Packet>::~DRR(){}

template <typename Packet>
bool DRR<Packet>::DoEnqueue(Ptr<Packet> packet){
    cout << "[DRR] DoEnqueue() called" << endl;
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
Ptr<Packet> DRR<Packet>::DoDequeue(){
    cout << "[DRR] DoDequeue() called" << endl;
    return DiffServ<Packet>::Dequeue();
}

template<typename Packet>
Ptr<Packet> DRR<Packet>::Schedule() {
    uint32_t class_count = this->q_class.size();
    cout << "Class count: " << class_count << endl;
    
    if(getLocalRoundQueue()->empty()){
        for(size_t i = 0; i < this->q_class.size(); ++i){
            uint32_t class_index =  i;
            cout << "[Schedule]Current Class Index: " << class_index << endl;
            TrafficClass *current_class = this->q_class[class_index];
    
            // If the queue is empty, just skip this class
            if (current_class->getMQueue().empty()) {
                continue;
            }
    
            //Quantum + Deficit Counter
            current_class->setDeficitCounter(current_class->getDeficitCounter() + current_class->getQuantumSize());
            
    
            while(!current_class->getMQueue().empty() && (current_class->getDeficitCounter() > 0)){
                Ptr<Packet> current_packet = current_class->getMQueue().front();
                uint32_t current_packet_size = current_packet->GetSize();
                cout << "Current packet size" << current_packet_size << endl;
    
                if(current_packet_size <= current_class->getDeficitCounter()){
                    current_class->Dequeue();
                    current_class->setDeficitCounter(current_class->getDeficitCounter() - current_packet_size);
                    local_round_queue.push(current_packet);
                }
                else{
                    break;
                }
    
                
            }
    
            if (current_class->getMQueue().empty()) {
                current_class->setDeficitCounter(0);
            }
            
        }

        Ptr<Packet> popped_from_local_round = local_round_queue.front();
        local_round_queue.pop();
        return popped_from_local_round;

    }
    // If the local round queue has packets, return the first one
    if (!getLocalRoundQueue()->empty()) {
        Ptr<Packet> scheduledPacket = local_round_queue.front();
        local_round_queue.pop();
        return scheduledPacket;
    }

    cout << "No packets available to schedule." << endl;
    return nullptr;
}


template<typename Packet>
uint32_t DRR<Packet>::Classify(Ptr<Packet> packet){
    const uint32_t INVALID_CLASS_ID = std::numeric_limits<uint32_t>::max();
    
    for(uint32_t traffic_id = 0; traffic_id < this->q_class.size(); traffic_id += 1){
        if(this->q_class[traffic_id]->match(packet)){
            cout << "[Classify]Classifying at traffic class: " << traffic_id << endl;
            return traffic_id;
        }
    }

    cout << "Classifying Default... " << endl;
    for(uint32_t traffic_default = 0; traffic_default < this->q_class.size(); traffic_default += 1){
        if(this->q_class[traffic_default]->isDefaultCheck()){
            cout << "[Classify]Classifying at default traffic calss with priority: " << traffic_default << endl;
            return traffic_default;
        }
    }

    return INVALID_CLASS_ID;
}

template<typename Packet>
queue<Ptr<Packet>>* DRR<Packet>::getLocalRoundQueue(){
    return &local_round_queue;
}


template<typename Packet>
void DRR<Packet>::CreateTrafficClassesVector(vector<TrafficClass*> set_traffic_vector){
    active_traffic_vector = {0, 1, 2}; //Can improve as dynamic
    this->q_class = set_traffic_vector;
}

//template<typename Packet>
void insertFilter(TrafficClass* tc, Filter *filter){

    tc->addFilter(filter);

}

void insertTrafficClass(vector<TrafficClass*>& traffic_class_vector_local, TrafficClass* tc){
    traffic_class_vector_local.push_back(tc);
}

template<typename Packet>
void DRR<Packet>::CheckQueue(){
    for(TrafficClass* it_traffic: this->q_class){
        cout << "Iterate priority: " << it_traffic->getQueueSize() << ", isDefault? " << it_traffic->isDefaultCheck() << endl;
    }
}

template<typename Packet>
vector<TrafficClass*> DRR<Packet>::getTrafficVector(){
    return this->q_class;
}

template<typename Packet>
bool DRR<Packet>::testEnqueue(Ptr<Packet> packet){
    return this->DoEnqueue(packet);
}

template<typename Packet>
Ptr<Packet> DRR<Packet>::testDequeue(){
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

void set_config(DRRConfig *cfg, string current_line, vector<map<string, string>> *raw_filters, map<string, string> &current_filter){
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
    else if(current_line.find("quantumSize:") == 0){
        cfg->quantum_size = stoi(current_line.substr(14));
        cout << "Quantum size: " << cfg->quantum_size << endl;
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

    if(key == "DestPortNumber"){
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


int main(int argc, char* argv[]){
    cout << "Running DRR..." << endl;

    Ptr<DRR<Packet>> drr = CreateObject<DRR<Packet>>();
    vector<TrafficClass*> traffic_vectors;

    //./ns3 run "test_drr_config --drr=scratch/QOS-Differentiated-Service/drr_config" 
    //Config file path
    string file_name;

    CommandLine cmd(__FILE__);
    cmd.AddValue("drr", "file name", file_name);
    cmd.Parse(argc, argv);

    //Read config
    ifstream configFile(file_name);

    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << file_name << std::endl;
        return 1;  // or handle error
    }

    cout << "File name: " << file_name << endl;

    vector<DRRConfig> drr_config_vector;
    vector<string> block;
    string line;

    //Parse config
    while(getline(configFile, line)){
        string trimmed = Trim(line);
        //cout << trimmed << endl;

        if(trimmed == "---"){
            //cout << "***End of Traffic, parsing current traffic***" << endl;

            DRRConfig current_traffic_config;
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
                else if(block_line.find("quantumSize:") == 0){
                    current_traffic_config.quantum_size = stoi(block_line.substr(12));
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

            drr_config_vector.push_back(current_traffic_config);
            block.clear();

            //cout << "******" << endl;
        }
        else{
            
            block.push_back(trimmed);
        }
    }

    //Deal with parsed config struct
    for(DRRConfig drr_config: drr_config_vector){
        TrafficClass *tc = new TrafficClass();

        cout << "Class id: " << drr_config.class_id << endl;
        cout << "Max packets: " << drr_config.max_packets << endl;
        tc->setMaxPackets(drr_config.max_packets);

        cout << "Quantum Size: " << drr_config.quantum_size << endl;
        tc->setQuantumSize(drr_config.quantum_size);

        tc->setDeficitCounter(0);

        cout << "Is Default: " << drr_config.isDefault << endl;
        tc->setDefault(drr_config.isDefault);

        for (size_t i = 0; i < drr_config.raw_filter_vector.size(); ++i) {
            cout << "Filter " << i << ":" << endl;
            Filter *filter = new Filter();
            for (const auto& pair : drr_config.raw_filter_vector[i]) {
                
                cout << "  " << pair.first << pair.second << endl;
                const string& key = pair.first;
                const string& value = pair.second;
                
                FilterElement *filter_element = set_element(key, value);

                filter->addElement(filter_element);
                
            }

            tc->addFilter(filter);
        }

        traffic_vectors.push_back(tc);
        cout << "---End of Traffic---" << endl;

        
    }

    drr->CreateTrafficClassesVector(traffic_vectors);

    drr->CheckQueue();

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
            router_device->SetQueue(drr);
        }
        else{
            cerr << "Failed to apply custom SPQ to router." << endl;
            return 1;
        }
        
        //Set UDP Server on node 2
        uint16_t port1 = 80, port2 = 70, port3 = 10;
        UdpEchoServerHelper server1(port1);
        ApplicationContainer apps1 = server1.Install(nodes.Get(2));
        apps1.Start(Seconds(1.0));
        apps1.Stop(Seconds(120.0));
    
        UdpEchoServerHelper server2(port2);
        ApplicationContainer apps2 = server2.Install(nodes.Get(2));
        apps2.Start(Seconds(1.0));
        apps2.Stop(Seconds(120.0));
    
        //Client App1
        UdpEchoClientHelper client1(interfaces2.GetAddress(1), port1);
        client1.SetAttribute("MaxPackets", UintegerValue(10000));
        client1.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        client1.SetAttribute("PacketSize", UintegerValue(512));
        
        ApplicationContainer clientApps1 = client1.Install(nodes.Get(0));
        clientApps1.Start(Seconds(1.0));
        clientApps1.Stop(Seconds(120.0));
    
        //Client App2
        UdpEchoClientHelper client2(interfaces2.GetAddress(1), port2);
        client2.SetAttribute("MaxPackets", UintegerValue(10000));
        client2.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        client2.SetAttribute("PacketSize", UintegerValue(512));
        
        ApplicationContainer clientApps2 = client2.Install(nodes.Get(0));
        clientApps2.Start(Seconds(1.0));
        clientApps2.Stop(Seconds(120.0));

        //Client App3
        UdpEchoClientHelper client3(interfaces2.GetAddress(1), port3);
        client3.SetAttribute("MaxPackets", UintegerValue(10000));
        client3.SetAttribute("Interval", TimeValue(Seconds(0.001)));
        client3.SetAttribute("PacketSize", UintegerValue(512));
        
        ApplicationContainer clientApps3 = client3.Install(nodes.Get(0));
        clientApps3.Start(Seconds(1.0));
        clientApps3.Stop(Seconds(120.0));
        
    
    
        // Enable PCAP capture
        // p2p_1.EnablePcap("router-incoming", p2p_device1.Get(1), true);  // Promiscuous mode
        // p2p_2.EnablePcap("router-outgoing", p2p_device2.Get(0), true);
        // p2p_1.EnablePcapAll("lab3-router-incoming");
        // p2p_2.EnablePcapAll("lab3-router-outgoing");
    
        // Capture incoming packets on the router (incoming from Node 0 to Node 1)
        p2p_1.EnablePcap("drr-router-incoming", p2p_device1.Get(0), false); 
    
        // Capture outgoing packets on the router (incoming from Node 0 to Node 1)
        p2p_1.EnablePcap("drr-router-outgoing", p2p_device2.Get(0), false); 
    
    
        cout << "Running Simulation" << endl;
        // Run simulation
        //Simulator::Stop(Seconds(12.0));
        Simulator::Run();
        Simulator::Destroy();
    
        return 0;


}


/*Alternate DRR Schedule method*/
// template<typename Packet>
// Ptr<Packet> DRR<Packet>::Schedule(){
//     uint32_t class_count = this->q_class.size();

//     cout << "Class count: " << class_count << endl;

//     for(uint32_t i = 0; i < class_count; i += 1){
//         current_robin = (current_robin + 1) % class_count;
//         cout << "Current robin: " << current_robin << endl;

//         TrafficClass *tc = this->q_class[current_robin];
//         cout << "Current Traffic Class: " << endl;

//         if(!tc->isEmpty()){
//             tc->setDeficitCounter(tc->getDeficitCounter() + tc->getQuantumSize());
            

//             while(!tc->isEmpty()){
//                 cout << "[Schedule] Checking class " << current_robin << ", queue size: " << tc->getQueueSize() << endl;
//                 Ptr<Packet> packet = tc->peek();

//                 cout << "[Schedule] Updated deficitCounter: " << tc->getDeficitCounter() << ", quantum: " << tc->getQuantumSize() << endl;

//                 if (!packet) {
//                     cout << "[Schedule] NULL packet from peek(), breaking inner loop" << endl;
//                     break; // nothing to process
//                 }

//                 uint32_t size = packet->GetSize();
//                 cout << "[Schedule] Packet size: " << size << ", deficit: " << tc->getDeficitCounter() << endl;

//                 if(size <= tc->getDeficitCounter()){
//                     tc->setDeficitCounter(tc->getDeficitCounter() - size);
//                     cout << "[Schedule] Sending packet..." << endl;
//                     return tc->Dequeue();
//                 }
//                 else{
//                     cout << "[Schedule] Deficit too small, skipping..." << endl;
//                     break;
//                 }
//             }


//             if(tc->isEmpty()){
//                 tc->setDeficitCounter(0);
//             }
//         }
//     }
//     return nullptr;
// }


// //Create Packet 1
//     // --- Construct packet: UDP port 80 ---
//     Ptr<Packet> pkt = Create<Packet>(100);
//     Ipv4Header ipHeader;
//     ipHeader.SetProtocol(17); // UDP
//     ipHeader.SetDestination("0.0.0.0");
 
//     UdpHeader udpHeader;
//     udpHeader.SetDestinationPort(80);
 
//     pkt->AddHeader(udpHeader);
//     pkt->AddHeader(ipHeader);

//     //Create packet 2 
//     Ptr<Packet> pkt2 = Create<Packet>(100);
//     Ipv4Header ipHeader2;
//     ipHeader2.SetProtocol(17); // UDP
//     ipHeader2.SetDestination("0.0.0.1");
 
//     UdpHeader udpHeader2;
//     udpHeader2.SetDestinationPort(10);
 
//     pkt2->AddHeader(udpHeader2);
//     pkt2->AddHeader(ipHeader2);

//     //Create packet3
//     Ptr<Packet> pkt3 = Create<Packet>(200);
//     Ipv4Header ipHeader3;
//     ipHeader3.SetProtocol(17); // UDP
//     ipHeader3.SetDestination("10.0.0.1");

//     UdpHeader udpHeader3;
//     udpHeader3.SetDestinationPort(70);
 
//     pkt3->AddHeader(udpHeader3);
//     pkt3->AddHeader(ipHeader3);

//     //Classify packet 1
//     uint32_t classId = drr->Classify(pkt);
//     cout << "Classified into class: " << classId << endl;

//     if (classId >= drr->getTrafficVector().size()) {
//         cerr << "Classification failed!" << endl;
//         return 1;
//     }
    
//     cout << " - * - * - * - * - * - * - * - * - * - * - * - * - * - *" << endl;

//     // --- Enqueue packet 1 into correct class ---
//     bool success = drr->DoEnqueue(pkt);
//     cout << "Enqueue success: " << (success ? "true" : "false") << endl;

//     cout << "Current size for class " << classId << ": " << drr->getTrafficVector()[classId]->getQueueSize() << endl;

//     //Classify packet 2
//     uint32_t classId2 = drr->Classify(pkt2);
//     cout << "Classified into class: " << classId2 << endl;

//     if (classId2 >= drr->getTrafficVector().size()) {
//         cerr << "Classification failed!" << endl;
//         return 1;
//     }

//     // --- Enqueue packet 2 into correct class ---
//     bool success2 = drr->DoEnqueue(pkt2); //Enqueue direct call or under Classify()?
//     cout << "Enqueue success: " << (success2 ? "true" : "false") << endl;

//     cout << "Current size for class " << classId2 << ": " << drr->getTrafficVector()[classId2]->getQueueSize() << endl;

//     //Classify packet 3
//     uint32_t classId3 = drr->Classify(pkt3);
//     cout << "Classified into class: " << classId3 << endl;

//     if (classId3 >= drr->getTrafficVector().size()) {
//         cerr << "Classification failed!" << endl;
//         return 1;
//     }

//     // --- Enqueue packet 2 into correct class ---
//     bool success3 = drr->DoEnqueue(pkt3); //Enqueue direct call or under Classify()?
//     cout << "Enqueue success: " << (success3 ? "true" : "false") << endl;

//     cout << "Current size for class " << classId3 << ": " << drr->getTrafficVector()[classId3]->getQueueSize() << endl;

//     //Test Schedule

//     for(int pkt_count = 3; pkt_count > 0; pkt_count -= 1){
//         cout << "-------------------" << endl;
//         Ptr<Packet> scheduledPkt = drr->DoDequeue();

//         Ipv4Header ipHeaderExtract;
//         UdpHeader udpHeaderExtract;

    

//         if (scheduledPkt) {
//             scheduledPkt->RemoveHeader(ipHeaderExtract);
//             scheduledPkt->RemoveHeader(udpHeaderExtract);

//             cout << "Scheduled a packet successfully!" << endl;
//             cout << "Destination IP: " << ipHeaderExtract.GetDestination() << endl;
//             cout << "Protocol: " << (uint16_t)ipHeaderExtract.GetProtocol() << endl;
//             cout << "Destination port: " << udpHeaderExtract.GetDestinationPort() << endl;
//         } else {
//             cout << "Schedule returned nullptr — all queues empty." << endl;
//         }
//     }