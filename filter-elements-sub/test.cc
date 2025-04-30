/* CS621: Lab 3 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/queue.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include <memory>
#include <vector>

/*
#include "filter_element.h"
#include "filter-elements-sub/src_ip.cc"
#include "filter-elements-sub/src_mask.cc"
#include "filter-elements-sub/src_port_number.cc"
#include "filter-elements-sub/dest_ip.cc"
#include "filter-elements-sub/dest_mask.cc"
#include "filter-elements-sub/dest_port_number.cc"
#include "filter-elements-sub/protocol_number.cc"*/

//#include "filter.cc"
//#include "traffic_class.cc"
#include "diffserv.cc"


// Default Network Topology
//
//       10.1.1.0

// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MyFirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  //TODO: Create two nodes
  NodeContainer nodes;
  nodes.Create(2);

  
  //TODO: Create a PointToPoint channel and set its parameters; create devices, 
  //      and install the channel on the nodes.
  PointToPointHelper pointToPoint;

  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));

  pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

  NetDeviceContainer devices;

  devices = pointToPoint.Install(nodes);


  //TODO: Use Ipv4InterfaceContainer, InternetStackHelper, and Ipv4AddressHelper 
  //      to install and assign IPv4 addresses.

  InternetStackHelper stack;
  stack.Install(nodes);

  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign(devices);


  //TODO: Use the associalted "helpers" to create UdpEchoServer and UdpEchoClient applications, 
  //      install each app on the designated node, set Start and Stop times for each app, 
  //      and use SetAttribute to set the proper parameters for the client app.

  UdpEchoServerHelper echoServer(9999); //Port 9999 listen

  ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));

  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(10.0));

  UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9999);

  echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  echoClient.SetAttribute("Interval", TimeValue(MicroSeconds(100)));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));

  ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));

  clientApps.Start(Seconds(2.0));
  clientApps.Stop(Seconds(10.0));

  
  //TODO: Use SetFill in the client app to populate the payload.

  echoClient.SetFill(clientApps.Get(0), "Test send");

  //Enable generating the pcap files.
  pointToPoint.EnablePcapAll("simple-udp-echo");


  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}