
#ifndef LrWpan_FLOW_MONITOR_HELPER_H
#define LrWpan_FLOW_MONITOR_HELPER_H

#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-classifier.h"
#include "ns3/singleton.h"
#include <string>
#include "lr-wpan-net-device.h"

namespace ns3 {

class AttributeValue;
class LrWpanFlowClassifier;

class LrWpanFlowMonitorHelper : public Singleton<LrWpanFlowMonitorHelper>
{
public:

  LrWpanFlowMonitorHelper ();
  ~LrWpanFlowMonitorHelper ();

  void SetMonitorAttribute (std::string n1, const AttributeValue &v1);

  Ptr<FlowMonitor> Install (Ptr<LrWpanNetDevice> device);

  Ptr<FlowMonitor> GetMonitor ();

  Ptr<FlowClassifier> GetClassifier ();

  void SerializeToXmlStream (std::ostream &os, uint16_t indent, bool enableHistograms, bool enableProbes);

  std::string SerializeToXmlString (uint16_t indent, bool enableHistograms, bool enableProbes);

  void SerializeToXmlFile (std::string fileName, bool enableHistograms, bool enableProbes);

private:
  friend class Singleton<LrWpanFlowMonitorHelper>;

  LrWpanFlowMonitorHelper (const LrWpanFlowMonitorHelper&);

  LrWpanFlowMonitorHelper& operator= (const LrWpanFlowMonitorHelper&);

  ObjectFactory m_monitorFactory;        //!< Object factory
  Ptr<FlowMonitor> m_flowMonitor;        //!< the FlowMonitor object
  Ptr<FlowClassifier> m_flowClassifierLrWpan; //!< the FlowClassifier object for IPv4
};

} // namespace ns3


#endif 
