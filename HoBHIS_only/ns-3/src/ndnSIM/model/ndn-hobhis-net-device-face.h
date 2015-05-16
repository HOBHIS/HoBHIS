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
 * This class adds interest shaping to NdnNetDeviceFace
 *
 * \see NdnNetDeviceFace
 */
class HobhisNetDeviceFace  : public NetDeviceFace
{
public:
  static TypeId
  GetTypeId ();

  /**
   * \brief Constructor
   *
   * @param node Node associated with the face
   * @param netDevice a smart pointer to NetDevice object to which
   * this face will be associate
   */
  HobhisNetDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice);

  /**
   * \brief Destructor
   *
   */
  virtual ~HobhisNetDeviceFace();

  /**
   * \brief get the current interest queue length
   *
   * \return the size of the m_interestQueue
   */
  uint32_t GetQueueLength() {return m_interestQueue.size();};

  /**
   * \brief no implementation
   */
  bool SetSendingTime(ndn::NameComponents prefix,
		  	  	   	  double stime);

//  void SetBackBW(double bndwth) {m_backBW = bndwth;};

  /**
   * \brief get the value of the m_hobhisEnabled
   * if we set HoBHIS enable
   *
   * \return m_hobHisEnabled
   */
  bool HobhisEnabled() {return m_hobhisEnabled;};

  /**
   * \brief get the value of the m_client_server
   * if the node is client or server, or another
   *
   * \return m_client_server
   */
  bool ClientServer() {return m_client_server;};

  /**
   * \brief set the band width for a interest
   * （first of all, we search the interest in the table of face.
   * If we don't find it, we will insert it.
   * If we find it, we will change the value of the band width）
   *
   * Important! it should be the BW of other hobhis_face, in which a Interest packet with this prefix arrive
   *
   * \param prefix the prefix of a interest
   * \parma bw the value of the band width
   */
  void SetInFaceBW(ndn::Name prefix, uint64_t bw);

  /**
   * \brief get the band width of a interest
   * first of all, we search the interest in the table of face.
   * If we don't find it, we will insert it.
   * If we find it, we will change the value of the band width
   *
   * \param prefix prefix of interest
   *
   * \return value of the band width
   */
  uint64_t GetInFaceBW(ndn::Name prefix);

  /**
   * \brief for each interest, it has its own queue for count the times request
   *
   * \param prefix the prefix of a interest
   *
   * \return the queue of a interest size	（the size of a interest queue）
   */
  uint32_t GetIntQueueSizePerFlow(ndn::Name prefix);

//  void SetFlowNumber(double nflows) {m_Nflows = nflows;};

protected:


  /**
   * \brief send a packet
   * There are kinds of cas :
   * 1. INTEREST PACKET
   * 	a. server
   * 	b. client
   * 	c. another type node
   * 2. CONTENT PACKET
   * 	a. server
   * 	b. client
   * 	c. another type node
   *
   * \param p Ptr<Packet>
   */
  virtual bool
  SendImpl (Ptr<Packet> p);

  ///face of the router
  Ptr<Face> inFace;
private:
  HobhisNetDeviceFace (const HobhisNetDeviceFace &); ///< \brief Disabled copy constructor
  HobhisNetDeviceFace& operator= (const HobhisNetDeviceFace &); ///< \brief Disabled copy operator


  /**
   * \brief open the shaper
   */
  void ShaperOpen ();

  /**
   * \brief set those parameters pour send packet
   * call by ShaperOpen()
   */
  void ShaperDequeue ();

  virtual void ReceiveFromNetDevice (Ptr<NetDevice> device,
                             Ptr<const Packet> p,
                             uint16_t protocol,
                             const Address &from,
                             const Address &to,
                             NetDevice::PacketType packetType);

  /**
   * \brief send the interest, call by sendImp()
   */
  void ShaperSend();

  /**
   * \brief compute the time between two interests sent
   * by compute the m_shappingRate
   * called by ShapperDqueue()
   */
  Time ComputeGap();

  /// interest queue in the face
  std::queue<Ptr<Packet> > m_interestQueue;

  /// the max size of interest queue
  uint32_t m_maxInterest;

  /// the rate of shape
  double m_shapingRate;

  /// the out rate
  /// The Data out rate in one Hobhis_Face
  uint64_t m_outBitRate;

//for shaping rate computation
  double m_design;

  /// the target
  uint32_t m_target;


  /// the first content packet in out_packet_queue, called by sendImp()
  bool m_outContentFirst;
  double m_outContentSize; /// the size of the content packet
  bool m_outInterestFirst; /// the first time to send interest packet
  /// The smoothing size of the out-sending interest packet
  double m_outInterestSize;/// the size of the interest packet

  bool m_inContentFirst; /// the first time to send the content packet
  double m_inContentSize;/// the content packet size

//  uint64_t m_backBW;

  /// enum states for shaper : 1. OPEN 2. BLOCKED
   enum ShaperState
  {
    OPEN,
    BLOCKED
  };

  /// shaper state
  ShaperState m_shaperState;

  /// hobhis enable
  bool m_hobhisEnabled;

  /// node type
  bool m_client_server;

  ///no use
  std::map<ndn::Name, bool> m_InterestFirst;

  ///map for each type of interest in the face
  /// Table for mapping a prefix and the flow number with this prefix (queue size)
  std::map <ndn::Name, uint32_t> m_nIntQueueSizePerFlow;

 // std::map<ndn::Name, uint64_t> m_InFaceBW;
  bool m_dynamic_design;
 };

} // namespace ndn
} // namespace ns3

#endif //NDN_HOBHIS_NET_DEVICE_FACE_H
