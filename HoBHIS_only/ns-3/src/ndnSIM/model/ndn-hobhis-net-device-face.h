/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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
 * Authors: Rozhnova Natalya <natalya.rozhnova@lip6.fr>
 */

#ifndef NDN_HOBHIS_NET_DEVICE_FACE_H
#define NDN_HOBHIS_NET_DEVICE_FACE_H

#include <queue>
#include "ndn-net-device-face.h"
#include "ns3/net-device.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"

//#include "ns3/random-variable.h"

namespace ns3 {
namespace ndn {

/**
 * \ingroup ndn-face
 * \brief Implementation of layer-2 (Ethernet) Ndn face with interest shaping
 *
 * This class adds interest shaping to NdnNetDeviceFace.
 *
 * \see NdnNetDeviceFace
 */
class HobhisNetDeviceFace  : public NetDeviceFace
{
public:

	/**
	 * \brief Get these Face Attributes.
	 *
	 */
	static TypeId
	GetTypeId ();

	/**
	 * \brief Constructor
	 *
	 * @param node Node associated with the face
	 * @param netDevice a smart pointer to NetDevice object to which
	 * This face will be associated.
	 */
	HobhisNetDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice);

	/**
	 * \brief Destructor
	 *
	 */
	virtual ~HobhisNetDeviceFace();

	/**
	 * \brief Get the interest queue length of this Face.
	 *
	 * \return The size of the interest queue of this Face.
	 */
	uint32_t GetQueueLength() {return m_interestQueue.size();};

	/**
	 * \brief No implementation yet.
	 *
	 * \return True if the function is successful else False.
	 */
	bool SetSendingTime(ndn::NameComponents prefix,
			double stime);

	//  void SetBackBW(double bndwth) {m_backBW = bndwth;};

	/**
	 * \brief Check if the Hobhis function is enable in this Face.
	 *
	 * \return True if the Hobhis function is enable; False if the Hobhis function is disable.
	 */
	bool HobhisEnabled() {return m_hobhisEnabled;};

	/**
	 * \brief Check if the node is a client - server or a Intermediate node.
	 *
	 * \return True if the node is a client - server; False if the node is a Intermediate node.
	 */
	bool ClientServer() {return m_client_server;};

	/**
	 * \brief write the band width of a Face in which an Interest who has a specific prefix arrived into a public table: InFaceBWTable.
	 *
	 * This function could be use only once for the same prefix.
	 *
	 * \param prefix The prefix of an Interest.
	 * \parma bw The value of the band width of a Face in which an Interest who has this prefix arrived.
	 */
	void SetInFaceBW(ndn::Name prefix, uint64_t bw);

	/**
	 * \brief Get the band width of a Face in which an Interest who has a specific prefix arrived from a public table: InFaceBWTable.
	 *
	 * \param prefix The prefix of an Interest.
	 *
	 * \return Value of the band width of a Face in which an Interest who has a this prefix arrived from a public table: InFaceBWTable.
	 */
	uint64_t GetInFaceBW(ndn::Name prefix);

	/**
	 * \brief Get the number total of the Interest packet who has a specific prefix in the Interest queue.
	 *
	 * \param prefix The prefix we want to calculate.
	 *
	 * \return The number total of the Interest packet who has this prefix.
	 */
	uint32_t GetIntQueueSizePerFlow(ndn::Name prefix);

	//  void SetFlowNumber(double nflows) {m_Nflows = nflows;};

protected:


	/**
	 * \brief send a packet
	 * There are kinds of cas :
	 * 1. INTEREST PACKET
	 * 	a. server will send the
	 * 	b. client
	 * 	c. another type node
	 * 2. CONTENT PACKET
	 * 	a. server
	 * 	b. client
	 * 	c. another type node
	 *
	 * \param p a packet will be sent, it could be Data or Interest
	 */
	virtual bool
	SendImpl (Ptr<Packet> p);

	Ptr<Face> inFace; ///< \brief no used
private:
	HobhisNetDeviceFace (const HobhisNetDeviceFace &); ///< \brief Disabled copy constructor
	HobhisNetDeviceFace& operator= (const HobhisNetDeviceFace &); ///< \brief Disabled copy operator


	/**
	 * \brief Control sending Interest packet from the Interest queue and if the queue is idle set a OPEN flag for this queue.
	 *
	 * Call by ShaperDequeue() and ShaperSend().
	 *
	 * \see SendImpl(Ptr<Packet> p), ShaperDequeue() and ShaperSend().
	 */
	void ShaperOpen ();

	/**
	 * \brief Send Interest packet from the Interest queue.
	 *
	 * Call by SendImpl(Ptr<Packet> p) and ShaperOpen().
	 *
	 * \see SendImpl(Ptr<Packet> p), ShaperOpen() and ShaperSend().
	 */
	void ShaperDequeue ();

	/**
	 * \brief Call when this Face receive a packet for calculate the average Data size received in this Face.
	 *
	 * \param Device the device associated to this Face.
	 * \param p The packet received in this Face.
	 * \param protocol The protocol of this packet.
	 * \param from The destination of this packet.
	 * \param to The source of this packet.
	 * \param packetType The packet type of this packet.
	 */
	virtual void ReceiveFromNetDevice (Ptr<NetDevice> device,
			Ptr<const Packet> p,
			uint16_t protocol,
			const Address &from,
			const Address &to,
			NetDevice::PacketType packetType);

	/**
	 * \brief Send the Interest packet.
	 *
	 * Call by sendImp() and ShaperDequeue().
	 *
	 * \see SendImpl(Ptr<Packet> p), ShaperOpen() and ShaperDequeue().
	 */
	void ShaperSend();

	/**
	 * \brief Compute the Interests Shaping delay.
	 *
	 * Call by ShaperDequeue().
	 *
	 * \return The delay for sending an Interest.
	 */
	Time ComputeGap();

	std::queue<Ptr<Packet> > m_interestQueue; ///< \brief Interest queue in the face.

	uint32_t m_maxInterest; ///< \brief The max size of interest queue.

	double m_shapingRate; ///< \brief The Interest shaping rate.

	uint64_t m_outBitRate;  ///< \brief The Data out rate in the Face.

	double m_design; ///< \brief The designed parameter related with the shaping converge.

	uint32_t m_target; ///< \brief The target size of the Data queue of others Face.


	bool m_outContentFirst; ///< \brief If this sending Data packet is the first time send.
	double m_outContentSize; ///< \brief The average Data size sent.
	bool m_outInterestFirst; ///< \brief If this sending Interest packet is the first time to send.
	double m_outInterestSize; ///< \brief The average Interest size sent.
	bool m_inContentFirst; ///< \brief If this receiving Data packet is the first time received.
	double m_inContentSize; ///< \brief The average Data size sent.

	//  uint64_t m_backBW;

	///< \brief The enum states for shaper : 1. OPEN 2. BLOCKED.
	enum ShaperState
	{
		OPEN,
		BLOCKED
	};

	ShaperState m_shaperState; ///< \brief The shaper state.

	bool m_hobhisEnabled; ///< \brief If HOBHIS is enable.

	bool m_client_server;  ///< \brief If the node is a client - server or a Intermediate node.

	///no use
	std::map<ndn::Name, bool> m_InterestFirst; ///< \brief No used.

	std::map <ndn::Name, uint32_t> m_nIntQueueSizePerFlow; ///< \brief The number total of the Interest packet who has a specific prefix in the Interest.
	bool m_dynamic_design;///< \brief The average Interest size sent.
};

} // namespace ndn
} // namespace ns3

#endif //NDN_HOBHIS_NET_DEVICE_FACE_H
