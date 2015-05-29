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


#ifndef NDN_SEND_TIME_ENTRY_HPP
#define NDN_SEND_TIME_ENTRY_HPP



#include <ostream>

#include "ns3/ndn-name.h"


namespace ns3 {
namespace ndn {


/**
 * \ingroup ndn
 * \brief a entry for saving these sending times
 *
 */
class STimeEntry
{

private:

	double m_stime; ///< \brief packet sending time


public:
	// Constructors

	/**
	 * \brief Constructor.
	 *
	 * \param stime the Interest sending time
	 */
	STimeEntry(double stime);

	// Getters/Setters

	/**
	 * \brief get the sending time from this entry
	 *
	 * \return the sending time from this entry
	 */
	inline double get_send_time() const {
		return this->m_stime;
	}

	/**
	 * \brief set the sending time to this entry
	 *
	 * \param stime the sending time should to be saved into this entry
	 */
	inline void set_send_time(double stime) {
		this->m_stime = stime;
	}

	/**
	 * \brief operator
	 *
	 */
	STimeEntry & operator = (const STimeEntry & rhs);

	//bool operator < (const STimeEntry & rhs) const;

};

/**
 * \brief Write the content of an STimeEntry instance
 *
 * \param out The output stream or updated
 * \param entry The STimeEntry instance we want to write
 */
std::ostream & operator << (std::ostream & out, const STimeEntry & entry);

/**
 * \brief Compare two STimeEntry instances
 *
 * \param x The left operand
 * \param y The right operand
 * \return True if x is less than y, else False
 */
bool operator < (const STimeEntry & x, const STimeEntry & y);
} // end namespace ndn
} // end namespace ns3

#endif
