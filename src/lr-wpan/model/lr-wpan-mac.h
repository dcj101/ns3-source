/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 The Boeing Company
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
 * Authors:
 *  Gary Pei <guangyu.pei@boeing.com>
 *  kwong yin <kwong-sang.yin@boeing.com>
 *  Tom Henderson <thomas.r.henderson@boeing.com>
 *  Sascha Alexander Jopen <jopen@cs.uni-bonn.de>
 *  Alberto Gallegos Ramonet <ramonet@fc.ritsumei.ac.jp>
 */
#ifndef LR_WPAN_MAC_H
#define LR_WPAN_MAC_H

#include <ns3/object.h>
#include <ns3/traced-callback.h>
#include <ns3/traced-value.h>
#include <ns3/mac16-address.h>
#include <ns3/mac64-address.h>
#include <ns3/sequence-number.h>
#include <ns3/lr-wpan-phy.h>
#include <ns3/lr-wpan-fields.h>
#include <ns3/event-id.h>
#include <deque>
#include "lr-wpan-mac-header.h"


namespace ns3 {

class Packet;
class LrWpanCsmaCa;

/**
 * \defgroup lr-wpan LR-WPAN models
 *
 * This section documents the API of the IEEE 802.15.4-related models.  For a generic functional description, please refer to the ns-3 manual.
 */

/**
 * \ingroup lr-wpan
 *
 * Tx options
 */
typedef enum
{
  TX_OPTION_NONE = 0,    //!< TX_OPTION_NONE
  TX_OPTION_ACK = 1,     //!< TX_OPTION_ACK
  TX_OPTION_GTS = 2,     //!< TX_OPTION_GTS
  TX_OPTION_INDIRECT = 4 //!< TX_OPTION_INDIRECT
} LrWpanTxOption;

/**
 * \ingroup lr-wpan
 *
 * MAC states
 */
typedef enum
{
  MAC_IDLE,              //!< MAC_IDLE
  MAC_CSMA,              //!< MAC_CSMA
  MAC_SENDING,           //!< MAC_SENDING
  MAC_ACK_PENDING,       //!< MAC_ACK_PENDING
  CHANNEL_ACCESS_FAILURE, //!< CHANNEL_ACCESS_FAILURE
  CHANNEL_IDLE,          //!< CHANNEL_IDLE
  SET_PHY_TX_ON,         //!< SET_PHY_TX_ON
  MAC_GTS,               //!< MAC_GTS
  MAC_INACTIVE,          //!< MAC_INACTIVE
  MAC_CSMA_DEFERRED      //!< MAC_CSMA_DEFERRED
} LrWpanMacState;

/**
 * \ingroup lr-wpan
 *
 * Superframe status
 */
typedef enum
{
  BEACON,           //!< The Beacon transmission or reception Period
  CAP,              //!< Contention Access Period
  CFP,              //!< Contention Free Period
  INACTIVE          //!< Inactive Period or unslotted CSMA-CA
} SuperframeStatus;

/**
 * \ingroup lr-wpan
 *
 * Superframe type
 */
typedef enum
{
  OUTGOING = 0,   //!< Outgoing Superframe
  INCOMING = 1    //!< Incoming Superframe
} SuperframeType;


namespace TracedValueCallback {

/**
 * \ingroup lr-wpan
 * TracedValue callback signature for LrWpanMacState.
 *
 * \param [in] oldValue original value of the traced variable
 * \param [in] newValue new value of the traced variable
 */
typedef void (*LrWpanMacState)(LrWpanMacState oldValue,
                               LrWpanMacState newValue);

/**
 * \ingroup lr-wpan
 * TracedValue callback signature for SuperframeStatus.
 *
 * \param [in] oldValue original value of the traced variable
 * \param [in] newValue new value of the traced variable
 */
typedef void (*SuperframeStatus)(SuperframeStatus oldValue,
                                 SuperframeStatus newValue);

}  // namespace TracedValueCallback

/**
 * \ingroup lr-wpan
 *
 * table 80 of 802.15.4
 */
typedef enum
{
  NO_PANID_ADDR = 0,
  ADDR_MODE_RESERVED = 1,
  SHORT_ADDR = 2,
  EXT_ADDR = 3
} LrWpanAddressMode;

/**
 * \ingroup lr-wpan
 *
 * table 83 of 802.15.4
 */
typedef enum
{
  ASSOCIATED = 0,
  PAN_AT_CAPACITY = 1,
  PAN_ACCESS_DENIED = 2,
  ASSOCIATED_WITHOUT_ADDRESS = 0xfe,
  DISASSOCIATED = 0xff
} LrWpanAssociationStatus;

/**
 * \ingroup lr-wpan
 *
 * Table 42 of 802.15.4-2006
 */
typedef enum
{
  IEEE_802_15_4_SUCCESS                = 0,
  IEEE_802_15_4_TRANSACTION_OVERFLOW   = 1,
  IEEE_802_15_4_TRANSACTION_EXPIRED    = 2,
  IEEE_802_15_4_CHANNEL_ACCESS_FAILURE = 3,
  IEEE_802_15_4_INVALID_ADDRESS        = 4,
  IEEE_802_15_4_INVALID_GTS            = 5,
  IEEE_802_15_4_NO_ACK                 = 6,
  IEEE_802_15_4_COUNTER_ERROR          = 7,
  IEEE_802_15_4_FRAME_TOO_LONG         = 8,
  IEEE_802_15_4_UNAVAILABLE_KEY        = 9,
  IEEE_802_15_4_UNSUPPORTED_SECURITY   = 10,
  IEEE_802_15_4_INVALID_PARAMETER      = 11
} LrWpanMcpsDataConfirmStatus;

/**
 * \ingroup lr-wpan
 *
 * Table 35 of IEEE 802.15.4-2011
 */
typedef enum
{
  MLMESTART_SUCCESS                = 0,
  MLMESTART_NO_SHORT_ADDRESS       = 1,
  MLMESTART_SUPERFRAME_OVERLAP     = 2,
  MLMESTART_TRACKING_OFF           = 3,
  MLMESTART_INVALID_PARAMETER      = 4,
  MLMESTART_COUNTER_ERROR          = 5,
  MLMESTART_FRAME_TOO_LONG         = 6,
  MLMESTART_UNAVAILABLE_KEY        = 7,
  MLMESTART_UNSUPPORTED_SECURITY   = 8,
  MLMESTART_CHANNEL_ACCESS_FAILURE = 9
} LrWpanMlmeStartConfirmStatus;

/**
 * \ingroup lr-wpan
 *
 * Table 37 of IEEE 802.15.4-2011
 */
typedef enum
{
  MLMESYNCLOSS_PAN_ID_CONFLICT    = 0,
  MLMESYNCLOSS_REALIGMENT         = 1,
  MLMESYNCLOSS_BEACON_LOST        = 2,
  MLMESYNCLOSS_SUPERFRAME_OVERLAP = 3
} LrWpanSyncLossReason;


/**
 * \ingroup lr-wpan
 *
 * Table 39 of IEEE 802.15.4-2011
 */
typedef enum
{
  MLMEPOLL_SUCCESS                = 0,
  MLMEPOLL_CHANNEL_ACCESS_FAILURE = 2,
  MLMEPOLL_NO_ACK                 = 3,
  MLMEPOLL_NO_DATA                = 4,
  MLMEPOLL_COUNTER_ERROR          = 5,
  MLMEPOLL_FRAME_TOO_LONG         = 6,
  MLMEPOLL_UNAVAILABLE_KEY        = 7,
  MLMEPOLL_UNSUPPORTED_SECURITY   = 8,
  MLMEPOLL_INVALID_PARAMETER      = 9
} LrWpanMlmePollConfirmStatus;

/**
 * \ingroup lr-wpan
 *
 * MCPS-DATA.request params. See 7.1.1.1
 */
struct McpsDataRequestParams
{
  McpsDataRequestParams ()
    : m_srcAddrMode (SHORT_ADDR),
      m_dstAddrMode (SHORT_ADDR),
      m_dstPanId (0),
      m_dstAddr (),
      m_msduHandle (0),
      m_txOptions (0)
  {}
  LrWpanAddressMode m_srcAddrMode; //!< Source address mode
  LrWpanAddressMode m_dstAddrMode; //!< Destination address mode
  uint16_t m_dstPanId;             //!< Destination PAN identifier
  Mac16Address m_dstAddr;          //!< Destination address
  Mac64Address m_dstExtAddr;       //!< Destination extended address
  uint8_t m_msduHandle;            //!< MSDU handle
  uint8_t m_txOptions;             //!< Tx Options (bitfield)
};

/**
 * \ingroup lr-wpan
 *
 * MCPS-DATA.confirm params. See 7.1.1.2
 */
struct McpsDataConfirmParams
{
  uint8_t m_msduHandle; //!< MSDU handle
  LrWpanMcpsDataConfirmStatus m_status; //!< The status of the last MSDU transmission
};
/**
 * \ingroup lr-wpan
 *
 * MCPS-DATA.indication params. See 7.1.1.3
 */
struct McpsDataIndicationParams
{
  uint8_t m_srcAddrMode;      //!< Source address mode
  uint16_t m_srcPanId;        //!< Source PAN identifier
  Mac16Address m_srcAddr;     //!< Source address
  Mac64Address m_srcExtAddr;  //!< Source extended address
  uint8_t m_dstAddrMode;      //!< Destination address mode
  uint16_t m_dstPanId;        //!< Destination PAN identifier
  Mac16Address m_dstAddr;     //!< Destination address
  Mac64Address m_dstExtAddr;  //!< Destination extended address
  uint8_t m_mpduLinkQuality;  //!< LQI value measured during reception of the MPDU
  uint8_t m_dsn;              //!< The DSN of the received data frame
};
/**
 * \ingroup lr-wpan
 *
 * MLME-START.request params. See 802.15.4-2011  Section 6.2.12.1
 MlmeStartRequestParams 是 NS-3 中用于指定 MLME-START.request 命令参数的数据结构。它包含以下成员变量：

m_panId: 网络标识符（PAN）ID。
m_logicalChannel: 逻辑信道号。
m_beaconOrder: 广播信标的周期性发送频率，指数形式。例如，m_beaconOrder 为 2 时，广播信标每 2^2（即 4）个超帧期间发送一次。
m_superframeOrder: 用于指定超帧的长度和类型。例如，m_superframeOrder 为 3 时，超帧的长度为 2^3（即 8）个时间槽，其中 1 个时间槽用于广播信标和GTS，其余时间槽用于数据传输。
m_associationPermit: 指示是否允许关联请求。
这些参数用于配置 802.15.4 网络并开始侦听信道。具体而言，它们将用于发送 MLME-START.request 命令以启动协调器的功能。
 */
struct MlmeStartRequestParams
{
  MlmeStartRequestParams ()
    : m_PanId (0),
      m_logCh (11),
      m_logChPage (0),
      m_startTime (0),
      m_bcnOrd (15),
      m_sfrmOrd (15),
      m_panCoor (false),
      m_battLifeExt (false),
      m_coorRealgn (false)
  {}
  uint16_t m_PanId;                //!< Pan Identifier used by the device.
  uint8_t m_logCh;                 //!< Logical channel on which to start using the new superframe configuration.
  uint32_t m_logChPage;            //!< Logical channel page on which to start using the new superframe configuration.
  uint32_t m_startTime;            //!< Time at which to begin transmitting beacons (Used by Coordinator not PAN Coordinators). The time is specified in symbols.
  uint8_t m_bcnOrd;                //!< Beacon Order, Used to calculate the beacon interval, a value of 15 indicates no periodic beacons will be transmitted.
  uint8_t m_sfrmOrd;               //!< Superframe Order, indicates the length of the CAP in time slots.
  bool m_panCoor;                  //!< On true this device will become coordinator.
  bool m_battLifeExt;              //!< Flag indicating whether or not the Battery life extension (BLE) features are used.
  bool m_coorRealgn;               //!< True if a realignment request command is to be transmitted prior changing the superframe.
};
/**
 * \ingroup lr-wpan
 *
 * MLME-SYNC.request params. See 802.15.4-2011  Section 6.2.13.1
 */
struct MlmeSyncRequestParams
{
  MlmeSyncRequestParams ()
    : m_logCh (),
      m_trackBcn (false)
  {}
  uint8_t m_logCh;                //!< The channel number on which to attempt coordinator synchronization.
  bool m_trackBcn;                //!< True if the mlme sync with the next beacon and attempts to track future beacons. False if mlme sync only the next beacon.
};
/**
 * \ingroup lr-wpan
 *
 * MLME-POLL.request params. See 802.15.4-2011  Section 6.2.14.1
 */
struct MlmePollRequestParams
{
  MlmePollRequestParams ()
    : m_coorAddrMode (SHORT_ADDR),
      m_coorPanId (0),
      m_coorShortAddr (),
      m_coorExtAddr ()
  {}
  LrWpanAddressMode m_coorAddrMode; //!< The addressing mode of the coordinator to which the pool is intended.
  uint16_t m_coorPanId;             //!< The PAN id of the coordinator to which the poll is intended.
  Mac16Address m_coorShortAddr;     //!< Coordintator short address.
  Mac64Address m_coorExtAddr;       //!< Coordinator extended address.


};
/**
 * \ingroup lr-wpan
 *
 * MLME-START.confirm params. See  802.15.4-2011   Section 6.2.12.2
 */
struct MlmeStartConfirmParams
{
  LrWpanMlmeStartConfirmStatus m_status; //!< The status of a MLME-start.request
};
/**
 * \ingroup lr-wpan
 *
 * MLME-BEACON-NOTIFY.indication params. See  802.15.4-2011   Section 6.2.4.1, Table 16
 */
struct MlmeBeaconNotifyIndicationParams
{
  uint8_t m_bsn; //!< The beacon sequence number
  //TODO: Add other params
  //m_PanDesc;
  //m_PenAddrSpec
  //m_sduAddrList
  //m_sduLength
  //m_sdu;
};
/**
 * \ingroup lr-wpan
 *
 * MLME-SYNC-LOSS.indication params. See  802.15.4-2011   Section 6.2.13.2, Table 37
 */
struct MlmeSyncLossIndicationParams
{
  LrWpanSyncLossReason m_lossReason;   //!< The reason for the lost of synchronization.
  uint16_t m_panId;                    //!< The PAN identifier with which the device lost synchronization or to which it was realigned.
  uint8_t  m_logCh;                    //!< The channel number on which the device lost synchronization or to which it was realigned.
};
/**
 * \ingroup lr-wpan
 *
 * MLME-START.confirm params. See  802.15.4-2011   Section 6.2.14.2
 */
struct MlmePollConfirmParams
{
  LrWpanMlmePollConfirmStatus m_status; //!< The confirmation status resulting from a MLME-poll.request.
};
/**
 * \ingroup lr-wpan
 *
 * This callback is called after a McpsDataRequest has been called from
 * the higher layer.  It returns a status of the outcome of the
 * transmission request
 */
typedef Callback<void, McpsDataConfirmParams> McpsDataConfirmCallback;
/**
 * \ingroup lr-wpan
 *
 * This callback is called after a Mcps has successfully received a
 *  frame and wants to deliver it to the higher layer.
 *
 *  \todo for now, we do not deliver all of the parameters in section
 *  802.15.4-2006 7.1.1.3.1 but just send up the packet.
 */
typedef Callback<void, McpsDataIndicationParams, Ptr<Packet> > McpsDataIndicationCallback;
/**
 * \ingroup lr-wpan
 *
 * This callback is called after a MlmeStartRequest has been called from
 * the higher layer.  It returns a status of the outcome of the
 * transmission request
 */
typedef Callback<void, MlmeStartConfirmParams> MlmeStartConfirmCallback;
/**
 * \ingroup lr-wpan
 *
 * This callback is called after a Mlme has successfully received a
 *  beacon frame and wants to deliver it to the higher layer.
 *
 *  \todo for now, we do not deliver all of the parameters in section
 *  802.15.4-2006 6.2.4.1 but just send up the packet.
 */
typedef Callback<void, MlmeBeaconNotifyIndicationParams, Ptr<Packet> > MlmeBeaconNotifyIndicationCallback;
/**
 * \ingroup lr-wpan
 *
 * This callback is called to indicate the loss of synchronization with
 * a coordinator.
 *
 * \todo for now, we do not deliver all of the parameters in section
 *  See IEEE 802.15.4-2011 6.2.13.2.
 */
typedef Callback<void, MlmeSyncLossIndicationParams> MlmeSyncLossIndicationCallback;
/**
 * \ingroup lr-wpan
 *
 * This callback is called after a Mlme-Poll.Request has been called from
 * the higher layer.  It returns a status of the outcome of the
 * transmission request
 */
typedef Callback<void, MlmePollConfirmParams> MlmePollConfirmCallback;
/**
 * \ingroup lr-wpan
 *
 * Class that implements the LR-WPAN MAC state machine
 */
class LrWpanMac : public Object
{
public:
  /**
   * Get the type ID.
   *
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  //MAC sublayer constants
  /**
   * The minimum number of octets added by the MAC sublayer to the PSDU.
   * See IEEE 802.15.4-2011, section 6.4.1, Table 51.
   */
  static const uint32_t aMinMPDUOverhead;
  /**
   * Length of a superframe slot in symbols. Defaults to 60 symbols in each
   * superframe slot.
   * See IEEE 802.15.4-2011, section 6.4.1, Table 51.
   */
  static const uint32_t aBaseSlotDuration;
  /**
   * Number of a superframe slots per superframe. Defaults to 16.
   * See IEEE 802.15.4-2011, section 6.4.1, Table 51.
   */
  static const uint32_t aNumSuperframeSlots;
  /**
   * Length of a superframe in symbols. Defaults to
   * aBaseSlotDuration * aNumSuperframeSlots in symbols.
   * See IEEE 802.15.4-2011, section 6.4.1, Table 51.
   */
  static const uint32_t aBaseSuperframeDuration;
  /**
   * The number of consecutive lost beacons that will
   * cause the MAC sublayer of a receiving device to
   * declare a loss of synchronization.
   * See IEEE 802.15.4-2011, section 6.4.1, Table 51.
   */
  static const uint32_t aMaxLostBeacons;
  /**
   * The maximum size of an MPDU, in octets, that can be
   * followed by a Short InterFrame Spacing (SIFS) period.
   * See IEEE 802.15.4-2011, section 6.4.1, Table 51.
   */
  static const uint32_t aMaxSIFSFrameSize;
  /**
   * Default constructor.
   */
  LrWpanMac (void);
  virtual ~LrWpanMac (void);
  /**
   * Check if the receiver will be enabled when the MAC is idle.
   *
   * \return true, if the receiver is enabled during idle periods, false otherwise
   */
  bool GetRxOnWhenIdle (void);
  /**
   * Set if the receiver should be enabled when the MAC is idle.
   *
   * \param rxOnWhenIdle set to true to enable the receiver during idle periods
   */
  void SetRxOnWhenIdle (bool rxOnWhenIdle);
  // XXX these setters will become obsolete if we use the attribute system
  /**
   * Set the short address of this MAC.
   *
   * \param address the new address
   */
  void SetShortAddress (Mac16Address address);
  /**
   * Get the short address of this MAC.
   *
   * \return the short address
   */
  Mac16Address GetShortAddress (void) const;
  /**
   * Set the extended address of this MAC.
   *
   * \param address the new address
   */
  void SetExtendedAddress (Mac64Address address);
  /**
   * Get the extended address of this MAC.
   *
   * \return the extended address
   */
  Mac64Address GetExtendedAddress (void) const;
  /**
   * Set the PAN id used by this MAC.
   *
   * \param panId the new PAN id.
   */
  void SetPanId (uint16_t panId);
  /**
   * Get the PAN id used by this MAC.
   *
   * \return the PAN id.
   */
  uint16_t GetPanId (void) const;
  /**
   *  IEEE 802.15.4-2006, section 7.1.1.1
   *  MCPS-DATA.request
   *  Request to transfer a MSDU.
   *
   *  \param params the request parameters
   *  \param p the packet to be transmitted
   */
  void McpsDataRequest (McpsDataRequestParams params, Ptr<Packet> p);
  /**
   *  IEEE 802.15.4-2006, section 7.1.14.1
   *  MLME-START.request
   *  Request to allow a PAN coordinator to initiate
   *  a new PAN or beginning a new superframe configuration.
   *
   *  \param params the request parameters
   */
  void MlmeStartRequest (MlmeStartRequestParams params);
  /**
   *  IEEE 802.15.4-2011, section 6.2.13.1
   *  MLME-SYNC.request
   *  Request to synchronize with the coordinator by acquiring and,
   *  if specified, tracking beacons.
   *
   *  \param params the request parameters
   */
  void MlmeSyncRequest (MlmeSyncRequestParams params);
  /**
   *  IEEE 802.15.4-2011, section 6.2.14.2
   *  MLME-POLL.request
   *  Prompts the device to request data from the coordinator.
   *
   *  \param params the request parameters
   */
  void MlmePollRequest (MlmePollRequestParams params);
  /**
   * Set the CSMA/CA implementation to be used by the MAC.
   *
   * \param csmaCa the CSMA/CA implementation
   */
  void SetCsmaCa (Ptr<LrWpanCsmaCa> csmaCa);
  /**
   * Set the underlying PHY for the MAC.
   *
   * \param phy the PHY
   */
  void SetPhy (Ptr<LrWpanPhy> phy);
  /**
   * Get the underlying PHY of the MAC.
   *
   * \return the PHY
   */
  Ptr<LrWpanPhy> GetPhy (void);
  /**
   * Set the callback for the indication of an incoming data packet.
   * The callback implements MCPS-DATA.indication SAP of IEEE 802.15.4-2006,
   * section 7.1.1.3.
   *
   * \param c the callback
   */
  void SetMcpsDataIndicationCallback (McpsDataIndicationCallback c);
  /**
   * Set the callback for the confirmation of a data transmission request.
   * The callback implements MCPS-DATA.confirm SAP of IEEE 802.15.4-2006,
   * section 7.1.1.2.
   *
   * \param c the callback
   */
  void SetMcpsDataConfirmCallback (McpsDataConfirmCallback c);
  /**
    * Set the callback for the confirmation of a data transmission request.
    * The callback implements MLME-START.confirm SAP of IEEE 802.15.4-2006,
    * section 7.1.14.2.
    *
    * \param c the callback
    */
  void SetMlmeStartConfirmCallback (MlmeStartConfirmCallback c);
  /**
   * Set the callback for the indication of an incoming beacon packet.
   * The callback implements MLME-BEACON-NOTIFY.indication SAP of IEEE 802.15.4-2011,
   * section 6.2.4.1.
   *
   * \param c the callback
   */
  void SetMlmeBeaconNotifyIndicationCallback (MlmeBeaconNotifyIndicationCallback c);
  /**
   * Set the callback for the loss of synchronization with a coordinator.
   * The callback implements MLME-BEACON-NOTIFY.indication SAP of IEEE 802.15.4-2011,
   * section 6.2.13.2.
   *
   * \param c the callback
   */
  void SetMlmeSyncLossIndicationCallback (MlmeSyncLossIndicationCallback c);
  /**
    * Set the callback for the confirmation of a data transmission request.
    * The callback implements MLME-POLL.confirm SAP of IEEE 802.15.4-2011,
    * section 6.2.14.2
    *
    * \param c the callback
    */
  void SetMlmePollConfirmCallback (MlmePollConfirmCallback c);

