/*Abstract Class so no filter_element.cc*/

#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"

class FilterElement {
    public:
        virtual bool match(ns3::Ptr<ns3::Packet> p) const = 0;
        virtual ~FilterElement() {};
};

#endif