/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/
#include <iostream>
// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

using namespace std;



namespace ns3 {

/**
 * This scenario simulates a very simple network topology:
 *
 *
 *      +----------+     1Mbps      +--------+     1Mbps      +----------+
 *      | consumer | <------------> | router | <------------> | producer |
 *      +----------+         10ms   +--------+          10ms  +----------+
 *
 *
 * Consumer requests data from producer with frequency 10 interests per second
 * (interests contain constantly increasing sequence number).
 *
 * For every received interest, producer replies with a data packet, containing
 * 1024 bytes of virtual payload.
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-simple
 */

int
main(int argc, char* argv[])
{
 

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 12);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/K-tree-Depth-4.txt");
  topologyReader.Read();


  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
   ndnHelper.setCsSize(100);
  ndnHelper.setPolicy("nfd::cs::lru01::Lru01Policy");
  //ndnHelper.setCsSize(100);
  //ndnHelper.setPolicy("nfd::cs::lru");
 // ndnHelper.SetOldContentStore("ns3::ndn::cs::Probability::Lru", "MaxSize", "100");
  //ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "100");
  //ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
  
  ndnHelper.InstallAll();

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();
  /****************************************************************************/
  // Getting containers for the consumer/producer
  Ptr<Node> consumer1 = Names::Find<Node>("Src1");
  Ptr<Node> consumer2 = Names::Find<Node>("Src2");
  Ptr<Node> consumer3 = Names::Find<Node>("Src3");
  Ptr<Node> consumer4 = Names::Find<Node>("Src4");
  Ptr<Node> consumer5 = Names::Find<Node>("Src5");
  Ptr<Node> consumer6 = Names::Find<Node>("Src6");
   
  Ptr<Node> producer1 = Names::Find<Node>("Dst1");
  Ptr<Node> producer2 = Names::Find<Node>("Dst2");
 
  
  // Consumer
  //ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbrFile");
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerfileZipfMandelbrot");
  //Consumer will request /prefix/0, /prefix/1, ... of distination files
  //consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/DatasetndnSIM.txt");
  consumerHelper.SetAttribute ("Frequency", DoubleValue (100.0)); // 10 interests a second
  //consumerHelper.SetAttribute("Randomize", StringValue("uniform"));
  consumerHelper.SetAttribute ("NumberOfContents", StringValue ("100")); // 10 different contents
    // that will express interests in /dst1 to /dst9 namespace
  consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/consumer1.txt");
  consumerHelper.Install(consumer1);

  consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/consumer2.txt");
  consumerHelper.Install(consumer2);

  consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/consumer3.txt");
  consumerHelper.Install(consumer3);

  consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/consumer4.txt");
  consumerHelper.Install(consumer4);
  
  consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/consumer5.txt");
  consumerHelper.Install(consumer5);
  
  consumerHelper.SetPrefix("/home/mohammed/Desktop/ndnSIM/consumer6.txt");
  consumerHelper.Install(consumer6);
  
    //auto apps = consumerHelper.Install(nodes.Get(0));                        // first node
  //apps.Stop(Seconds(23.05)); // stop the consumer app at 10 seconds mark

  // Producer
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  
  //producerHelper.SetPrefix("/Important");
  ndnGlobalRoutingHelper.AddOrigins("/Important/", producer1);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetPrefix("/Important");
  producerHelper.Install(producer1); // last node
  
  ndnGlobalRoutingHelper.AddOrigins("/NotImportant/", producer2);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetPrefix("/NotImportant");
  producerHelper.Install(producer2); // last node
  
 // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(600)); /*        ----------------------------------------------------------  */
  //ndn::CsTracer1::Install(nodes.Get(1),"cs-trace-lru.txt", Seconds(0.5));
  //ndn::L3RateTracer::Install(producer1,"rate-traceconf.csv", Seconds(1));
  //ndn::AppDelayTracer::InstallAll("/home/mohammed/Desktop/UATL0.1/My-app-delays-trace-tree4.csv");
 // ndn::CsTracer::InstallAll("/home/mohammed/Desktop/Datasets/cs-trace-Placement.csv", Seconds(1.0));
  //ndn::L3RateTracer::InstallAll("rate-traceconf.csv", Seconds(1));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
