#ifndef LrWpan_FLOW_PROBE_H
#define LrWpan_FLOW_PROBE_H

#include "ns3/flow-probe.h"
#include "ns3/flow-monitor.h"
#include "ns3/node.h"
#include "lr-wpan-flow-classifier.h"
#include "lr-wpan-mac.h"
#include "lr-wpan-flow-probe-tag.h"
#include "lr-wpan-net-device.h"

namespace ns3
{

class LrWpanFlowProbe : public FlowProbe
{
public:
    LrWpanFlowProbe (Ptr<FlowMonitor> monitor, Ptr<LrWpanFlowClassifier> classifier, Ptr<LrWpanNetDevice> device);
    virtual ~LrWpanFlowProbe ();

    static TypeId GetTypeId (void);

protected:

    virtual void DoDispose (void);

private:
    void SendOutgoingLogger (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload);

    void ForwardLogger (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload);

    void ForwardUpLogger (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload);
    
    Ptr<LrWpanFlowClassifier> m_classifier;

    Ptr<LrWpanMac> m_LrWpanMac;

};

}
#endif