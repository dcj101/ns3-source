#ifndef WSN_F_H
#define WSN_F_H

#include <stdint.h>
#include <std::vector>
#include "ns3/node.h"

namespace ns3
{


 class WsnFedTag : public Tag
  {
  public:

    static TypeId GetTypeId (void);
    
    virtual TypeId GetInstanceTypeId (void) const;
    

    WsnFedTag (void);
    
    WsnFedTag (std::vector<uint32_t> mode);
    
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
    virtual void Print (std::ostream &os) const;
    
    void Set (std::vector<uint32_t> mode);
    
    double Get (void) const;
  private:
    uint32_t size;
    std::vector<uint32_t> m_mode;
  };

}


#endif