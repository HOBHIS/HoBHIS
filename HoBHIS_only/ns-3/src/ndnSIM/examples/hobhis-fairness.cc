/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 *
 * Author: Natalya Rozhnova <natalya.rozhnova@lip6.fr>
 */

/*
 *	For the questions, search "****?"
 *	Description :
 *	1, define a function CheckQueueSize (Ptr<Queue> queue, Ptr<NetDeviceFace> ndf) for record every 0.01s those numbers of packets(interest and data) in different queues
 *	2, define a function CheckInterestQueueSize (Ptr<NetDeviceFace> ndf) for record every 0.01s those numbers of the interest packets in different queues
 *	3,
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <ns3/ndnSIM/utils/tracers/ndn-l3-aggregate-tracer.h>
#include <ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.h>
#include "ns3/ndn-hobhis-net-device-face.h"

#include <ns3/ndnSIM/utils/tracers/ndn-app-delay-tracer.h>
#include "ns3/point-to-point-net-device.h"
#include "ns3/point-to-point-module.h"

#include "ns3/ndn-drop-tail-queue.h"

using namespace ns3;
using namespace ndn;

std::stringstream filePlotQueue;
std::stringstream filePlotInterestQueue;
std::stringstream filePlotQueue1;

void
// ****?CheckQueueSize (Ptr<Queue> queue, Ptr<HobhisNetDeviceFace> ndf)?
CheckQueueSize (Ptr<Queue> queue, Ptr<NetDeviceFace> ndf)
{
	// define different the number of packets of queues
	uint32_t DqtotalSize;
	uint32_t queuec1 = 0;
	uint32_t queuec2 = 0;
	uint32_t queuec3 = 0;
	// give the number of packets currently stored in the queue to the variable DqtotalSize
	DqtotalSize = queue->GetNPackets();

	// if HobhisEnabled is true
	if(ndf->HobhisEnabled()==true){
		// cast queue （type Ptr<Queue>） to ndnqueue （type Ptr<NDNDropTailQueue>）, like copy. ****?Ptr<NDNDropTailQueue> ndnqueue = StaticCast<Qeueue> (queue)?
		Ptr<NDNDropTailQueue> ndnqueue = StaticCast<NDNDropTailQueue> (queue);
		if(ndnqueue != NULL)
		{
			// get those different numbers of packets for three different queues who have different prefix
			queuec1 = ndnqueue->GetQueueSizePerFlow("/c1");
			queuec2 = ndnqueue->GetQueueSizePerFlow("/c2");
			queuec3 = ndnqueue->GetQueueSizePerFlow("/c3");

			// update the numbers total of packets for DqtotalSize
			DqtotalSize = ndnqueue->GetDataQueueLength();
		}
	}

	// check queue size every 1/100 of a second by calling itself: update variable DqtotalSize, queuec1, queuec2 and queuec3
	Simulator::Schedule (Seconds (0.01), &CheckQueueSize, queue, ndf);

	// print each three queues size by flow(prefixs) and total DqtotalSize
	std::ofstream fPlotQueue (filePlotQueue.str ().c_str (), std::ios::out|std::ios::app);
	fPlotQueue << Simulator::Now ().GetSeconds () << " @ "<< queue <<" /c1 "<<queuec1<<" /c2 "<<queuec2<<" /c3 "<<queuec3<< " -total " <<DqtotalSize<< std::endl;
	fPlotQueue.close ();
}

void
CheckInterestQueueSize (Ptr<NetDeviceFace> ndf)
{
	// cast ndf(type NetDeviceFace) to hndf(type HobhisNetDeviceFace) ****? Ptr<HobhisNetDeviceFace> hndf = StaticCast<NetDeviceFace> (ndf);
	Ptr<HobhisNetDeviceFace> hndf = StaticCast<HobhisNetDeviceFace> (ndf);
	// define different queues
	uint32_t queuec1 = 0;
	uint32_t queuec2 = 0;
	uint32_t queuec3 = 0;

	// get the size of interest queue to IqSize
	uint32_t IqSize = hndf->GetQueueLength();
	// check queue size every 1/100 of a second by calling itself: update variable DqtotalSize, queuec1, queuec2 and queuec3
	Simulator::Schedule (Seconds (0.01), &CheckInterestQueueSize, ndf);
	// get those different numbers of packets for three different queues who have different prefix
	queuec1 = hndf->GetIntQueueSizePerFlow("/c1");
	queuec2 = hndf->GetIntQueueSizePerFlow("/c2");
	queuec3 = hndf->GetIntQueueSizePerFlow("/c3");

	// print each three interest queues size by flow(prefixs) and total IqSize
	std::ofstream fPlotQueue (filePlotInterestQueue.str ().c_str (), std::ios::out|std::ios::app);
	fPlotQueue << Simulator::Now ().GetSeconds () << " @ "<< hndf << " -tql " <<IqSize<<" /c1 "<<queuec1<<" /c2 "<<queuec2<<" /c3 "<<queuec3<< std::endl;
	fPlotQueue.close ();
}

int
main (int argc, char *argv[])
{
	// default writeForPlot = false
	bool writeForPlot = false;

	// control if writeForPlot and get parameters via command line
	CommandLine cmd;
	cmd.AddValue ("wfp", "<0/1> to write results for plot (gnuplot)", writeForPlot);
	cmd.Parse (argc, argv);

	// Read topology
	AnnotatedTopologyReader topologyReader ("", 25);
	topologyReader.SetFileName ("src/ndnSIM/examples/topologies/hobhis-fairness.txt");
	topologyReader.Read ();

	// Getting containers for the consumer/producer and router
	Ptr<Node> c1 = Names::Find<Node> ("C1");
	Ptr<Node> c2 = Names::Find<Node> ("C2");
	Ptr<Node> c3 = Names::Find<Node> ("C3");
	Ptr<Node> r1 = Names::Find<Node> ("R1");
	Ptr<Node> r2 = Names::Find<Node> ("R2");
	Ptr<Node> r3 = Names::Find<Node> ("R3");
	Ptr<Node> s1 = Names::Find<Node> ("P1");
	Ptr<Node> s2 = Names::Find<Node> ("P2");
	Ptr<Node> s3 = Names::Find<Node> ("P3");

	// Install CCNx stack on all nodes
	ndn::StackHelper ndnHelper;
	// set forwarding strategy
	ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
	// (enabled, client/server, INTEREST buffer size, target, convergence rate)
	ndnHelper.EnableHobhis (true, false, 1000000, 60, 0.7);
	// almost no caching, max size is 1
	ndnHelper.SetContentStore ("ns3::ndn::cs::Lru", "MaxSize", "1");
	// install for r1, r2 and r3
	ndnHelper.Install (r1);
	ndnHelper.Install (r2);
	ndnHelper.Install (r3);

	//**************************** Installing NDN-drop-tail-queue to manage the data queue on each router. ******************************************

	// Router 1 interface 0
	ObjectFactory factory;
	// define variable type L3Protocol ndn from r1
	Ptr<L3Protocol> ndn = r1->GetObject<L3Protocol> ();
	// define variable type Face face 0 from ndn
	Ptr<Face> face = ndn->GetFace (0);
	// cast face (type Face) to ndf (type NetDeviceFace) ****?Ptr<NetDeviceFace> ndf = DynamicCast<Face>(face)?
	Ptr<NetDeviceFace> ndf = DynamicCast<NetDeviceFace>(face);
	// define variable type NetDevice nd from ndf
	Ptr<NetDevice> nd = ndf->GetNetDevice();
	// cast nd (type NetDevice) to p2pnd (type PointToPointNetDevice) ****?Ptr<PointToPointNetDevice> p2pnd = StaticCast<NetDevice> (nd)?
	Ptr<PointToPointNetDevice> p2pnd = StaticCast<PointToPointNetDevice> (nd);

	// define type ID and default config : max packets is 100
	factory.SetTypeId("ns3::NDNDropTailQueue");
	Config::SetDefault ("ns3::NDNDropTailQueue::MaxPackets", UintegerValue (100));
	// create a queueA
	Ptr<Queue> queueA = factory.Create<Queue> ();
	// cast queueA (type Queue) to ndnqueue (type NDNDropTailQueue)
	Ptr<NDNDropTailQueue> ndnqueue = StaticCast<NDNDropTailQueue> (queueA);
	// set ndnqueue mode
	ndnqueue->SetMode(ndn::NDNDropTailQueue::QUEUE_MODE_PACKETS);
	// install queueA a p2pnd
	p2pnd->SetQueue (queueA);

	// Router 1 interface 1

	ObjectFactory factory3;
	Ptr<L3Protocol> ndn3 = r1->GetObject<L3Protocol> ();
	Ptr<Face> face3 = ndn3->GetFace (1);
	Ptr<NetDeviceFace> ndf3 = DynamicCast<NetDeviceFace>(face3);
	Ptr<NetDevice> nd3 = ndf3->GetNetDevice();
	Ptr<PointToPointNetDevice> p2pnd3 = StaticCast<PointToPointNetDevice> (nd3);

	factory3.SetTypeId("ns3::NDNDropTailQueue");
	Config::SetDefault ("ns3::NDNDropTailQueue::MaxPackets", UintegerValue (100));
	Ptr<Queue> queueC = factory3.Create<Queue> ();
	Ptr<NDNDropTailQueue> ndnqueue3 = StaticCast<NDNDropTailQueue> (queueC);
	ndnqueue3->SetMode(ndn::NDNDropTailQueue::QUEUE_MODE_PACKETS);
	p2pnd3->SetQueue (queueC);

	// Router 1 interface 2
	ObjectFactory factoryF2;
	Ptr<L3Protocol> ndnF2 = r1->GetObject<L3Protocol> ();
	Ptr<Face> faceF2 = ndnF2->GetFace (2);
	Ptr<NetDeviceFace> ndfF2 = DynamicCast<NetDeviceFace>(faceF2);
	Ptr<NetDevice> ndF2 = ndfF2->GetNetDevice();
	Ptr<PointToPointNetDevice> p2pndF2 = StaticCast<PointToPointNetDevice> (ndF2);

	factoryF2.SetTypeId("ns3::NDNDropTailQueue");
	Config::SetDefault ("ns3::NDNDropTailQueue::MaxPackets", UintegerValue (100));
	Ptr<Queue> queueF2 = factoryF2.Create<Queue> ();
	Ptr<NDNDropTailQueue> ndnqueueF2 = StaticCast<NDNDropTailQueue> (queueF2);
	ndnqueueF2->SetMode(ndn::NDNDropTailQueue::QUEUE_MODE_PACKETS);
	p2pndF2->SetQueue (queueF2);

	// Router 2
	ObjectFactory factory2;
	Ptr<L3Protocol> ndn2 = r2->GetObject<L3Protocol> ();
	Ptr<Face> face2 = ndn2->GetFace (0);
	Ptr<NetDeviceFace> ndf2 = DynamicCast<NetDeviceFace>(face2);
	Ptr<NetDevice> nd2 = ndf2->GetNetDevice();
	Ptr<PointToPointNetDevice> p2pnd2 = StaticCast<PointToPointNetDevice> (nd2);

	factory2.SetTypeId("ns3::NDNDropTailQueue");
	Config::SetDefault ("ns3::NDNDropTailQueue::MaxPackets", UintegerValue (100));
	Ptr<Queue> queueB = factory2.Create<Queue> ();
	Ptr<NDNDropTailQueue> ndnqueue2 = StaticCast<NDNDropTailQueue> (queueB);
	ndnqueue2->SetMode(ndn::NDNDropTailQueue::QUEUE_MODE_PACKETS);
	p2pnd2->SetQueue (queueB);

	// Router 3
	ObjectFactory factory4;
	Ptr<L3Protocol> ndn4 = r3->GetObject<L3Protocol> ();
	Ptr<Face> face4 = ndn4->GetFace (0);
	Ptr<NetDeviceFace> ndf4 = DynamicCast<NetDeviceFace>(face4);
	Ptr<NetDevice> nd4 = ndf4->GetNetDevice();
	Ptr<PointToPointNetDevice> p4pnd4 = StaticCast<PointToPointNetDevice> (nd4);

	factory4.SetTypeId("ns3::NDNDropTailQueue");
	Config::SetDefault ("ns3::NDNDropTailQueue::MaxPackets", UintegerValue (100));
	Ptr<Queue> queueD = factory4.Create<Queue> ();
	Ptr<NDNDropTailQueue> ndnqueue4 = StaticCast<NDNDropTailQueue> (queueD);
	ndnqueue4->SetMode(ndn::NDNDropTailQueue::QUEUE_MODE_PACKETS);
	p4pnd4->SetQueue (queueD);

	//***********************************************************************************************************************************************************

	ndn::StackHelper ndnHelper1;
	ndnHelper1.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
	ndnHelper1.EnableHobhis (true, true);   // Consumers and Servers don't need HoBHIS to shape the packets
	ndnHelper1.SetContentStore ("ns3::ndn::cs::Lru", "MaxSize", "1"); // almost no caching
	ndnHelper1.Install (c1);
	ndnHelper1.Install (s1);
	ndnHelper1.Install (c2);
	ndnHelper1.Install (s2);
	ndnHelper1.Install (c3);
	ndnHelper1.Install (s3);


	// Installing global routing interface on all nodes
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll ();

	// Install consumers
	ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
	consumerHelper.SetAttribute("Frequency", StringValue ("100.0"));

	consumerHelper.SetPrefix ("/c1");
	consumerHelper.Install (c1);

	ndn::AppHelper consumerHelper2 ("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetAttribute("Frequency", StringValue ("25.0"));

	consumerHelper2.SetPrefix ("/c2");
	consumerHelper2.Install (c2);

	ndn::AppHelper consumerHelper3 ("ns3::ndn::ConsumerCbr");
	consumerHelper3.SetAttribute("Frequency", StringValue ("100.0"));

	consumerHelper3.SetPrefix ("/c3");
	consumerHelper3.Install (c3);

	// Register prefix with global routing controller and install producer

	ndnGlobalRoutingHelper.AddOrigins ("/c1", s1);
	ndnGlobalRoutingHelper.AddOrigins ("/c2", s2);
	ndnGlobalRoutingHelper.AddOrigins ("/c3", s3);

	// Install servers
	ndn::AppHelper producerHelper ("ns3::ndn::Producer");
	producerHelper.SetAttribute ("PayloadSize", StringValue("1000"));
	producerHelper.SetAttribute ("RandomDelayMin", StringValue("0"));
	producerHelper.SetAttribute ("RandomDelayMax", StringValue("0.1"));
	producerHelper.SetPrefix ("/c1");
	producerHelper.Install (s1);

	ndn::AppHelper producerHelper2 ("ns3::ndn::Producer");
	producerHelper2.SetAttribute ("PayloadSize", StringValue("1000"));
	producerHelper2.SetAttribute ("RandomDelayMin", StringValue("0"));
	producerHelper2.SetAttribute ("RandomDelayMax", StringValue("0.1"));
	producerHelper2.SetPrefix ("/c2");
	producerHelper2.Install (s2);

	ndn::AppHelper producerHelper3 ("ns3::ndn::Producer");
	producerHelper3.SetAttribute ("PayloadSize", StringValue("1000"));
	producerHelper3.SetAttribute ("RandomDelayMin", StringValue("0"));
	producerHelper3.SetAttribute ("RandomDelayMax", StringValue("0.1"));
	producerHelper3.SetPrefix ("/c3");
	producerHelper3.Install (s3);

	// Calculate and install FIBs
	ndnGlobalRoutingHelper.CalculateRoutes ();

	Simulator::Stop (Seconds (50.0));

	// Queue trace

	std::string pathOut = ".";
	if (writeForPlot)
	{
		filePlotQueue << pathOut << "/" << "data-queue-r2-r1.plotme";
		remove (filePlotQueue.str ().c_str ());
		Ptr<L3Protocol> ndn = r2->GetObject<L3Protocol> ();

		Ptr<Face> face = ndn->GetFace (0);
		Ptr<NetDeviceFace> ndf = DynamicCast<NetDeviceFace>(face);
		Ptr<NetDevice> nd = ndf->GetNetDevice();
		Ptr<PointToPointNetDevice> p2pnd = StaticCast<PointToPointNetDevice> (nd);
		Ptr<Queue> queue = p2pnd->GetQueue();
		Simulator::ScheduleNow (&CheckQueueSize, queue, ndf);

		// Check Interest queue size
		Ptr<Face> face1 = ndn->GetFace (1);
		Ptr<NetDeviceFace> ndf1 = DynamicCast<NetDeviceFace>(face1);
		filePlotInterestQueue << pathOut << "/" << "interest-queue-r2-r3.plotme";
		remove (filePlotInterestQueue.str ().c_str ());
		Simulator::ScheduleNow (&CheckInterestQueueSize, ndf1);
	}
	Simulator::Run ();
	Simulator::Destroy ();

	return 0;
}
