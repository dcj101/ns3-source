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
 * Author:
 *  kwong yin <kwong-sang.yin@boeing.com>
 *  Sascha Alexander Jopen <jopen@cs.uni-bonn.de>
 *  Alberto Gallegos Ramonet <ramonet@fc.ritsumei.ac.jp>
 */

#ifndef LR_WPAN_CSMACA_H
#define LR_WPAN_CSMACA_H

#include <ns3/object.h>
#include <ns3/event-id.h>
#include <ns3/lr-wpan-mac.h>
#include <ns3/lr-wpan-flow-helper.h>
#include <ns3/lr-wpan-flow-classifier.h>
namespace ns3 {

class UniformRandomVariable;

/**
 * \ingroup lr-wpan
 *
 * This method informs the MAC whether the channel is idle or busy.
 */
typedef Callback<void, LrWpanMacState> LrWpanMacStateCallback;
typedef Callback<uint32_t ,uint32_t , float , float> LrWpanBackOffRlCallback;
/**
 * \ingroup lr-wpan
 *
 * This method informs the transaction cost in a slotted CSMA-CA data transmission.
 * i.e. Reports number of symbols (time) it would take slotted CSMA-CA to process the current transaction.
 * 1 Transaction = 2 CCA + frame transmission (PPDU) + turnaroudtime or Ack time (optional) + IFS
 * See IEEE 802.15.4-2011 (Sections 5.1.1.1 and 5.1.1.4)
 */
typedef Callback<void, uint32_t> LrWpanMacTransCostCallback;
/**
 * \ingroup lr-wpan
 *
 * This class is a helper for the LrWpanMac to manage the Csma/CA
 * state machine according to IEEE 802.15.4-2006, section 7.5.1.4.
 */
class LrWpanCsmaCa : public Object
{

public:
  /**
   * Get the type ID.
   *
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Default constructor.
   */
  LrWpanCsmaCa (void);
  virtual ~LrWpanCsmaCa (void);
  /**
   * Set the MAC to which this CSMA/CA implementation is attached to.
   *
   * \param mac the used MAC
   */
  void SetMac (Ptr<LrWpanMac> mac);
  /**
   * Get the MAC to which this CSMA/CA implementation is attached to.
   *
   * \return the used MAC
   */
  Ptr<LrWpanMac> GetMac (void) const;

  /**
   * Configure for the use of the slotted CSMA/CA version.
   */
  void SetSlottedCsmaCa (void);
  /**
   * Configure for the use of the unslotted CSMA/CA version.
   */
  void SetUnSlottedCsmaCa (void);
  /**
   * Check if the slotted CSMA/CA version is being used.
   *
   * \return true, if slotted CSMA/CA is used, false otherwise.
   */
  bool IsSlottedCsmaCa (void) const;
  /**
   * Check if the unslotted CSMA/CA version is being used.
   *
   * \return true, if unslotted CSMA/CA is used, false otherwise.
   */
  bool IsUnSlottedCsmaCa (void) const;
  /**
   * Set the minimum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \param macMinBE the minimum backoff exponent value
   */
  void SetMacMinBE (uint8_t macMinBE);
  /**
   * Get the minimum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \return the minimum backoff exponent value
   */
  uint8_t GetMacMinBE (void) const;
  /**
   * Set the maximum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \param macMaxBE the maximum backoff exponent value
   */
  void SetMacMaxBE (uint8_t macMaxBE);
  /**
   * Get the maximum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \return the maximum backoff exponent value
   */
  uint8_t GetMacMaxBE (void) const;
  /**
   * Set the maximum number of backoffs.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \param macMaxCSMABackoffs the maximum number of backoffs
   */
  void SetMacMaxCSMABackoffs (uint8_t macMaxCSMABackoffs);

