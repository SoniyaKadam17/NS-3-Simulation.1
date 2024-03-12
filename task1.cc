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
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//    192.168.2.0/24
// n0 -------------- n1
//    point-to-point
//
// SONIYA NITIN KADAM GROUP27

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Assignment1Task1"); //defining a new log component named "Assignment1Task1".

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);  // Enable logging at the INFO level for the log components 
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes; // Create 2 nodes in point-to-point connection
    nodes.Create(2);

    PointToPointHelper pointToPoint;// Adding link attributes for the point to point link
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));// setting data rate as 10Mbps for the point to point link
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));// setting delay as 2ms for the point to point link

    //method installs a point-to-point link between the two nodes specified by the p2pNodes
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    InternetStackHelper stack;// InternetStackHelper class is used to install the Internet stack on p2p
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("192.168.2.0", "255.255.255.0"); // Setting IP address of the P2P link

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    UdpEchoServerHelper echoServer(63);// sets the listening port to 63

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1)); // Echoing to server with index 1
    serverApps.Start(Seconds(1.0));// start time
    serverApps.Stop(Seconds(10.0));// stop time

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 63);// specifies the destination address and port number for the echo client.
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));// sending 1 packet
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));// packet at interval of 1 second
    echoClient.SetAttribute("PacketSize", UintegerValue(256));// size of the packets is 256 bytes

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));// Echoing to client 
    clientApps.Start(Seconds(2.0));// start time
    clientApps.Stop(Seconds(10.0));// stop time

    pointToPoint.EnablePcapAll ("Task1"); // creating Pcap file of the point to point nodes

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
