/*Abstract Class so no filter_element.cc*/

#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/object.h"

using namespace ns3;

class FilterElement : public ns3::Object{ //Inherits object since sub classes sets this as parent
    public:
        
        FilterElement() {};
        ~FilterElement() {};

        virtual bool match(ns3::Ptr<ns3::Packet> p) const = 0;
};

#endif