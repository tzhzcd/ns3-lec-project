/*
ns-3培训　2016.6
参考程序：projec2_1.cc
更改：csma信道改为wifi2，服务器安装在n4节点即wifi2最后一个节点上。
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
//   Wifi1 10.1.3.0
//                         AP
//  *      *      *     *
//  |      |      |     |      10.1.1.0
//　 n5   　n6　 　  n7  　　 n0 ------------------ 　n1   　n2 　　　 n3  　　 n4
//                         point-to-point      |  　　  |  　 　 |   　 |
//                                             *      *      *     *
//                                                wifi2 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("project2_2");		

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi1 = 3;			
  uint32_t nWifi2 = 3;				
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

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);
/*//修改后舍弃
  //创建csma节点，包含一个p2p节点

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  //创建信道，设置信道参数，在设备安装到节点上
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);
*/

                /*搭建Wifi1拓扑*/
  NodeContainer wifi1StaNodes;
  wifi1StaNodes.Create (nWifi1);
  NodeContainer wifi1ApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
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

                /*搭建Wifi2拓扑*/
  NodeContainer wifi2StaNodes;
  wifi2StaNodes.Create (nWifi2);
  NodeContainer wifi2ApNode = p2pNodes.Get (1);

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());

  WifiHelper wifi2 = WifiHelper::Default ();
  wifi2.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac2 = NqosWifiMacHelper::Default ();
  //ssid不一样
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


  //配置wifi1移动模型
  MobilityHelper mobility1;
  mobility1.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility1.Install (wifi1StaNodes);
  for (uint n = 0; n < wifi1StaNodes.GetN(); n++)
  {
    Ptr<ConstantVelocityMobilityModel> mob = wifi1StaNodes.Get(n)->GetObject<ConstantVelocityMobilityModel>();
    mob->SetPosition(Vector(80.0, 50.0, 0.0));
    mob->SetVelocity(Vector(-(n/2.0+0.5), 0.0, 0.0));   

  }
  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install (wifi1ApNode);
  Ptr<ConstantPositionMobilityModel> mob = wifi1ApNode.Get(0)->GetObject<ConstantPositionMobilityModel>();
  mob->SetPosition(Vector(80.0, 50.0, 0.0)); 


  //配置wifi2移动模型，必须和wifi1分开配置，否则很难调位置。
  MobilityHelper mobility2;
  mobility2.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility2.Install (wifi2StaNodes);
  for (uint n2 = 0; n2 < wifi2StaNodes.GetN(); n2++)
  {
    Ptr<ConstantVelocityMobilityModel> mob2 = wifi2StaNodes.Get(n2)->GetObject<ConstantVelocityMobilityModel>();
    mob2->SetPosition(Vector(120.0, 50.0, 0.0));   
    mob2->SetVelocity(Vector(n2/2.0+0.5, 0.0, 0.0));
  }

  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifi2ApNode);
  Ptr<ConstantPositionMobilityModel> mob2 = wifi2ApNode.Get(0)->GetObject<ConstantPositionMobilityModel>();
  mob2->SetPosition(Vector(120.0, 50.0, 0.0)); 
 

  InternetStackHelper stack;
  stack.Install (wifi2ApNode);
  stack.Install (wifi2StaNodes);
  stack.Install (wifi1ApNode);
  stack.Install (wifi1StaNodes);

 
  Ipv4AddressHelper address;
 
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);
 //wifi1信道ip地址
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifi1staipadd;
  wifi1staipadd = address.Assign (staDevices1);
  address.Assign (apDevices1);
 //wifi2信道ip地址
  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices2);
  address.Assign (apDevices2);

  //放置echo服务端程序在最右边的wifi1节点,端口为9
  //注意是.get(nwifi1-1)!!
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (wifi1StaNodes.Get (nWifi1-1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //回显客户端放在wifi2最后的STA节点，指向wifi2网络的服务器，上面的节点地址，端口为9
  //.GetAddress函数是NetDeviceContainer的方法,和具体是csma还是wifi节点无关。
  UdpEchoClientHelper echoClient (wifi1staipadd.GetAddress (nWifi1-1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  
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
      pointToPoint.EnablePcapAll ("project2_2");
      phy1.EnablePcap ("project2_2", apDevices1.Get (0));
      phy2.EnablePcap ("project2_2", apDevices2.Get (0));
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
