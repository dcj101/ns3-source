#include "wsn-fedlearning-tag.h"
#include <ns3/integer.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (WsnFedTag);

TypeId
WsnFedTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WsnFedTag")
    .SetParent<Tag> ()
    .SetGroupName ("LrWpan")
    .AddConstructor<WsnFedTag> ()
  ;
  return tid;
}

TypeId
WsnFedTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

WsnFedTag::WsnFedTag (void)
{
    size = 0;
}

WsnFedTag::WsnFedTag (std::vector<double> mode)
  : m_model (mode)
{
    size = m_model.size();
}

uint32_t
WsnFedTag::GetSerializedSize (void) const
{
  return m_model.size()*sizeof (double);
}

void
WsnFedTag::Serialize (TagBuffer i) const
{
  for(auto it : m_model)
  {
    i.WriteDouble(it);
  }
}

void
WsnFedTag::Deserialize (TagBuffer i)
{
  for(uint32_t j = 0; j < size; ++ j)
  {
    double x = i.ReadDouble ();
    m_model.push_back(x);
  }
}

void
WsnFedTag::Print (std::ostream &os) const
{
  
}

void
WsnFedTag::Set (std::vector<double> mode)
{
  m_model = mode;
}

std::vector<double>
WsnFedTag::Get (void) const
{
  return m_model;
}

}
