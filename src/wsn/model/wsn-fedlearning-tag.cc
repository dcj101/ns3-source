#include "wsn-sensor-tag.h"
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
    .AddAttribute ("WsnFedTag", "WsnFedTag",
                   IntegerValue (0),
                   MakeIntegerAccessor (&WsnFedTag::Get),
                   MakeIntegerChecker<uint8_t> ())
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

WsnFedTag::WsnFedTag (std::vector<uint32_t> mode)
  : m_mode (mode)
{
    size = m_mode.size();
}

uint32_t
WsnFedTag::GetSerializedSize (void) const
{
  return sizeof (double);
}

void
WsnFedTag::Serialize (TagBuffer i) const
{
  for(auto it : m_mode)
  {
    i.WriteU32(it);
  }
}

void
WsnFedTag::Deserialize (TagBuffer i)
{
  for(uint32_t i = 0; i < size; ++ i)
  {
    m_mode.push_back(i);
  }
}

void
WsnFedTag::Print (std::ostream &os) const
{
  
}

void
WsnFedTag::Set (std::vector<uint32_t> mode)
{
  m_mode = mode;
}

std::vector<uint32_t>
WsnFedTag::Get (void) const
{
  return m_mode;
}

}
