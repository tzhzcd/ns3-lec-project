/*
ns-3培训　2016.6
参考程序：third.cc
更改：wifi的移动模型改为匀速；客户端安装在全部4个节点上
*/

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

// Default Network Topology
//默认网络拓扑
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   Wifi 10.1.3.0
//                         AP
//  *      *      *     *
//  |      |      |     |      10.1.1.0
// 　　n5 　  n6  　 n7  　　 n0 　------------------ n1   n2   n3   n4
//                    　　　　　　　point-to-point    |    |    |    |
//                                    　　　　　　　  ================
//                                        　　　　　  LAN 10.1.2.0

using namespace ns3;
  //注册记录组件
NS_LOG_COMPONENT_DEFINE ("project2_1");		

int 
main (int argc, char *argv[])
{
  bool verbose = true;//logging系统日志功能开启，输出系统信息
  uint32_t nCsma = 3;			
  uint32_t nWifi = 3;				
  bool tracing = true;//记录设备设备信息功能开启，生产相应文件


  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);
  if (nWifi > 250 || nCsma > 250)
    {
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
    }
  //详见logging
  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);	
    }


  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();

 
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

 
  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  //配置移动模型，起始位置
  MobilityHelper mobility;
/*
//此段为设置节点初始位置，以(0,0)为第一位置，横间距为5，纵间距为10，每行排3个节点
 mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
 */
  //配置STA移动方式，ConstantVelocityMobilityModel，固定速度模型
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install (wifiStaNodes);
  //设置必要参数，先设初始位置，后设初始时间。
  for (uint n = 0; n < wifiStaNodes.GetN(); n++)
  {
    Ptr<ConstantVelocityMobilityModel> mob = wifiStaNodes.Get(n)->GetObject<ConstantVelocityMobilityModel>();
    mob->SetPosition(Vector(0.0, 0.0, 0.0));   
    mob->SetVelocity(Vector(n/2.0+0.5, 0.0, 0.0));
  }
  //配置AP移动方式，ConstantPositionMobilityModel，固定位置模型
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  //设置初始位置
  Ptr<ConstantPositionMobilityModel> mob = wifiApNode.Get(0)->GetObject<ConstantPositionMobilityModel>();
  mob->SetPosition(Vector(0.0, 0.0, 0.0)); 
  


  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

 
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);
 
  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices);
  address.Assign (apDevices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  //安装客户端程序到节点，需要重复定义。
  ApplicationContainer clientApps_1 = 
  echoClient.Install (wifiApNode);
  clientApps_1.Start (Seconds (2.0));
  clientApps_1.Stop (Seconds (10.0));

  ApplicationContainer clientApps_2 = 
  echoClient.Install (wifiStaNodes.Get(0));
  clientApps_2.Start (Seconds (2.0));
  clientApps_2.Stop (Seconds (10.0));

  ApplicationContainer clientApps_3 = 
  echoClient.Install (wifiStaNodes.Get(1));
  clientApps_3.Start (Seconds (2.0));
  clientApps_3.Stop (Seconds (10.0));

  ApplicationContainer clientApps_4 = 
  echoClient.Install (wifiStaNodes.Get(2));
  clientApps_4.Start (Seconds (3.0));
  clientApps_4.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  //详见tracing设备信息功能
  Simulator::Stop (Seconds (10.0));
  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("project2_1");
      phy.EnablePcap ("project2_1", apDevices.Get (0));
      csma.EnablePcap ("project2_1", csmaDevices.Get (0), true);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
