
#include "lr-wpan-flow-probe.h"
#include "ns3/log.h"

namespace ns3
{


NS_LOG_COMPONENT_DEFINE ("LrWpanFlowProbe");

// ------------------------------------
// ------------------------------------
// -------LrWpanFlowProbe-------------
// -------LrWpanFlowProbe------------------
// ------------------------------------
// ------------------------------------
// ------------------------------------

LrWpanFlowProbe::~LrWpanFlowProbe ()
{

}

LrWpanFlowProbe::LrWpanFlowProbe (Ptr<FlowMonitor> monitor,
                              Ptr<LrWpanFlowClassifier> classifier,
                              Ptr<LrWpanNetDevice> device)
  : FlowProbe (monitor),
    m_classifier (classifier)
{
  NS_LOG_FUNCTION (this);

  m_LrWpanMac = device->GetMac();

  if (!m_LrWpanMac->TraceConnectWithoutContext ("MacSendTrace",
                                           MakeCallback (&LrWpanFlowProbe::SendOutgoingLogger, Ptr<LrWpanFlowProbe> (this))))
    {
      NS_FATAL_ERROR ("trace fail");
    }
  if (!m_LrWpanMac->TraceConnectWithoutContext ("MacLocalDeliverTrace",
                                           MakeCallback (&LrWpanFlowProbe::ForwardUpLogger, Ptr<LrWpanFlowProbe> (this))))
    {
      NS_FATAL_ERROR ("trace fail");
    }
}


void
LrWpanFlowProbe::DoDispose ()
{
  m_LrWpanMac = 0;
  m_classifier = 0;
  FlowProbe::DoDispose ();
}


void
LrWpanFlowProbe::SendOutgoingLogger (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload)
{
  NS_LOG_FUNCTION(this << " ------- >>>");
  FlowId flowId;
  FlowPacketId packetId;
  LrWpanFlowProbeTag fTag;
  bool found = macPayload->FindFirstMatchingByteTag (fTag);
  NS_LOG_FUNCTION(this << " ------- >>>" << found);

  if (found)
    {
      return;
    }

  if (m_classifier->Classify (macHeader, macPayload, &flowId, &packetId))
    {
      uint32_t size = (macPayload->GetSize () + macHeader.GetSerializedSize ());
      NS_LOG_DEBUG ("ReportFirstTx ("<<this<<", "<<flowId<<", "<<packetId<<", "<<size<<"); "
                                     << macHeader.GetExtSrcAddr () << macHeader.GetExtDstAddr () << *macPayload);

      m_flowMonitor->ReportFirstTx (this, flowId, packetId, size);

      LrWpanFlowProbeTag fTag (flowId, packetId, size, macHeader.GetExtSrcAddr (), macHeader.GetExtDstAddr ());
      macPayload->AddByteTag (fTag);
    }
}

void
LrWpanFlowProbe::ForwardUpLogger (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload)
{
  LrWpanFlowProbeTag fTag;
  bool found = macPayload->FindFirstMatchingByteTag (fTag);
  NS_LOG_FUNCTION(this << " found" << found);
  if (found)
    {
      if (!fTag.IsSrcDstValid (macHeader.GetExtSrcAddr (), macHeader.GetExtDstAddr ()))
        {
          NS_LOG_LOGIC ("Not reporting encapsulated packet");
          return;
        }

      FlowId flowId = fTag.GetFlowId ();
      FlowPacketId packetId = fTag.GetPacketId ();

      uint32_t size = (macPayload->GetSize () + macHeader.GetSerializedSize ());
      // NS_LOG_DEBUG ("ReportLastRx ("<<this<<", "<<flowId<<", "<<packetId<<", "<<size<<"); "
      //                                << macHeader << *macPayload);
      m_flowMonitor->ReportLastRx (this, flowId, packetId, size);
    }
}


}   