  /**
   * Get the maximum number of backoffs.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \return the maximum number of backoffs
   */
  uint8_t GetMacMaxCSMABackoffs (void) const;
  /**
   * Set the number of symbols forming the basic time period used by the
   * CSMA-CA algorithm.
   * See IEEE 802.15.4-2006, section 7.4.1, Table 85.
   *
   * \param unitBackoffPeriod the period length in symbols
   */
  void SetUnitBackoffPeriod (uint64_t unitBackoffPeriod);
  /**
   * Get the number of symbols forming the basic time period used by the
   * CSMA-CA algorithm.
   * See IEEE 802.15.4-2006, section 7.4.1, Table 85.
   *
   * \return the period length in symbols
   */
  uint64_t GetUnitBackoffPeriod (void) const;
  /**
   * Locates the time to the next backoff period boundary in the SUPERFRAME
   * and returns the amount of time left to this moment.
   *
   * \return time offset to the next slot
   */
  Time GetTimeToNextSlot (void) const;
  /**
   * Start CSMA-CA algorithm (step 1), initialize NB, BE for both slotted and unslotted
   * CSMA-CA. For slotted CSMA-CA initializes CW and starts the backoff slot count.
   */
  void Start (void);
  /**
   * Cancel CSMA-CA algorithm.
   */
  void Cancel (void);
  /**
   * In step 2 of the CSMA-CA, perform a random backoff in the range of 0 to 2^BE -1
   */
  void RandomBackoffDelay (void);
  /*
  *
  */
  void RlBackoffDelay(void);
  /**
   * In the slotted CSMA-CA, after random backoff, determine if the remaining
   * CSMA-CA operation can proceed, i.e. can the entire transactions can be
   * transmitted before the end of the CAP. This step is performed between step
   * 2 and 3. This step is NOT performed for the unslotted CSMA-CA. If it can
   * proceed function RequestCCA() is called.
   */
  void CanProceed (void);
  /**
   * Request the Phy to perform CCA (Step 3)
   */
  void RequestCCA (void);
  /**
   * The CSMA algorithm call this function at the end of the CAP to return the MAC state
   * back to to IDLE after a transmission was deferred due to the lack of time in the CAP.
   */
  void DeferCsmaTimeout (void);
  /**
   *  IEEE 802.15.4-2006 section 6.2.2.2
   *  PLME-CCA.confirm status
   *  @param status TRX_OFF, BUSY or IDLE
   *
   * When Phy has completed CCA, it calls back here which in turn execute the final steps
   * of the CSMA-CA algorithm.
   * It checks to see if the Channel is idle, if so check the Contention window  before
   * permitting transmission (step 5). If channel is busy, either backoff and perform CCA again or
   * treat as channel access failure (step 4).
   */
  void PlmeCcaConfirm (LrWpanPhyEnumeration status);
  /**
   * Set the callback function to report a transaction cost in slotted CSMA-CA. The callback is
   * triggered in CanProceed() after calculating the transaction cost (2 CCA checks,transmission cost, turnAroundTime, ifs)
   * in the boundary of an Active Period.
   *
   * \param trans the transaction cost callback
   */
  void SetLrWpanMacTransCostCallback (LrWpanMacTransCostCallback trans);
  /**
   * Set the callback function to the MAC. Used at the end of a Channel Assessment, as part of the
   * interconnections between the CSMA-CA and the MAC. The callback
   * lets MAC know a channel is either idle or busy.
   *
   * \param macState the mac state callback
   */
  void SetLrWpanMacStateCallback (LrWpanMacStateCallback macState);
  /*
  
  */
  void SetLrWpanNwkBackOffRl(LrWpanBackOffRlCallback rlCb);
  /**
   * Set the value of the Battery Life Extension
   *
   * \param batteryLifeExtension the Battery Life Extension value active or inactive
   */
  void SetBatteryLifeExtension (bool batteryLifeExtension);
  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams that have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);
  /**
   * Get the number of CSMA retries
   *
   * \returns the number of CSMA retries
   */
  uint8_t GetNB (void);
  /**
   * Get the value of the Battery Life Extension
   *
   * \returns  true or false to Battery Life Extension support
   */
  bool GetBatteryLifeExtension (void);

private:
  // Disable implicit copy constructors
  /**
   * \brief Copy constructor - defined and not implemented.
   */
  LrWpanCsmaCa (LrWpanCsmaCa const &);
  /**
   * \brief Copy constructor - defined and not implemented.
   * \returns
   */
  LrWpanCsmaCa& operator= (LrWpanCsmaCa const &);

