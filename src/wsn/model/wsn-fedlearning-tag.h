#ifndef WSN_F_H
#define WSN_F_H

#include <stdint.h>
#include <vector>
#include "ns3/node.h"

namespace ns3
{


 class WsnFedTag : public Tag
  {
  public:

    static TypeId GetTypeId (void);
    
    virtual TypeId GetInstanceTypeId (void) const;
    

    WsnFedTag (void);
    
    WsnFedTag (std::vector<double> model);
    
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
    virtual void Print (std::ostream &os) const;
    
    void Set (std::vector<double> model);
    
    std::vector<double> Get (void) const;
  private:
    uint32_t size;
    std::vector<double> m_model;
  };

}


#endif