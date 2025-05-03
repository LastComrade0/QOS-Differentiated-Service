#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/packet.h"

#include "drr.h"
#include "traffic_class.h"
#include "util.h"

using namespace ns3;

int main(int argc, char *argv[]){
    LogComponentEnable("DRR", LOG_LEVEL_INFO);

    Ptr<DRR> drrScheduler = CreateObject<DRR>();

    for(int i = 0; i < 3; i += 1){
        TrafficClass *traffic = new TrafficClass();

        
    }
}