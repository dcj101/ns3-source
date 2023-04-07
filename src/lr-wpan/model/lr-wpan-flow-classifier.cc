#include "lr-wpan-flow-classifier.h"
#include "ns3/log.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("LrWpanFlowClassifier");


bool operator < (const LrWpanFlowClassifier::TwoTuple &t1,
                 const LrWpanFlowClassifier::TwoTuple &t2)
{
  if (t1.sourceAddress < t2.sourceAddress)
    {
      return true;
    }
  if (t1.sourceAddress != t2.sourceAddress)
    {
      return false;
    }

  if (t1.destinationAddress < t2.destinationAddress)
    {
      return true;
    }
  if (t1.destinationAddress != t2.destinationAddress)
    {
      return false;
    }
  return false;
}

bool operator == (const LrWpanFlowClassifier::TwoTuple &t1,
                  const LrWpanFlowClassifier::TwoTuple &t2)
{
  return (t1.sourceAddress      == t2.sourceAddress &&
          t1.destinationAddress == t2.destinationAddress);
}

LrWpanFlowClassifier::LrWpanFlowClassifier ()
{
}

bool
LrWpanFlowClassifier::Classify (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload,
                              uint32_t *out_flowId, uint32_t *out_packetId)
{
  TwoTuple tuple;
  tuple.sourceAddress = macHeader.GetExtSrcAddr ();
  tuple.destinationAddress = macHeader.GetExtDstAddr ();

  std::pair<std::map<TwoTuple, FlowId>::iterator, bool> insert
    = m_flowMap.insert (std::pair<TwoTuple, FlowId> (tuple, 0));

  if (insert.second)
    {
      FlowId newFlowId = GetNewFlowId ();
      insert.first->second = newFlowId;
      m_flowPktIdMap[newFlowId] = 0;
    }
  else
    {
      m_flowPktIdMap[insert.first->second] ++;
    }

  *out_flowId = insert.first->second;
  *out_packetId = m_flowPktIdMap[*out_flowId];
  return true;
}


LrWpanFlowClassifier::TwoTuple
LrWpanFlowClassifier::FindFlow (FlowId flowId) const
{
  for (std::map<TwoTuple, FlowId>::const_iterator
       iter = m_flowMap.begin (); iter != m_flowMap.end (); iter++)
    {
      if (iter->second == flowId)
        {
          return iter->first;
        }
    }
  NS_FATAL_ERROR ("Could not find the flow with ID " << flowId);
  TwoTuple retval = { Mac64Address(), Mac64Address()};
  return retval;
}

void
LrWpanFlowClassifier::SerializeToXmlStream (std::ostream &os, uint16_t indent) const
{
  Indent (os, indent); os << "<LrWpanFlowClassifier>\n";

  indent += 2;
  for (std::map<TwoTuple, FlowId>::const_iterator
       iter = m_flowMap.begin (); iter != m_flowMap.end (); iter++)
    {
      Indent (os, indent);
      os << "<Flow flowId=\"" << iter->second << "\""
         << " sourceAddress=\"" << iter->first.sourceAddress << "\""
         << " destinationAddress=\"" << iter->first.destinationAddress << "\"";
      Indent (os, indent); os << "</Flow>\n";
    }
  indent -= 2;
  Indent (os, indent); os << "</LrWpanFlowClassifier>\n";
}

}