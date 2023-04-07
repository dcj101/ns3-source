#include "lr-wpan-flow-probe-tag.h"

namespace ns3
{

TypeId 
LrWpanFlowProbeTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LrWpanFlowProbeTag")
    .SetParent<Tag> ()
    .SetGroupName ("FlowMonitor")
    .AddConstructor<LrWpanFlowProbeTag> ()
  ;
  return tid;
}
TypeId 
LrWpanFlowProbeTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
LrWpanFlowProbeTag::GetSerializedSize (void) const
{
  return 4 + 4 + 4 + 8 + 8;
}
void 
LrWpanFlowProbeTag::Serialize (TagBuffer buf) const
{
  buf.WriteU32 (m_flowId);
  buf.WriteU32 (m_packetId);
  buf.WriteU32 (m_packetSize);
  uint8_t data[8];

  m_src.CopyTo(data);
  buf.Write(data,8);

  m_dst.CopyTo(data);
  buf.Write(data,8);
}
void 
LrWpanFlowProbeTag::Deserialize (TagBuffer buf)
{
  m_flowId = buf.ReadU32 ();
  m_packetId = buf.ReadU32 ();
  m_packetSize = buf.ReadU32 ();
  uint8_t data[8];

  buf.Read(data,8);
  m_src.CopyFrom(data);

  buf.Read(data,8);
  m_dst.CopyFrom(data);
}
void 
LrWpanFlowProbeTag::Print (std::ostream &os) const
{
  os << "FlowId=" << m_flowId;
  os << " PacketId=" << m_packetId;
  os << " PacketSize=" << m_packetSize;
}
LrWpanFlowProbeTag::LrWpanFlowProbeTag ()
  : Tag () 
{
}

LrWpanFlowProbeTag::LrWpanFlowProbeTag (uint32_t flowId, uint32_t packetId, uint32_t packetSize, Mac64Address src, Mac64Address dst)
  : Tag (), m_flowId (flowId), m_packetId (packetId), m_packetSize (packetSize), m_src (src), m_dst (dst)
{
}

void
LrWpanFlowProbeTag::SetFlowId (uint32_t id)
{
  m_flowId = id;
}
void
LrWpanFlowProbeTag::SetPacketId (uint32_t id)
{
  m_packetId = id;
}
void
LrWpanFlowProbeTag::SetPacketSize (uint32_t size)
{
  m_packetSize = size;
}
uint32_t
LrWpanFlowProbeTag::GetFlowId (void) const
{
  return m_flowId;
}
uint32_t
LrWpanFlowProbeTag::GetPacketId (void) const
{
  return m_packetId;
} 
uint32_t
LrWpanFlowProbeTag::GetPacketSize (void) const
{
  return m_packetSize;
}
bool
LrWpanFlowProbeTag::IsSrcDstValid (Mac64Address src, Mac64Address dst) const
{
  return ((m_src == src) && (m_dst == dst));
}

}