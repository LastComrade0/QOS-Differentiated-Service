#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "traffic_class.h"
#include <iostream>
#include <vector>
#include <memory>

using namespace std;
using namespace ns3;

//DRR
TrafficClass::TrafficClass(uint32_t max_packets, uint32_t quantum_size, uint32_t *deficit_counter, bool isDefault,
                            Ipv4Address src_ip, Ipv4Mask src_mask, uint32_t src_port,
                            Ipv4Address dest_ip, Ipv4Mask dest_mask, uint32_t dest_port, uint32_t protocol_number) : maxPackets(max_packets), quantum_size(quantum_size),
                                                                                                                     deficit_counter(deficit_counter), isDefault(isDefault) {
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

TrafficClass::~TrafficClass(){}

        
bool Enqueue(Ptr<Packet> packet){
    return false;
}

Ptr<Packet> Dequeue(){
    return nullptr;
}

bool match(Ptr<Packet> packet){
    return false;
}
