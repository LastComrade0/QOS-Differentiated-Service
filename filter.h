#ifndef FILTER_H
#define FILTER_H

#include "ns3/log.h"
#include "filter_element.h"
#include "src_ip.h"
#include "src_mask.h"
#include "src_port_number.h"
#include "dest_ip.h"
#include "dest_mask.h"
#include "dest_port_number.h"
#include "protocol_number.h"
#include <iostream>
#include <vector>
#include <memory>

using namespace std;
using namespace ns3;

class Filter: public Object{//Inherits object as NS_OBJECT_ENSURE_REGISTERED(Filter); was called in c++ file
    public:
        
        Filter();

        Filter(SrcIPAddress* src_ip, SrcMask* src_mask, SrcPortNumber* src_port,
                DestIPAddress* dest_ip, DestMask* dest_mask, DestPortNumber* dest_port,
                ProtocolNumber* protocol_number);
        
        ~Filter();

        vector<FilterElement*> filter_elements; //Shared Pointer for smart memory management

        bool match(Ptr<Packet> packet) const;

        void addElement(FilterElement *element);

};

#endif