  // interfaces between MAC and PHY

  /**
   *  IEEE 802.15.4-2006 section 6.2.1.3
   *  PD-DATA.indication
   *  Indicates the transfer of an MPDU from PHY to MAC (receiving)
   *  @param psduLength number of bytes in the PSDU
   *  @param p the packet to be transmitted
   *  @param lqi Link quality (LQI) value measured during reception of the PPDU
   */
  void PdDataIndication (uint32_t psduLength, Ptr<Packet> p, uint8_t lqi);
  /**
   *  IEEE 802.15.4-2006 section 6.2.1.2
   *  Confirm the end of transmission of an MPDU to MAC
   *  @param status to report to MAC
   *         PHY PD-DATA.confirm status
   */
  void PdDataConfirm (LrWpanPhyEnumeration status);
  /**
   *  IEEE 802.15.4-2006 section 6.2.2.2
   *  PLME-CCA.confirm status
   *  @param status TRX_OFF, BUSY or IDLE
   */
  void PlmeCcaConfirm (LrWpanPhyEnumeration status);
  /**
   *  IEEE 802.15.4-2006 section 6.2.2.4
   *  PLME-ED.confirm status and energy level
   *  @param status SUCCESS, TRX_OFF or TX_ON
   *  @param energyLevel 0x00-0xff ED level for the channel
   */
  void PlmeEdConfirm (LrWpanPhyEnumeration status, uint8_t energyLevel);
  /**
   *  IEEE 802.15.4-2006 section 6.2.2.6
   *  PLME-GET.confirm
   *  Get attributes per definition from Table 23 in section 6.4.2
   *  @param status SUCCESS or UNSUPPORTED_ATTRIBUTE
   *  @param id the attributed identifier
   *  @param attribute the attribute value
   */
  void PlmeGetAttributeConfirm (LrWpanPhyEnumeration status,
                                LrWpanPibAttributeIdentifier id,
                                LrWpanPhyPibAttributes* attribute);
  /**
   *  IEEE 802.15.4-2006 section 6.2.2.8
   *  PLME-SET-TRX-STATE.confirm
   *  Set PHY state
   *  @param status in RX_ON,TRX_OFF,FORCE_TRX_OFF,TX_ON
   */
  void PlmeSetTRXStateConfirm (LrWpanPhyEnumeration status);
  /**
   *  IEEE 802.15.4-2006 section 6.2.2.10
   *  PLME-SET.confirm
   *  Set attributes per definition from Table 23 in section 6.4.2
   *  @param status SUCCESS, UNSUPPORTED_ATTRIBUTE, INVALID_PARAMETER, or READ_ONLY
   *  @param id the attributed identifier
   */
  void PlmeSetAttributeConfirm (LrWpanPhyEnumeration status,
                                LrWpanPibAttributeIdentifier id);
  /**
   * CSMA-CA algorithm calls back the MAC after executing channel assessment.
   *
   * \param macState indicate BUSY oder IDLE channel condition
   */
  void SetLrWpanMacState (LrWpanMacState macState);
  /**
   * Get the current association status.
   *
   * \return current association status
   */
  LrWpanAssociationStatus GetAssociationStatus (void) const;

