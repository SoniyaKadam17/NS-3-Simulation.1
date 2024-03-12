/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

//              TOPOLOGY
//                   
//             192.168.3.0/24 
//  n0  n1 n2  -------------   n3  n4  n5  
//  |   |   |                 |   |   |
//  =========                  ==========
// 192.168.1.0/24            192.168.2.0/24
//  (CSMA link1)              (CSMA link2)

// SONIYA NITIN KADAM GROUP27
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Assignment1Task2"); //defining a new log component named "Assignment1Task2".

int
main(int argc, char* argv[])
{
    bool verbose = true; //variable controls whether or not the simulation logs 
    //unsigned 32-bit integer variable
    uint32_t nCsma1 = 2; // variables control the number of CSMA link 1 nodes i.e. 2 nodes
    uint32_t nCsma2 = 2; // variables control the number of CSMA link 2 nodes i.e. 2 nodes

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma1", "Number of \"extra\" CSMA nodes/devices", nCsma1); //The number of CSMA nodes in the first CSMA network.
    cmd.AddValue("nCsma2", "Number of \"extra\" CSMA nodes/devices", nCsma2); //The number of CSMA nodes in the second CSMA network.
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose); //variable controls whether or not the simulation logs 

    cmd.Parse(argc, argv);

    if (verbose) // Enable logging at the INFO level for the log components if verbose command line argument is set
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma1 = nCsma1 == 0 ? 1 : nCsma1;// If nCsma1 = 0 sent to 1 to ensure working.
    nCsma2 = nCsma2 == 0 ? 1 : nCsma2;// If nCsma2 = 0 sent to 1 to ensure working.

    NodeContainer p2pNodes; // Create 2 nodes in point-to-point connection
    p2pNodes.Create(2);

    NodeContainer csmaNodes2; // Adding the nCsma2 to the point-to-point link node with index1
    csmaNodes2.Add(p2pNodes.Get(1));
    csmaNodes2.Create(nCsma2);

    NodeContainer csmaNodes1; // Adding the nCsma1 to the point-to-point link node with index0
    csmaNodes1.Add(p2pNodes.Get(0));
    csmaNodes1.Create(nCsma1); 
    

    PointToPointHelper pointToPoint; // Adding link attributes for the point to point link
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps")); // setting data rate as 10Mbps for the point to point link
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));// setting delay as 2ms for the point to point link

    //method installs a point-to-point link between the two nodes specified by the p2pNodes
    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    CsmaHelper ncsma1;// Adding link attributes for the nCsma1 link
    ncsma1.SetChannelAttribute("DataRate", StringValue("100Mbps"));// setting data rate as 100Mbps for the nCsma1 link
    ncsma1.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));// setting delay as 10 micro sec for the nCsma1 link

    CsmaHelper ncsma2;// Adding link attributes for the nCsma2 link
    ncsma2.SetChannelAttribute("DataRate", StringValue("100Mbps"));// setting data rate as 100Mbps for the nCsma2 link
    ncsma2.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));// setting delay as 10 micro sec for the nCsma2 link

    //method installs a CSMA link between the two nodes specified by csmaNodes1 & csmaNodes2
    NetDeviceContainer csmaDevices1, csmaDevices2; 
    csmaDevices1 = ncsma1.Install(csmaNodes1);
    csmaDevices2 = ncsma2.Install(csmaNodes2);

    InternetStackHelper stack0;// InternetStackHelper class is used to install the Internet stack on p2p and CSMA link1
    stack0.Install(p2pNodes.Get(0));
    stack0.Install(csmaNodes1);
  

    InternetStackHelper stack1;// InternetStackHelper class is used to install the Internet stack on p2p and CSMA link2
    stack1.Install(p2pNodes.Get(1));
    stack1.Install(csmaNodes2);


    Ipv4AddressHelper address; 
    address.SetBase("192.168.3.0", "255.255.255.0");// Setting IP address of the P2P link
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("192.168.1.0", "255.255.255.0");// Setting IP address of the CSMA link1
    Ipv4InterfaceContainer csmaInterfaces1;
    csmaInterfaces1 = address.Assign(csmaDevices1);

    address.SetBase("192.168.2.0", "255.255.255.0");// Setting IP address of the CSMA link2
    Ipv4InterfaceContainer csmaInterfaces2;
    csmaInterfaces2 = address.Assign(csmaDevices2);

    UdpEchoServerHelper echoServer(21);// sets the listening port to 21

    ApplicationContainer serverApps = echoServer.Install(csmaNodes1.Get(1)); // Echoing to server at CSMA link 1 with index 1
    serverApps.Start(Seconds(4.0));// start time
    serverApps.Stop(Seconds(10.0));// stop time

    UdpEchoClientHelper echoClient(csmaInterfaces1.GetAddress(1), 21); // specifies the destination address and port number for the echo client.
    echoClient.SetAttribute("MaxPackets", UintegerValue(2)); // sending 2 packets
    echoClient.SetAttribute("Interval", TimeValue(Seconds(3.0))); // 2 packets at interval of 3 seconds
    echoClient.SetAttribute("PacketSize", UintegerValue(1024)); // size of the packets is 1024 bytes


    ApplicationContainer clientApps = echoClient.Install(csmaNodes2.Get(nCsma2));// Echoing to client at CSMA link 2 
    clientApps.Start(Seconds(4.0));// start time
    clientApps.Stop(Seconds(10.0));// stop time

    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); // Building routing Database

    ncsma1.EnablePcap("serverside", csmaDevices1.Get(0), true); // creating Pcap file at node in CSMA link 1 with index 0 which is Node2 
    ncsma2.EnablePcap("clientside", csmaDevices2.Get(1), true); // creating Pcap file at node in CSMA link 2 with index 1 which is Node4

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}