  virtual void DoDispose (void);
  /**
   *  \brief Get the time left in the CAP portion of the Outgoing or Incoming superframe.
   *  \return the time left in the CAP
   */
  Time GetTimeLeftInCap ();
  /**
   * The callback to inform the cost of a transaction in slotted CSMA-CA.
   */
   /*
   slotted CSMA-CA是一种信道访问方法，它使用超帧结构和二进制指数退避算法来减少碰撞和提高效率。
   回调是在另一个函数执行完毕后执行的函数。
   因此，slotted CSMA-CA中用于通知交易成本的回调可能是一个计算并返回使用该方法发送或接收数据的成本的函数。
   */
  LrWpanMacTransCostCallback m_lrWpanMacTransCostCallback;
  /**
   * The callback to inform the configured MAC of the CSMA/CA result.
   */
  LrWpanMacStateCallback m_lrWpanMacStateCallback;
  /**
   * Beacon-enabled slotted or nonbeacon-enabled unslotted CSMA-CA.
      时序和非时序
   */
  bool m_isSlotted;
  /**
   * The MAC instance for which this CSMA/CA implemenation is configured.
      mac层的配置
   */
  Ptr<LrWpanMac> m_mac;
  /**
   * Number of backoffs for the current transmission.
    当前传输的退避次数
   */
  uint8_t m_NB;
  /**
   * Contention window length (used in slotted ver only).
   CW 定义的是竞争窗口大小，
   表示 CSMA/CA 机制要求接入信道发送报文前需要信道连续空闲的检测次数
   */
  uint8_t m_CW;
  /**
   * Backoff exponent.
   BE 定义的是退避指数，表征着节点竞争信道时随机退避范围的上限值
   */
  uint8_t m_BE;
  /**
   * Battery Life Extension.
   电池寿命延长
   */
  bool m_macBattLifeExt;
  /**
   * Minimum backoff exponent. 0 - macMaxBE, default 3
   最小的退避指数0~ max 默认是3
   */
  uint8_t m_macMinBE;                   //
  /**
   * Maximum backoff exponent. 3 - 8, default 5
   */
  uint8_t m_macMaxBE;
  /**
   * Maximum number of backoffs. 0 - 5, default 4
   最大退避次数 默认4
   */
  uint8_t m_macMaxCSMABackoffs;
  /**
   * Number of symbols per CSMA/CA time unit, default 20 symbols.
   CSMA/CA 时间单位内可以传输的二进制位序列的个数。
   */
  uint64_t m_aUnitBackoffPeriod;
  /**
   * Count the number of remaining random backoff periods left to delay.
   在802.11的退避机制中，为了避免信道冲突，每个站点在发送数据前都要随机选择一个退避时间段，
   这个时间段由一定数量的退避周期组成1。
  在伪随机数生成中，伪随机数周期是指具有周期性循环往复的伪随机数列中不重复伪随机数的个数2。
   在随机过程中，随机退避周期是指一个状态在经历一定次数后再次回到自身的最小次数3。
   */
  uint64_t m_randomBackoffPeriodsLeft;
  /**
   * Uniform random variable stream.
   */
  Ptr<UniformRandomVariable> m_random;
  /**
   * Scheduler event for the start of the next random backoff/slot.
   */
  EventId m_randomBackoffEvent;
  /**
   * Scheduler event for the end of the current CAP
   */
  EventId m_endCapEvent;
  /**
   * Scheduler event when to start the CCA after a random backoff.
   */
  EventId m_requestCcaEvent;
  /**
   * Scheduler event for checking if we can complete the transmission before the
   * end of the CAP.
   */
  EventId m_canProceedEvent;
  /**
   * Flag indicating that the PHY is currently running a CCA. Used to prevent
   * reporting the channel status to the MAC while canceling the CSMA algorithm.
   */
  bool m_ccaRequestRunning;
  /**
   * Indicates whether the CSMA procedure is targeted for a message to be sent to the coordinator.
   * Used to run slotted CSMA/CA on the incoming or outgoing superframe
   * according to the target.
   m_coorDest 是一个布尔变量，用于指示当前 CSMA/CA 过程是为将消息发送到协调器还是为将消息发送到其他设备。
   如果 m_coorDest 为 true，则表明当前 CSMA/CA 过程是为将消息发送到协调器。否则，如果为 false，
   则表明当前 CSMA/CA 过程是为将消息发送到其他设备。
   这个变量的作用是确保设备在进行 CSMA/CA 过程时遵循正确的过程和参数设置。
   */
  bool m_coorDest;

  LrWpanBackOffRlCallback m_backoffRl;

  uint32_t ActionRlBackoff();

};

}

// namespace ns-3

#endif /* LR_WPAN_CSMACA_H */
