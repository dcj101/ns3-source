#ifndef LrWpan_FLOW_TAG_H
#define LrWpan_FLOW_TAG_H
#include <ns3/mac64-address.h>
#include "ns3/type-id.h"
#include "ns3/flow-probe.h"
#include "ns3/flow-monitor.h"
#include "lr-wpan-flow-classifier.h"
#include "ns3/tag.h"

namespace ns3
{

class LrWpanFlowProbeTag : public Tag
{
public:

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer buf) const;
  virtual void Deserialize (TagBuffer buf);
  virtual void Print (std::ostream &os) const;
  LrWpanFlowProbeTag ();

  LrWpanFlowProbeTag (uint32_t flowId, uint32_t packetId, uint32_t packetSize, Mac64Address src, Mac64Address dst);

  void SetFlowId (uint32_t flowId);

  void SetPacketId (uint32_t packetId);

  void SetPacketSize (uint32_t packetSize);

  uint32_t GetFlowId (void) const;

  uint32_t GetPacketId (void) const;

  uint32_t GetPacketSize (void) const;

  bool IsSrcDstValid (Mac64Address src, Mac64Address dst) const;
private:
  uint32_t m_flowId;      //!< flow identifier
  uint32_t m_packetId;    //!< packet identifier
  uint32_t m_packetSize;  //!< packet size
  Mac64Address m_src;      //!< IP source
  Mac64Address m_dst;      //!< IP destination
};

}

#endif