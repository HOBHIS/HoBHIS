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

#ifndef NDN_SHR_ENTRY_HPP
#define NDN_SHR_ENTRY_HPP



#include <ostream>

#include "ns3/ndn-name.h"


namespace ns3 {
namespace ndn {

/**
 * \ingroup ndn
 * \brief a entry for saving these informations for shaping
 */
class ShrEntry
{
private:

	double    m_sh_rate;     ///< \brief The shaping rate
	uint32_t  m_q_len;       ///< \brief The queue length per flow
	uint32_t  m_total_q_len; ///< \brief Total queue length
	double    m_rtt;         ///< \brief The RTT
	double    m_bandwidth;   ///< \brief The bandwidth
	bool	  m_fromIRC;	 ///< \brief Indicates that tolrate in this table is the tolrate from IRC packet (the one of bottleneck node)

	uint32_t  m_max_chunks;	 ///< \brief Transmission buffer size to send Chunks


public:
	// Constructors

	/**
	 * \brief Constructor
	 *
	 * \param shrate the shaping rate
	 * \param qlen_flow the queue length per flow
	 * \param qlen total queue length
	 * \param rtt the rtt
	 * \param bw the bandwidth
	 * \param irc Indicates that tolrate in this table is the tolrate from IRC packet
	 * \param max_chunks the max sending Chunks number
	 */
	ShrEntry(double shrate, uint32_t qlen_flow, uint32_t qlen, double rtt, double bw, bool irc, uint32_t max_chunks);

	// Getters/Setters

	/**
	 * \brief get the shaping rate from this entry
	 *
	 * \return the shaping rate from this entry
	 */
	inline double get_sh_rate() const {
		return this->m_sh_rate;
	}

	/**
	 * \brief set the shaping rate to this entry
	 *
	 * \param sh_rate the shaping rate should to be saved into this entry
	 */
	inline void set_sh_rate(double sh_rate) {
		this->m_sh_rate = sh_rate;
	}

	/**
	 * \brief get the queue length for a flow from this entry
	 *
	 * \return the queue length for a flow from this entry
	 */
	inline uint32_t get_queue_length() const {
		return this->m_q_len;
	}

	/**
	 * \brief get the total queue length from this entry
	 *
	 * \return the total queue length from this entry
	 */
	inline uint32_t get_total_queue_length() const {
		return this->m_total_q_len;
	}

	/**
	 * \brief get irc from this entry
	 *
	 * \return irc from this entry
	 */
	inline bool get_irc() const {
		return this->m_fromIRC;
	}

	/**
	 * \brief set the queue length for a flow into this entry
	 *
	 * \param qlen the queue length for a flow should to be saved into this entry
	 */
	inline void set_queue_length(uint32_t qlen) {
		this->m_q_len = qlen;
	}

	/**
	 * \brief set the total queue length into this entry
	 *
	 * \param qlen the total queue length should to be saved into this entry
	 */
	inline void set_total_queue_length(uint32_t qlen) {
		this->m_total_q_len = qlen;
	}

	/**
	 * \brief get rtt from this entry
	 *
	 * \return rtt from this entry
	 */
	inline double get_rtt() const {
		return this->m_rtt;
	}

	/**
	 * \brief set rtt into this entry
	 *
	 * \param rtt the rtt should to be saved into this entry
	 */
	inline void set_rtt(double rtt) {
		this->m_rtt = rtt;
	}

	/**
	 * \brief get bandwith from this entry
	 *
	 * \return bandwith from this entry
	 */
	inline double get_bandwith() const {
		return this->m_bandwidth;
	}

	/**
	 * \brief set bandwidth into this entry
	 *
	 * \param bw the bandwidth should to be saved into this entry
	 */
	inline void set_bandwidth(double bw) {
		this->m_bandwidth = bw;
	}

	/**
	 * \brief set irc into this entry
	 *
	 * \param bw the irc should to be saved into this entry
	 */
	inline void set_irc(bool irc) {
		this->m_fromIRC = irc;
	}

	/**
	 * \brief set the max chunks number into this entry
	 *
	 * \param max_chunks the max chunks number should to be saved into this entry
	 */
	inline void set_max_chunks(uint32_t max_chunks) {
		this->m_max_chunks = max_chunks;
	}

	/**
	 * \brief get max chunks number from this entry
	 *
	 * \return max chunks number from this entry
	 */
	inline uint32_t get_max_chunks() const {
		return this->m_max_chunks;
	}

	/**
	 * \brief operator
	 *
	 */
	ShrEntry & operator = (const ShrEntry & rhs);

	//bool operator < (const ShrEntry & rhs) const;

};

/**
 * \brief Write the content of an ShrEntry instance.
 * \param out The output stream.
 * \param entry The ShrEntry instance we want to write.
 * \param out The updated output stream.
 */
std::ostream & operator << (std::ostream & out, const ShrEntry & entry);

/**
 * \brief Compare two ShrEntry instances
 * \param x The left operand
 * \param y The right operand
 * \param return true if x is less than y
 */
bool operator < (const ShrEntry & x, const ShrEntry & y);
} // end namespace ndn
} // end namespace ns3

#endif
