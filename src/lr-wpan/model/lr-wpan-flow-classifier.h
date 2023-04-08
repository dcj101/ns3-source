#ifndef LrWpan_FLOAT_CLASSIFIER_H
#define LrWpan_FLOAT_CLASSIFIER_H

#include "lr-wpan-mac.h"
#include "lr-wpan-mac-header.h"
#include "ns3/flow-classifier.h"
#include <mutex>

namespace ns3
{

class LrWpanFlowClassifier : public FlowClassifier
{
public:
    struct TwoTuple
    {
        Mac64Address sourceAddress;
        Mac64Address destinationAddress;
    };

    LrWpanFlowClassifier();
    
    bool Classify (const LrWpanMacHeader &macHeader, Ptr<const Packet> macPayload,
                 uint32_t *out_flowId, uint32_t *out_packetId);

    TwoTuple FindFlow (FlowId flowId) const;

    virtual void SerializeToXmlStream (std::ostream &os, uint16_t indent) const;

    void getlock(){m_mutex.lock();}
    void releaselock(){m_mutex.unlock();}
private:
  std::map<TwoTuple, FlowId> m_flowMap;
  std::map<FlowId, FlowPacketId> m_flowPktIdMap;
  mutable std::mutex m_mutex;
};

bool operator < (const LrWpanFlowClassifier::TwoTuple &t1, const LrWpanFlowClassifier::TwoTuple &t2);

bool operator == (const LrWpanFlowClassifier::TwoTuple &t1, const LrWpanFlowClassifier::TwoTuple &t2);

}

#endif