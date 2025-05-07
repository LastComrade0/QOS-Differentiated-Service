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
#include "udp_application.h"
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

template<typename Packet>
Ptr<Packet> SPQ<Packet>::Schedule(){
    for(TrafficClass *tc : this->q_class){
        if(!tc->isEmpty()){
            return tc->Dequeue();
        }
    }
    return nullptr;
}

template<typename Packet>
uint32_t SPQ<Packet>::Classify(Ptr<Packet> packet){
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

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1 = address.Assign(devices1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces2 = address.Assign(devices2);

    // Attach SPQ to Router Node (Node 1)
    Ptr<PointToPointNetDevice> routerDevice = devices2.Get(0)->GetObject<PointToPointNetDevice>();
    if (routerDevice == nullptr) {
        std::cerr << "Failed to get PointToPointNetDevice on router." << std::endl;
        return 1;
    }

    routerDevice->SetQueue(spq);

    // Set up UDP sender applications
    Ptr<Socket> udpSocketA = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
    Ptr<Socket> udpSocketB = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());

    Address destAddress = InetSocketAddress(interfaces2.GetAddress(1), 8080);

    // Application B (Low Priority) starts first
    Ptr<UdpApplication> appB = CreateObject<UdpApplication>();
    NS_OBJECT_ENSURE_REGISTERED(UdpApplication);
    appB->Setup(udpSocketB, destAddress, 1000, 100, MilliSeconds(10));
    nodes.Get(0)->AddApplication(appB);
    appB->SetStartTime(Seconds(1.0));
    appB->SetStopTime(Seconds(10.0));

    // Application A (High Priority) starts later
    Ptr<UdpApplication> appA = CreateObject<UdpApplication>();
    appA->Setup(udpSocketA, destAddress, 1000, 100, MilliSeconds(10));
    nodes.Get(0)->AddApplication(appA);
    appA->SetStartTime(Seconds(3.0));
    appA->SetStopTime(Seconds(10.0));

    // Receiver (Server)
    Ptr<Socket> sink = Socket::CreateSocket(nodes.Get(2), UdpSocketFactory::GetTypeId());
    sink->Bind(InetSocketAddress(Ipv4Address::GetAny(), 8080));
    sink->Listen();
    sink->SetRecvCallback(MakeCallback(&ReceivePacketCallback));

    Simulator::Run();
    Simulator::Destroy();

    return 0;



}