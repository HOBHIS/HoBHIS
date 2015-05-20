/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 University of Washington
 *
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
 * Natalya Rozhnova, <natalya.rozhnova@lip6.fr>, UPMC, Paris, France
 */

#ifndef NDNDROPTAIL_H
#define NDNDROPTAIL_H

#include <queue>
#include <map>
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/ndn-name.h"

namespace ns3 {
namespace ndn{


class TraceContainer;

/**
 * \ingroup queue
 *
 * \brief A FIFO packet queue that drops tail-end packets on overflow
 * This is for the content packet. if the queue is filled, we delete the fires packet who arrive
 * in the first time and insert the new content packet in the last place of the queue.
 */
class NDNDropTailQueue : public Queue {
public:

	/**
	 * \brief get the the type ID
	 *
	 * \return TypeID
	 */
	static TypeId GetTypeId (void);

	/**
	 * \brief NDNDropTailQueue Constructor
	 *
	 * Creates a droptail queue with a maximum size of 100 packets by default
	 */
	NDNDropTailQueue ();
	/**
	 * \brief NDNDropTailQueue Destructor
	 *
	 */
	virtual ~NDNDropTailQueue();

	/**
	 * Set the operating mode of this queue: QUEUE_MODE_PACKETS or QUEUE_MODE_BYTES
	 *
	 * \param mode The operating mode of this queue
	 */
	void SetMode (NDNDropTailQueue::QueueMode mode);

	/**
	 * Get the encapsulation mode of this queue: QUEUE_MODE_PACKETS or QUEUE_MODE_BYTES
	 *
	 * \returns The encapsulation mode of this queue
	 */
	NDNDropTailQueue::QueueMode GetMode (void);

	/**
	 * \brief Get the queue length table by flow type constant
	 *
	 * \return the queue length table by flow type constant
	 */
	inline const std::map<ndn::Name, uint32_t> & GetQLengthPerFlow() const {return this->m_nQueueSizePerFlow;}

	/**
	 * \brief Get the queue length table by flow
	 *
	 * \return the queue length table by flow
	 */
	inline std::map<ndn::Name, uint32_t> & GetQLengthPerFlow() {return this->m_nQueueSizePerFlow;}

	/**
	 * \brief print the information of each flow in this queue
	 */
	void PrintQueueSizePerFlow();

	/**
	 * \brief get the number of the packet by the prefix in this queue
	 *
	 * \param prefix of a flow
	 *
	 * \return the number of the packet by the prefix in this queue
	 */
	uint32_t GetQueueSizePerFlow(ndn::Name prefix);

	/**
	 * \brief Get the total content packet number in this queue
	 *
	 * \param prefix of a flow
	 *
	 * \return the amount of packet
	 */
	uint32_t GetDataQueueLength() {return m_packets.size();};

	/**
	 * \brief Get the amount of the flow
	 *
	 * the amount of the flow
	 *
	 * \return the amount of the flow
	 */
	double GetFlowNumber();

	/**
	 * \brief Get the max Data packet number could be accepted in this queue
	 *
	 * \return the max Data packet number could be accepted in this queue
	 */
	uint32_t GetMaxChunks() const {return this->m_maxPackets;};

private:
	/**
	 * \brief insert a packet, if the queue is filled, just drop the packet
	 *
	 * \param a packet
	 */
	virtual bool DoEnqueue (Ptr<Packet> p);

	/**
	 * \brief just delete the first element in the total queue and delete it also
	 * in the flow
	 */
	virtual Ptr<Packet> DoDequeue (void);

	/**
	 * \brief show the packet, queue state
	 *
	 */
	virtual Ptr<const Packet> DoPeek (void) const;

	std::queue<Ptr<Packet> > m_packets; /// packet queue

	uint32_t m_maxPackets; ///< the maximum total packet number in the queue, used in the mode packet
	uint32_t m_maxBytes; ///< the maximum bytes in the queue, used in the mode byte
	uint32_t m_bytesInQueue; ///< total bytes in the queue
	QueueMode m_mode; ///< mode of the queue : QUEUE_MODE_PACKETS or QUEUE_MODE_BYTES
	std::map <ndn::Name, uint32_t> m_nQueueSizePerFlow; ///< the table for the queue size by each flow

};

} // namespace ns3
}
#endif /* DROPTAIL_H */