  /**
   * Set the current association status.
   *
   * \param status new association status
   */
  void SetAssociationStatus (LrWpanAssociationStatus status);


  //MAC PIB attributes

  /**
   * The time that the device transmitted its last beacon frame.
   * It also indicates the start of the Active Period in the Outgoing superframe.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
  在ns3中，m_macBeaconTxTime是LrWpanMac类的一个私有成员变量，它表示MAC层发送一个beacon帧的时间。该变量的类型是Time，单位是纳秒。LrWpanMac类是IEEE 802.15.4 LR-WPAN MAC协议的ns3实现，它负责处理数据包的传输和管理网络中的节点之间的通信。

在LR-WPAN网络中，beacon帧是由协调器节点定期发送的帧，
用于向所有节点广播网络的参数和状态信息。在LrWpanMac类中，
当该节点被选为协调器时，它将使用m_macBeaconTxTime来计算beacon帧的发送时间，
并将此时间存储在协调器的beacon帧中。

具体来说，当协调器节点收到MlmeStartRequest命令时，
它将使用m_macBeaconTxTime计算出下一个beacon帧的发送时间，
并将此时间存储在协调器的LrWpanMacHeader中。
随后，协调器节点将启动一个计时器来定期发送beacon帧。

在非协调器节点中，
m_macBeaconTxTime主要用于计算下一个beacon帧的接收时间。
当节点接收到beacon帧时，它将从帧中获取beacon帧的发送时间，
并使用m_macBeaconTxTime计算出下一个beacon帧的接收时间。
   */
  Time m_macBeaconTxTime;
  /**
   * The time that the device received its last bit of the beacon frame.
   * It does not indicate the start of the Active Period in the Incoming superframe.
   * Not explicitly listed by the standard but its use is implied.
   * Its purpose is somehow similar to m_macBeaconTxTime
m_macBeaconRxTime 是 ns3 LR-WPAN MAC 类中的一个私有成员变量，用于存储最后一次接收到 MAC Beacon 帧的时间。它的类型是 Time，表示从模拟器开始运行到接收到 Beacon 帧的时间间隔。

在 ns3 LR-WPAN 模块中，设备通过发送和接收 Beacon 帧来同步其时间和在网络中的位置。
m_macBeaconRxTime 变量可以用来计算设备最后一次接收到 Beacon 帧的时间，
进而计算其与网络协调器的时间差。
这个时间差可以用于帮助设备进行网络同步和计算其在网络中的位置。
   */
  Time m_macBeaconRxTime;
  /**
   * The short address of the coordinator through which the device is
   * associated.
   * 0xFFFF indicates this value is unknown.
   * 0xFFFE indicates the coordinator is only using its extended address.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  Mac16Address m_macCoordShortAddress;
  /**
   * The extended address of the coordinator through which the device
   * is associated.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  Mac64Address m_macCoordExtendedAddress;
  /**
   * Symbol boundary is same as m_macBeaconTxTime.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   */
  uint64_t m_macSyncSymbolOffset;
  /**
   * Used by a PAN coordinator or coordinator.
   * Defines how often the coordinator transmits its beacon
   * (outgoing superframe). Range 0 - 15 with 15 meaning no beacons are being sent.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
m_macBeaconOrder是LR-WPAN MAC实体类中的一个私有成员变量，代表超帧中beacon帧的发送间隔。在IEEE 802.15.4规范中，超帧通过在其第一帧（即beacon帧）中的一组超帧规范元素来定义，其中包括超帧持续时间和超帧中beacon帧的发送间隔（beacon order）。m_macBeaconOrder用于指定该设备在超帧中的beacon帧发送间隔，通常表示为macBeaconOrder或BO。

在LR-WPAN网络中，当一个设备成为协调器时，
它将指定一个超帧持续时间和一个beacon帧发送间隔，
其他设备将依据这个间隔来同步它们的时间。
超帧持续时间是指从一个beacon帧的发送时间到下一个beacon帧的发送时间之间的时间间隔。
beacon帧的发送间隔（beacon order）是一个指数值，用于计算beacon帧的发送时间，
其值范围为0到14，表示2的幂次方个超帧期间。
因此，一个beacon order为n的设备每隔2^n个超帧期间发送一个beacon帧，
其他设备通过接收beacon帧来同步它们的时间。
用于划分信道资源和同步设备。超帧被划分为16个大小相等的时隙，
信标帧在每一个超帧的第一个时隙中进行传输。信标的周期是指两个连续信标帧之间的时间间隔，它由m_macBeaconOrder变量来控制。
信标的周期等于16个超帧的长度，也就是
16 * aBaseSuperframeDuration * 2^BO。
   */
  uint8_t m_macBeaconOrder;
  /**
   * Used by a PAN coordinator or coordinator. The length of the active portion
   * of the outgoing superframe, including the beacon frame.
   * 0 - 15 with 15 means the superframe will not be active after the beacon.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
  m_macSuperframeOrder ns3是一个参数，用于控制超帧的活跃期占整个超帧周期的比例。它越大，表示活跃期越短，节省了能耗，但也降低了网络吞吐量123。


m_macSuperframeOrder ns3的功能是用于设置超帧的活跃部分的长度，
从而影响网络的性能和能耗。超帧是一种用于组织信标网络中节点通信的机制，
它由一个活跃期和一个非活跃期组成。在活跃期内，节点可以发送和接收数据，
而在非活跃期内，节点可以进入低功耗模式。
m_macSuperframeOrder ns3是一个参数，用于控制超帧的活跃期占整个超帧周期的比例。
它越大，表示活跃期越短，节省了能耗，但也降低了网络吞吐量 。
   */
  uint8_t m_macSuperframeOrder;
  /**
   * The maximum time (in superframe periods) that a transaction is stored by a
   * coordinator and indicated in its beacon.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  uint16_t m_macTransactionPersistanceTime;
  /**
   * The total size of the received beacon in symbols.
   * Its value is used to calculate the end CAP time of the incoming superframe.
   */
  uint64_t m_rxBeaconSymbols;
  /**
   * Indication of the Slot where the CAP portion of the OUTGOING Superframe ends.
   */
  uint8_t m_fnlCapSlot;
  /**
   * The beaconOrder value of the INCOMING frame. Used by all devices that have a parent.
   * Specification of how often the parent coordinator transmits its beacon.
   * 0 - 15 with 15 means the parent is not currently transmitting beacons.
   */
  uint8_t m_incomingBeaconOrder;
  /**
   * Used by all devices that have a parent.
   * The length of the active portion of the INCOMING superframe, including the
   * beacon frame.
   * 0 - 15 with 15 meaning the superframe will not be active after the beacon.
   */
  uint8_t m_incomingSuperframeOrder;
  /**
   * Indication of the Slot where the CAP portion of the INCOMING Superframe ends.
   */
  uint8_t m_incomingFnlCapSlot;
  /**
   * Indicates if MAC sublayer is in receive all mode. True mean accept all
   * frames from PHY.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   */
  bool m_macPromiscuousMode;
  /**
   * 16 bits id of PAN on which this device is operating. 0xffff means not
   * associated.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   */
  uint16_t m_macPanId;
  /**
   * Sequence number added to transmitted data or MAC command frame, 00-ff.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   */
  SequenceNumber8 m_macDsn;
  /**
   * Sequence number added to transmitted beacon frame, 00-ff.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  SequenceNumber8 m_macBsn;
  /**
   * The maximum number of retries allowed after a transmission failure.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   */
  uint8_t m_macMaxFrameRetries;
  /**
   * Indication of whether the MAC sublayer is to enable its receiver during
   * idle periods.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   */
  bool m_macRxOnWhenIdle;
  /**
   * The minimum time forming a Long InterFrame Spacing (LIFS) period.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  uint32_t m_macLIFSPeriod;
  /**
   * The minimum time forming a Short InterFrame Spacing (SIFS) period.
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  uint32_t m_macSIFSPeriod;
  /**
   * Indication of whether a device automatically sends data request command
   * if its address is listed in the beacon frame.
   * TRUE = request command automatically is sent. This command also affects
   * the generation of MLME-BEACON-NOTIFY.indication (6.2.4.1)
   * See IEEE 802.15.4-2011, section 6.4.2, Table 52.
   */
  bool m_macAutoRequest;
  /**
   * The value of the necessary InterFrame Space after the transmission of a packet.
   */
  uint32_t m_ifs;
  /**
   * Indication of whether the current device is the PAN coordinator
   */
  bool m_panCoor;
  /**
   * Indication of the Interval used by the coordinator to transmit beacon frames
   * expressed in symbols.
   */
  uint32_t m_beaconInterval;
  /**
   * Indication of the superframe duration in symbols.
   * (e.g. 1 symbol = 4 bits in a 250kbps O-QPSK PHY)
   */
  uint32_t m_superframeDuration;
  /**
   * Indication of the interval a node should receive a superframe
   * expressed in symbols.
   */
  uint32_t m_incomingBeaconInterval;
  /**
   * Indication of the superframe duration in symbols
   * (e.g. 1 symbol = 4 bits in a 250kbps O-QPSK PHY)
   */
  uint32_t m_incomingSuperframeDuration;
  /**
   * Indication of current device capability (FFD or RFD)
   */
  uint8_t m_deviceCapability;
  /**
   * Indication of whether the current device is tracking incoming beacons.
   */
  bool m_beaconTrackingOn;
  /**
   * The number of consecutive loss beacons in a beacon tracking operation.
   */
  uint8_t  m_numLostBeacons;
  /**
   * Get the macAckWaitDuration attribute value.
   *
   * \return the maximum number symbols to wait for an acknowledgment frame
   */
  uint64_t GetMacAckWaitDuration (void) const;
  /**
   * Get the macMaxFrameRetries attribute value.
   *
   * \return the maximum number of retries
   */
  uint8_t GetMacMaxFrameRetries (void) const;
  /**
   * Print the number of elements in the packet transmit queue.
   */
  void PrintTransmitQueueSize (void);
  /**
   * Set the macMaxFrameRetries attribute value.
   *
   * \param retries the maximum number of retries
   */
  void SetMacMaxFrameRetries (uint8_t retries);
  /**
   * Check if the packet destination is its coordinator
   *
   * \return True if m_txPkt (packet awaiting to be sent) destination is its coordinator
   */
  bool isCoordDest (void);
  /**
   * Check if the packet destination is its coordinator
   *
   *\param mac The coordinator short MAC Address
   */
  void SetAssociatedCoor (Mac16Address mac);


