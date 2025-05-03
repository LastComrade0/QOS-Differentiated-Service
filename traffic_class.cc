#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "traffic_class.h"
#include <iostream>
#include <vector>
#include <memory>
#include <math.h>

using namespace std;
using namespace ns3;

//General
TrafficClass::TrafficClass(){}

//DRR
TrafficClass::TrafficClass(uint32_t max_packets, uint32_t quantum_size, uint32_t *deficit_counter, bool isDefault,
                            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
                            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number) : maxPackets(max_packets), quantum_size(quantum_size),
                                                                                                                     deficit_counter(deficit_counter), isDefault(isDefault) {
                                    
                                    //Create new filter elements for DRR
                                    SrcIPAddress *src_address_drr_element = new SrcIPAddress(src_ip);
                                    SrcMask *src_mask_drr_element = new SrcMask(src_ip, src_mask);
                                    SrcPortNumber *src_port_drr_element = new SrcPortNumber(src_port);

                                    DestIPAddress *dest_address_drr_element = new DestIPAddress(dest_ip);
                                    DestMask *dest_mask_drr_element = new DestMask(dest_ip, dest_mask);
                                    DestPortNumber *dest_port_drr_element = new DestPortNumber(dest_port);

                                    ProtocolNumber *protocol_number_drr_element = new ProtocolNumber(protocol_number);

                                    Filter *filter_drr = new Filter(src_address_drr_element, src_mask_drr_element, src_port_drr_element, 
                                                                    dest_address_drr_element, dest_mask_drr_element, dest_port_drr_element, protocol_number_drr_element);

                                    vector<Filter*> filter_vector;

                                    filter_vector.push_back(filter_drr);

                                    this->filter_vector = filter_vector;
}

//SPQ
TrafficClass::TrafficClass(uint32_t max_packets, double_t weight, uint32_t priority_level, bool isDefault,
                            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
                            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number) : maxPackets(max_packets), weight(weight),
                                                                                                                     priority_level(priority_level), isDefault(isDefault) {
                                    SrcIPAddress *src_address_spq_element = new SrcIPAddress(src_ip);
                                    SrcMask *src_mask_spq_element = new SrcMask(src_ip, src_mask);
                                    SrcPortNumber *src_port_spq_element = new SrcPortNumber(src_port);

                                    DestIPAddress *dest_address_spq_element = new DestIPAddress(dest_ip);
                                    DestMask *dest_mask_spq_element = new DestMask(dest_ip, dest_mask);
                                    DestPortNumber *dest_port_spq_element = new DestPortNumber(dest_port);

                                    ProtocolNumber *protocol_number_spq_element = new ProtocolNumber(protocol_number);

                                    Filter *filter_spq = new Filter(src_address_spq_element, src_mask_spq_element, src_port_spq_element, 
                                                                    dest_address_spq_element, dest_mask_spq_element, dest_port_spq_element, protocol_number_spq_element);

                                    vector<Filter*> filter_vector;

                                    filter_vector.push_back(filter_spq);

                                    this->filter_vector = filter_vector;
}

TrafficClass::~TrafficClass(){}

        
bool TrafficClass::Enqueue(Ptr<Packet> packet){
    if (match(packet) && m_queue.size() < maxPackets){
        m_queue.push(packet);

        return true;
    }

    return false;
}

Ptr<Packet> TrafficClass::Dequeue(){
    if(m_queue.empty()){
        return nullptr;
    }
    else{
        Ptr<Packet> res = m_queue.front();
        m_queue.pop();
        return res;
    }
}

bool TrafficClass::match(Ptr<Packet> packet){
    for(Filter *filter : filter_vector){
        if(filter->match(packet)){
            return true;
        }
    }

    return false;
}

int TrafficClass::getQueueSize(){
    return m_queue.size();
}

Ptr<Packet> TrafficClass::peek() const{
    if(!m_queue.empty()){
        return m_queue.front();
    }

    cout << "Unable to peek: m_queue empty" << endl;
    return nullptr;
}

bool TrafficClass::isEmpty(){
    return m_queue.empty();
}

void TrafficClass::addFilter(Filter* set_filter){
    filter_vector.push_back(set_filter);
}

void TrafficClass::setPackets(uint32_t set_packets){
    packets = set_packets;
}

void TrafficClass::setMaxPackets(uint32_t max_packets){
    maxPackets = max_packets;
}

void TrafficClass::setWeight(double_t set_weight){
    weight = set_weight;
}

void TrafficClass::setPriorityLvl(uint32_t set_priority_level){
    priority_level = set_priority_level;
}

void TrafficClass::setDefault(bool is_default){
    isDefault = is_default;
}

void TrafficClass::setQuantumSize(uint32_t set_quantum_size){
    quantum_size = set_quantum_size;
}

void TrafficClass::setDeficitCounter(uint32_t set_deficit_counter){
    *deficit_counter = set_deficit_counter;
}