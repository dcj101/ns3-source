
#include "lr-wpan-flow-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/lr-wpan-flow-classifier.h"
#include "ns3/lr-wpan-flow-probe.h"
#include "ns3/lr-wpan-mac.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include <iostream>


namespace ns3 {

LrWpanFlowMonitorHelper::LrWpanFlowMonitorHelper ()
{
  m_monitorFactory.SetTypeId ("ns3::FlowMonitor");
  // m_flowMonitor = m_monitorFactory.Create<FlowMonitor> ();

}

LrWpanFlowMonitorHelper::~LrWpanFlowMonitorHelper ()
{
  if (m_flowMonitor)
    {
      m_flowMonitor->Dispose ();
      m_flowMonitor = 0;
      m_flowClassifierLrWpan = 0;
    }
}

void 
LrWpanFlowMonitorHelper::SetMonitorAttribute (std::string n1, const AttributeValue &v1)
{
  m_monitorFactory.Set (n1, v1);
}


Ptr<FlowMonitor>
LrWpanFlowMonitorHelper::GetMonitor ()
{
  if (!m_flowMonitor)
    {
      m_flowMonitor = m_monitorFactory.Create<FlowMonitor> ();
      m_flowClassifierLrWpan = Create<LrWpanFlowClassifier> ();
      m_flowMonitor->AddFlowClassifier (m_flowClassifierLrWpan);
    }
  return m_flowMonitor;
}


Ptr<FlowClassifier>
LrWpanFlowMonitorHelper::GetClassifier ()
{
  if (!m_flowClassifierLrWpan)
    {
      m_flowClassifierLrWpan = Create<LrWpanFlowClassifier> ();
    }
  return m_flowClassifierLrWpan;
}



Ptr<FlowMonitor>
LrWpanFlowMonitorHelper::Install (Ptr<LrWpanNetDevice> device)
{
  Ptr<FlowMonitor> monitor = GetMonitor ();
  Ptr<FlowClassifier> classifier = GetClassifier ();
  Ptr<LrWpanMac> lrWpanMac = device->GetMac();
  if (lrWpanMac)
    {
      Ptr<LrWpanFlowProbe> probe = Create<LrWpanFlowProbe> (monitor,
                                                        DynamicCast<LrWpanFlowClassifier> (classifier),
                                                        device);
    }
  return m_flowMonitor;
}


void
LrWpanFlowMonitorHelper::SerializeToXmlStream (std::ostream &os, uint16_t indent, bool enableHistograms, bool enableProbes)
{
  if (m_flowMonitor)
    {
      m_flowMonitor->SerializeToXmlStream (os, indent, enableHistograms, enableProbes);
    }
}

std::string
LrWpanFlowMonitorHelper::SerializeToXmlString (uint16_t indent, bool enableHistograms, bool enableProbes)
{
  std::ostringstream os;
  if (m_flowMonitor)
    {
      m_flowMonitor->SerializeToXmlStream (os, indent, enableHistograms, enableProbes);
    }
  return os.str ();
}

void
LrWpanFlowMonitorHelper::SerializeToXmlFile (std::string fileName, bool enableHistograms, bool enableProbes)
{
  if (m_flowMonitor)
    {
      m_flowMonitor->SerializeToXmlFile (fileName, enableHistograms, enableProbes);
    }
}


} // namespace ns3