  Mac16Address GetAssociatedMac16AddressCoor();
  /**
   * Check if the packet destination is its coordinator
   *
   *\param mac The coordinator extended MAC Address
   */
  void SetAssociatedCoor (Mac64Address mac);



  Mac64Address GetAssociatedMac64AddressCoor();
  
  
  

  /**
   * Get the size of the Interframe Space according to MPDU size (m_txPkt).
   *
   * \return the IFS size in symbols
   */
  uint32_t GetIfsSize ();
  /**
   * Obtain the number of symbols in the packet which is currently being sent by the MAC layer.
   *
   *\return packet number of symbols
   * */
  uint64_t GetTxPacketSymbols (void);
  /**
   * Check if the packet to transmit requires acknowledgment
   *
   *\return True if the Tx packet requires acknowledgment
   * */
  bool isTxAckReq (void);
  /**
   * TracedCallback signature for sent packets.
   *
   * \param [in] packet The packet.
   * \param [in] retries The number of retries.
   * \param [in] backoffs The number of CSMA backoffs.
   */
  typedef void (*SentTracedCallback)(Ptr<const Packet> packet, uint8_t retries, uint8_t backoffs);

  /**
   * TracedCallback signature for LrWpanMacState change events.
   *
   * \param [in] oldValue The original state value.
   * \param [in] newValue The new state value.
   * \deprecated The LrWpanMacState is now accessible as the
   * TracedValue \c MacStateValue. The \c MacState TracedCallback will
   * be removed in a future release.
   */
  typedef void (*StateTracedCallback)(LrWpanMacState oldState, LrWpanMacState newState);

protected:
  // Inherited from Object.
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

private:
  /**
   * Helper structure for managing transmission queue elements.
   */
  struct TxQueueElement
  {
    uint8_t txQMsduHandle; //!< MSDU Handle
    Ptr<Packet> txQPkt;    //!< Queued packet

  };
  /**
   * Helper structure for managing indirect transmission queue elements.
   */
  struct IndTxQueueElement
  {
    uint8_t txQMsduHandle; //!< MSDU Handle.
    Ptr<Packet> txQPkt;    //!< Queued packet.
    Time expireTime;       //!< The expiration time of the packet in the indirect transmission queue.
  };
  /**
   * Called to send a single beacon frame.
   */
  void SendOneBeacon (void);
  /**
   * Called to begin the Contention Free Period (CFP) in a
   * beacon-enabled mode.
   *
   * \param superframeType The incoming or outgoing superframe reference
   */
  void StartCFP (SuperframeType superframeType);
  /**
   * Called to begin the Contention Access Period (CAP) in a
   * beacon-enabled mode.
   *
   * \param superframeType The incoming or outgoing superframe reference
   */
  void StartCAP (SuperframeType superframeType);
  /**
   * Start the Inactive Period in a beacon-enabled mode.
   *
   * \param superframeType The incoming or outgoing superframe reference
   *
   */
  void StartInactivePeriod (SuperframeType superframeType);
  /**
   * Called after the end of an INCOMING superframe to start the moment a
   * device waits for a new incoming beacon.
   */
  void AwaitBeacon (void);
  /**
   * Called if the device is unable to locate a beacon in the time set by MLME-SYNC.request.
   */
  void BeaconSearchTimeout (void);
  /**
   * Send an acknowledgment packet for the given sequence number.
   *
   * \param seqno the sequence number for the ACK
   */
  void SendAck (uint8_t seqno);
  /**
   * Remove the tip of the transmission queue, including clean up related to the
   * last packet transmission.
   */
  void RemoveFirstTxQElement ();
  /**
   * Change the current MAC state to the given new state.
   *
   * \param newState the new state
   */
  void ChangeMacState (LrWpanMacState newState);
  /**
   * Handle an ACK timeout with a packet retransmission, if there are
   * retransmission left, or a packet drop.
   */
  void AckWaitTimeout (void);
  /**
   * After a successful transmission of a frame (beacon, data) or an ack frame reception,
   * the mac layer wait an Interframe Space (IFS) time and triggers this function
   * to continue with the MAC flow.
   *
   * \param ifsTime IFS time
   */
  void IfsWaitTimeout (Time ifsTime);
  /**
   * Check for remaining retransmissions for the packet currently being sent.
   * Drop the packet, if there are no retransmissions left.
   *
   * \return true, if the packet should be retransmitted, false otherwise.
   */
  bool PrepareRetransmission (void);
  /**
   * Check the transmission queue. If there are packets in the transmission
   * queue and the MAC is idle, pick the first one and initiate a packet
   * transmission.
   */
  void CheckQueue (void);
  /**
   * Constructs a Superframe specification field from the local information,
   * the superframe Specification field is necessary to create a beacon frame.
   *
   * \returns the Superframe specification field
   */
  SuperframeField GetSuperframeField (void);
  /**
   * Constructs the Guaranteed Time Slots (GTS) Fields from local information.
   * The GTS Fields are part of the beacon frame.
   *
   * \returns the Guaranteed Time Slots (GTS) Fields
   */
  GtsFields GetGtsFields (void);
  /**
   * Constructs Pending Address Fields from the local information,
   * the Pending Address Fields are part of the beacon frame.
   *
   * \returns the Pending Address Fields
   */
  PendingAddrFields GetPendingAddrFields (void);

