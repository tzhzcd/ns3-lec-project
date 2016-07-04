/*
ns-3培训　2016.6
参考程序：projec2_2.cc  example/wireless/mixed-wireless.cc(ad-hoc骨干)
更改：　1.移动模型，和初始位置 ok
	2.传播模型改为　ConstantSpeedPropagationDelayModel  ok
	3.Mac类型为：NqosWifiMacHelper（发现一样，不需要修改） ok
	4.packet size为512byte，包间隔为100ms ok 
	5.wifi1、wifi2之间用ad-hoc连接　fail
说明：wifi1、wifi2之间改用ad-hoc连接失败了，现象是客户端发送正常，可是服务器回写失　败，无报错。猜想信道配置出错，由于时间问题，本project仍用p2p信道连接AP。出错程序见project4_WRONG.cc
*/
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

#include <iostream>
// Default Network Topology
//默认网络拓扑
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//  Wifi1 10.1.3.0
// 6个sta　     AP
//  *          *
//  |       　　　|      10.1.1.0
//n8-13     　　 n0        ……          　n1   　　 n2-n7
//                   wifi(ad-hoc)     |  　　   　 |
//                                    *         *
//                                    AP       6个sta
//                                    wifi2 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("project2_2");		

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi1 = 6;	//修改sta节点数		
  uint32_t nWifi2 = 6;				
   bool tracing = true;


  CommandLine cmd;
  cmd.AddValue ("nWifi2", "Number of wifi2 STA devices", nWifi2);
  cmd.AddValue ("nWifi1", "Number of wifi1 STA devices", nWifi1);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  if (nWifi1 > 250 || nWifi2 > 250)
    {
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);	
    }
  //把p2p信道改为Ad-hoc信道
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


  NodeContainer wifi1StaNodes;
  wifi1StaNodes.Create (nWifi1);
  NodeContainer wifi1ApNode = p2pNodes.Get (0);//需要修改
 
  YansWifiChannelHelper channel1;
  //修改信道的传播模型为ConstantSpeedPropagationDelayModel
  channel1.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel1.AddPropagationLoss ("ns3::LogDistancePropagationLossModel",
"Exponent", DoubleValue (3.0));
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());

  WifiHelper wifi1 = WifiHelper::Default ();
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac1 = NqosWifiMacHelper::Default ();

  Ssid ssid1 = Ssid ("ns-3-ssid1");
  mac1.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices1;
  staDevices1 = wifi1.Install (phy1, mac1, wifi1StaNodes);

  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));

  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy1, mac1, wifi1ApNode);

  NodeContainer wifi2StaNodes;
  wifi2StaNodes.Create (nWifi2);
  NodeContainer wifi2ApNode = p2pNodes.Get (1);//需要修改

  YansWifiChannelHelper channel2 ;
  //修改信道的传播模型为ConstantSpeedPropagationDelayModel
  //注意：只设置Delay,不设置Loss会蹦
  channel2.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel2.AddPropagationLoss ("ns3::LogDistancePropagationLossModel",
"Exponent", DoubleValue (3.0));
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi2 = WifiHelper::Default ();
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac2 = NqosWifiMacHelper::Default ();

  Ssid ssid2 = Ssid ("ns-3-ssid2");
  mac2.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid2),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices2;
  staDevices2 = wifi2.Install (phy2, mac2, wifi2StaNodes);

  mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi2.Install (phy2, mac2, wifi2ApNode);


  //修改移动模型
  //注意：移动模型后的参数是说明移动的范围，所以初始位置不能超出这个范围
  MobilityHelper mobility1;
  Ptr<ListPositionAllocator> positionAllocS = CreateObject<ListPositionAllocator> ();
  positionAllocS->Add(Vector(95.0, 58.66, 0.0));
  positionAllocS->Add(Vector(105.0, 58.66, 0.0));
  positionAllocS->Add(Vector(90.0, 50.0, 0.0));
  positionAllocS->Add(Vector(110.0, 50.0, 0.0));
  positionAllocS->Add(Vector(95.0, 41.34, 0.0));
  positionAllocS->Add(Vector(105.0, 41.34, 0.0));
  positionAllocS->Add(Vector(100.0, 50.0, 0.0));
  mobility1.SetPositionAllocator(positionAllocS);
	//设初始位置为以ap为圆心的六边形边长10m
  mobility1.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (0, 150, 0, 100)));
  mobility1.Install (wifi1StaNodes);
  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install (wifi1ApNode);

  //同样配置wifi2的移动模型
  MobilityHelper mobility2;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add(Vector(145.0, 58.66, 0.0));
  positionAlloc->Add(Vector(155.0, 58.66, 0.0));
  positionAlloc->Add(Vector(140.0, 50.0, 0.0));
  positionAlloc->Add(Vector(160.0, 50.0, 0.0));
  positionAlloc->Add(Vector(145.0, 41.34, 0.0));
  positionAlloc->Add(Vector(155.0, 41.34, 0.0));
  positionAlloc->Add(Vector(150.0, 50.0, 0.0));
  mobility2.SetPositionAllocator(positionAlloc);
  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (100, 250, 0, 100)));
  mobility2.Install (wifi2StaNodes);
  
  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifi2ApNode);
 

  InternetStackHelper stack;
  stack.Install (wifi2ApNode);
  stack.Install (wifi2StaNodes);
  stack.Install (wifi1ApNode);
  stack.Install (wifi1StaNodes);

 
  Ipv4AddressHelper address;
  //需要修改
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifi1staipadd;
  wifi1staipadd = address.Assign (staDevices1);
  address.Assign (apDevices1);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices2);
  address.Assign (apDevices2);


  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (wifi1StaNodes.Get (nWifi1-1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //packet size为512byte，包间隔为100ms
  UdpEchoClientHelper echoClient (wifi1staipadd.GetAddress (nWifi1-1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (512));
  
  ApplicationContainer clientApps_1 = 
  echoClient.Install (wifi2ApNode);
  clientApps_1.Start (Seconds (2.0));
  clientApps_1.Stop (Seconds (10.0));

  ApplicationContainer clientApps_2 = 
  echoClient.Install (wifi2StaNodes.Get(0));
  clientApps_2.Start (Seconds (3.0));
  clientApps_2.Stop (Seconds (10.0));

  ApplicationContainer clientApps_3 = 
  echoClient.Install (wifi2StaNodes.Get(1));
  clientApps_3.Start (Seconds (4.0));
  clientApps_3.Stop (Seconds (10.0));

  ApplicationContainer clientApps_4 = 
  echoClient.Install (wifi2StaNodes.Get(2));
  clientApps_4.Start (Seconds (5.0));
  clientApps_4.Stop (Seconds (10.0));
 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("project2_2");//需要修改
      phy1.EnablePcap ("project2_2", apDevices1.Get (0));
      phy2.EnablePcap ("project2_2", apDevices2.Get (0));
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
