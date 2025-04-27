#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "filter_element.h"
#include "traffic_class.cc"
#include <iostream>
#include <vector>
#include <memory>

using namespace std;
using namespace ns3;

class Filter : public TrafficClass{
    public:
        vector<shared_ptr<FilterElement>> filter_elements; //Shared Pointer for smart memory management

        bool match(Ptr<Packet> packet) const;
        
    
        
};