  /**
   * The trace source is fired at the end of any Interframe Space (IFS).
   */
  TracedCallback<Time> m_macIfsEndTrace;
  /**
   * The trace source fired when packets are considered as successfully sent
   * or the transmission has been given up.
   * Only non-broadcast packets are traced.
   *
   * The data should represent:
   * packet, number of retries, total number of csma backoffs
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, uint8_t, uint8_t > m_sentPktTrace;
  /**
   * The trace source fired when packets come into the "top" of the device
   * at the L3/L2 transition, when being queued for transmission.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macTxEnqueueTrace;
  /**
   * The trace source fired when packets are dequeued from the
   * L3/l2 transmission queue.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macTxDequeueTrace;
  /**
   * The trace source fired when packets are being sent down to L1.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macTxTrace;
  /**
   * The trace source fired when packets where successfully transmitted, that is
   * an acknowledgment was received, if requested, or the packet was
   * successfully sent by L1, if no ACK was requested.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macTxOkTrace;
  /**
   * The trace source fired when packets are dropped due to missing ACKs or
   * because of transmission failures in L1.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macTxDropTrace;
  /**
   * The trace source fired for packets successfully received by the device
   * immediately before being forwarded up to higher layers (at the L2/L3
   * transition).  This is a promiscuous trace.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macPromiscRxTrace;
  /**
   * The trace source fired for packets successfully received by the device
   * immediately before being forwarded up to higher layers (at the L2/L3
   * transition).  This is a non-promiscuous trace.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macRxTrace;
  /**
   * The trace source fired for packets successfully received by the device
   * but dropped before being forwarded up to higher layers (at the L2/L3
   * transition).
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_macRxDropTrace;
  /**
   * A trace source that emulates a non-promiscuous protocol sniffer connected
   * to the device.  Unlike your average everyday sniffer, this trace source
   * will not fire on PACKET_OTHERHOST events.
   *
   * On the transmit size, this trace hook will fire after a packet is dequeued
   * from the device queue for transmission.  In Linux, for example, this would
   * correspond to the point just before a device hard_start_xmit where
   * dev_queue_xmit_nit is called to dispatch the packet to the PF_PACKET
   * ETH_P_ALL handlers.
   *
   * On the receive side, this trace hook will fire when a packet is received,
   * just before the receive callback is executed.  In Linux, for example,
   * this would correspond to the point at which the packet is dispatched to
   * packet sniffers in netif_receive_skb.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_snifferTrace;

  /**
   * A trace source that emulates a promiscuous mode protocol sniffer connected
   * to the device.  This trace source fire on packets destined for any host
   * just like your average everyday packet sniffer.
   *
   * On the transmit size, this trace hook will fire after a packet is dequeued
   * from the device queue for transmission.  In Linux, for example, this would
   * correspond to the point just before a device hard_start_xmit where
   * dev_queue_xmit_nit is called to dispatch the packet to the PF_PACKET
   * ETH_P_ALL handlers.
   *
   * On the receive side, this trace hook will fire when a packet is received,
   * just before the receive callback is executed.  In Linux, for example,
   * this would correspond to the point at which the packet is dispatched to
   * packet sniffers in netif_receive_skb.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_promiscSnifferTrace;
  /**
   * A trace source that fires when the LrWpanMac changes states.
   * Parameters are the old mac state and the new mac state.
   *
   * \deprecated This TracedCallback is deprecated and will be
   * removed in a future release,  Instead, use the \c MacStateValue
   * TracedValue.
   */
  TracedCallback<LrWpanMacState, LrWpanMacState> m_macStateLogger;
  /**
   * The PHY associated with this MAC.
   */
  Ptr<LrWpanPhy> m_phy;
  /**
   * The CSMA/CA implementation used by this MAC.
   */
  Ptr<LrWpanCsmaCa> m_csmaCa;
  /**
   * This callback is used to notify incoming beacon packets to the upper layers.
   * See IEEE 802.15.4-2011, section 6.2.4.1.
   */
  MlmeBeaconNotifyIndicationCallback m_mlmeBeaconNotifyIndicationCallback;
  /**
   * This callback is used to indicate the loss of synchronization with a coordinator.
   * See IEEE 802.15.4-2011, section 6.2.13.2.
   */
  MlmeSyncLossIndicationCallback m_mlmeSyncLossIndicationCallback;
  /**
   * This callback is used to report the status after a device send data command request to
   * the coordinator to transmit data.
   * See IEEE 802.15.4-2011, section 6.2.14.2.
   */
  MlmePollConfirmCallback m_mlmePollConfirmCallback;
  /**
   * This callback is used to report the start of a new PAN or
   * the begin of a new superframe configuration.
   * See IEEE 802.15.4-2006, section 7.1.14.2.
   */
  MlmeStartConfirmCallback m_mlmeStartConfirmCallback;
  /**
   * This callback is used to notify incoming packets to the upper layers.
   * See IEEE 802.15.4-2006, section 7.1.1.3.
   */
  McpsDataIndicationCallback m_mcpsDataIndicationCallback;
  /**
   * This callback is used to report data transmission request status to the
   * upper layers.
   * See IEEE 802.15.4-2006, section 7.1.1.2.
   */
  McpsDataConfirmCallback m_mcpsDataConfirmCallback;
  /**
   * The current state of the MAC layer.
   */
  TracedValue<LrWpanMacState> m_lrWpanMacState;
  /**
   * The current period of the incoming superframe.
   */
  TracedValue<SuperframeStatus> m_incSuperframeStatus;
  /**
   * The current period of the outgoing superframe.
   */
  TracedValue<SuperframeStatus> m_outSuperframeStatus;
  /**
   * The current association status of the MAC layer.
   */
  LrWpanAssociationStatus m_associationStatus;
  /**
   * The packet which is currently being sent by the MAC layer.
   */
  Ptr<Packet> m_txPkt;  // XXX need packet buffer instead of single packet
  /**
   * The short address used by this MAC. Currently we do not have complete
   * extended address support in the MAC, nor do we have the association
   * primitives, so this address has to be configured manually.
   */
  Mac16Address m_shortAddress;
  /**
   * The extended address used by this MAC. Extended addresses are currently not
   * really supported.
   */
  Mac64Address m_selfExt;
  /**
   * The transmit queue used by the MAC.
   */
  std::deque<TxQueueElement*> m_txQueue;
  /**
   * The indirect transmit queue used by the MAC pending messages.
   */
  std::deque<IndTxQueueElement*> m_indTxQueue;
  /**
   * The number of already used retransmission for the currently transmitted
   * packet.
   */
  uint8_t m_retransmission;
  /**
   * The number of CSMA/CA retries used for sending the current packet.
   */
  uint8_t m_numCsmacaRetry;
  /**
   * Scheduler event for the ACK timeout of the currently transmitted data
   * packet.
   */
  EventId m_ackWaitTimeout;
  /**
   * Scheduler event for a deferred MAC state change.
   */
  EventId m_setMacState;
  /**
   * Scheduler event for Interframe spacing wait time.
   */
  EventId m_ifsEvent;
  /**
   * Scheduler event for generation of one beacon.
   */
  EventId m_beaconEvent;
  /**
   * Scheduler event for the end of the outgoing superframe CAP.
   **/
  EventId m_capEvent;
  /**
   * Scheduler event for the end of the outgoing superframe CFP.
   */
  EventId m_cfpEvent;
  /**
   * Scheduler event for the end of the incoming superframe CAP.
   **/
  EventId m_incCapEvent;
  /**
   * Scheduler event for the end of the incoming superframe CFP.
   */
  EventId m_incCfpEvent;
  /**
   * Scheduler event to track the incoming beacons.
   */
  EventId m_trackingEvent;

      // track
    TracedCallback<const LrWpanMacHeader&, Ptr<const Packet> > m_sendTrace;
    /// Trace of locally delivered packets
    TracedCallback<const LrWpanMacHeader&, Ptr<const Packet> > m_localDeliverTrace;

};
} // namespace ns3

#endif /* LR_WPAN_MAC_H */
