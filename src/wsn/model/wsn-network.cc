#include "wsn-network.h"
#include "wsn-address-allocator.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("WsnNwkProtocol");

NS_OBJECT_ENSURE_REGISTERED (WsnNwkProtocol);


TypeId 
WsnNwkProtocol::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::WsnNwkProtocol")
                    .SetParent<Object> ()
                    .AddConstructor<WsnNwkProtocol>()
                    ;
    return tid;
}

WsnNwkProtocol::WsnNwkProtocol()
{
  NS_LOG_INFO(this);
  m_depth = -1;
}

WsnNwkProtocol::WsnNwkProtocol(NODE_TYPE type)
{
  NS_LOG_INFO(this);
  m_depth = -1;
  m_nodeType = type;
}


void 
WsnNwkProtocol::Send(NwkShortAddress sourceaddr,  
                      NwkShortAddress dstaddr,Ptr<Packet> packet, 
                      NwkHeader::FrameType ftype,
                      WsnNwkPayload::NWKCommandIdentifier dtype)
{
  NS_LOG_INFO(this << Simulator::Now ().GetSeconds () << " mddr " << m_addr << " source " << sourceaddr << " dst " << dstaddr << " ftype = " << ftype );

  if(m_depth == -1) 
  {
    NS_LOG_ERROR("No Join Request");
  }

  NwkHeader nwkHeader;
  nwkHeader.setType(ftype);
  nwkHeader.SetDestAddr(dstaddr);
  nwkHeader.SetSourceAddr(sourceaddr);
  packet->AddHeader(nwkHeader);
    WsnNwkPayload pl;

  if(ftype == NwkHeader::NWK_FRAME_COMMAND)
  {
    WsnNwkPayload wsnNwkPayload;
    wsnNwkPayload.SetnwkCommandIdentifier(dtype);
    packet->AddPacketTag(wsnNwkPayload);
  }
  
  McpsDataRequestParams params;
  m_ack = 1;
  if(m_ack) 
    params.m_txOptions = TX_OPTION_ACK;
  params.m_dstPanId = 1;
  params.m_msduHandle = 0;
  params.m_srcAddrMode = EXT_ADDR;
  params.m_dstAddrMode = EXT_ADDR;
  
  NwkShortAddress nextHop = m_rtable.Lookup(dstaddr);
  
  if(nextHop.GetAddressU16() == 0x0000)
      nextHop = m_route;

  NS_LOG_FUNCTION(this << " ntable is " << nextHop.GetAddressU16());
  NeighborTable::NeighborEntry nextNeight = m_ntable.GetNeighborEntry(nextHop.GetAddressU16());

  params.m_dstExtAddr = nextNeight.extendedAddr;
  NS_LOG_FUNCTION(this << " ntable MAC addr is  " << nextNeight.extendedAddr);
  // m_netDevice->GetMac()->McpsDataRequest(params,packet);
  Simulator::Schedule(Seconds(0.0),
                      &LrWpanMac::McpsDataRequest,
                      m_netDevice->GetMac(),params,packet);
}


void
WsnNwkProtocol::BuildRtable(std::vector<StaticRoute>&rtable)
{
    for(std::vector<StaticRoute>::iterator begin = rtable.begin()
                                ;begin != rtable.end(); ++ begin)
    {
        m_rtable.AddRoute(*begin);
    }
}

void 
WsnNwkProtocol::Install(Ptr<Node> node)
{
    m_node = node;
}


void 
WsnNwkProtocol::Assign(Ptr<LrWpanNetDevice> netDevice)
{
    NS_LOG_FUNCTION(this << netDevice);
    m_netDevice = netDevice;
}

void 
WsnNwkProtocol::JoinRequest(Ptr<WsnNwkProtocol> wsnNwkProtocol)
{
  NS_LOG_FUNCTION(this << " " << wsnNwkProtocol << m_nodeType);
  uint16_t addr;

  MlmeStartRequestParams params;
  NeighborTable* ntable;
  NwkShortAddress parents;
  Ptr<LrWpanNetDevice> netDevice;
  
  NeighborTable::NeighborEntry That;
  NeighborTable::NeighborEntry This;

  if(m_nodeType != NODE_TYPE::COOR)
  {
    m_depth = wsnNwkProtocol->GetDepth() + 1;
    m_panId = wsnNwkProtocol->GetPanID();

    ntable = wsnNwkProtocol->GetNeighborTable();
    parents = wsnNwkProtocol->GetNwkShortAddress();
    netDevice = wsnNwkProtocol->GetLrWpanNetDevice();
    
    addr = WsnAddressAllocator::Get()->AllocateNwkAddress(m_depth-1,IsRoute(),
                                                            parents.GetAddressU16());
    NS_LOG_FUNCTION(this << "addr is = " << addr);
    m_addr = std::move(NwkShortAddress(addr));

    m_route = parents;

    That.extendedAddr = netDevice->GetMac()->GetExtendedAddress();
    That.networkAddr = wsnNwkProtocol->GetNwkShortAddress();

    NS_LOG_FUNCTION(this << " that net addr is " << That.networkAddr << " " << That.extendedAddr);

    This.extendedAddr = m_netDevice->GetMac()->GetExtendedAddress();
    This.networkAddr = GetNwkShortAddress();

    NS_LOG_FUNCTION(this << " This net addr is " << This.networkAddr << " " << This.extendedAddr);

    m_ntable.AddNeighborEntry(That);
    ntable->AddNeighborEntry(This);

  }

  switch(m_nodeType)
  {
    case NODE_TYPE::EDGE :
      params.m_panCoor = false;
      params.m_PanId = m_panId;
      m_netDevice->GetMac ()->SetPanId (m_panId);
      m_netDevice->GetMac ()->SetAssociatedCoor(netDevice->GetMac()
                                                          ->GetAssociatedMac64AddressCoor());
    break;
    case NODE_TYPE::COOR :
      params.m_panCoor = true;
      params.m_PanId = 1;
      m_panId = 1;
      m_addr = std::move(NwkShortAddress("00:00"));
      m_depth = 0;
    break;
    case NODE_TYPE::ROUTE :
      params.m_panCoor = false;
      params.m_PanId = m_panId;
      m_netDevice->GetMac ()->SetPanId (m_panId);
      m_netDevice->GetMac ()->SetAssociatedCoor(netDevice->GetMac()
                                                          ->GetAssociatedMac64AddressCoor());
    break;
    default:
    break;
  }
  params.m_bcnOrd = 15; // 非时隙
    
  SetCallbackSet();
  
  Simulator::Schedule(Seconds(0.0),&LrWpanMac::MlmeStartRequest,
                        this->m_netDevice->GetMac(),params);
  
  if(m_nodeType != NODE_TYPE::COOR)
    Simulator::Schedule(Seconds(0.1),&WsnNwkProtocol::Send,
                        this,m_addr,parents,Create<Packet>(50),
                        NwkHeader::NWK_FRAME_COMMAND,WsnNwkPayload::WSN_PL_NETWORK_UPDATE);
}

void 
WsnNwkProtocol::SetNodeType(NODE_TYPE type)
{
  m_nodeType = type;
}

void 
WsnNwkProtocol::SetNode(Ptr<Node> node)
{
    NS_LOG_FUNCTION (this << node);
    m_node = node;
}

void
WsnNwkProtocol::SetAck(bool ack)
{
  m_ack = ack;
}

uint8_t
WsnNwkProtocol::GetPanID()
{
  NS_LOG_FUNCTION(this << " getmypanID" << m_panId);
  return m_panId;
}

uint8_t
WsnNwkProtocol::GetDepth()
{
  return m_depth;
}   

NwkShortAddress 
WsnNwkProtocol::GetNwkShortAddress()
{
  NS_LOG_FUNCTION(this << m_addr << " Get Addr");
  return m_addr.GetAddressU16();
}

Ptr<LrWpanNetDevice> 
WsnNwkProtocol::GetLrWpanNetDevice()
{
  return m_netDevice;
}

RoutingTable*
WsnNwkProtocol::GetRoutingTable()
{
  return &m_rtable;
}

NeighborTable* 
WsnNwkProtocol::GetNeighborTable()
{
  return &m_ntable;
}

void 
WsnNwkProtocol::NotifyNewAggregate (void)
{
  NS_LOG_FUNCTION (this);
    if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node> ();
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}

void 
WsnNwkProtocol::BeaconIndication (MlmeBeaconNotifyIndicationParams params, Ptr<Packet> p)
{
    NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " secs | Received BEACON packet of size " << p->GetSize ());
}

void
WsnNwkProtocol::DataIndication (McpsDataIndicationParams params, Ptr<Packet> p)
{    
    NS_LOG_FUNCTION(this);
    if(m_nodeType == NODE_TYPE::EDGE)
      NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "s EDGE Received packet of size " << p->GetSize ());
    else if(m_nodeType == NODE_TYPE::COOR)
      NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "s Coordinator Received packet (size " << p->GetSize () << " bytes)");
    else 
      NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "s Route Received packet (size " << p->GetSize () << " bytes)");
    
    NwkHeader receiverNwkHeader;
    p->RemoveHeader(receiverNwkHeader);

    double Delay = 0.1;
    double gap = 0.1;

     
    if(receiverNwkHeader.GetType() == NwkHeader::NWK_FRAME_COMMAND)
    {
      WsnNwkPayload pl;
      p->RemovePacketTag(pl);
      if(pl.GetnwkCommandIdentifier() == (uint8_t)WsnNwkPayload::WSN_PL_NETWORK_UPDATE)
      {
        if(m_nodeType == NODE_TYPE::EDGE) 
        {
          return;
        }
        NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "s Received Command packet of size " << p->GetSize ());
        std::vector<NeighborTable::NeighborEntry> ntable = m_ntable.GetNeighborEntries();
        for(auto it : ntable)
        {
          if(it.extendedAddr == params.m_srcExtAddr) 
          {
            NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " secs update route ");
            // 更新路由表
            StaticRoute newRoute(receiverNwkHeader.GetSourceAddr(),it.networkAddr);
            m_rtable.AddRoute(newRoute);
            m_rtable.Print();
            continue;
          }
          // 注意要把包复制出来，要不然会出现浅拷贝
          Ptr<Packet> newp = p->Copy();
          Simulator::Schedule(Seconds(Delay),&WsnNwkProtocol::Send,
                        this,receiverNwkHeader.GetSourceAddr(),it.networkAddr
                        ,newp,NwkHeader::NWK_FRAME_COMMAND,WsnNwkPayload::WSN_PL_NETWORK_UPDATE);
          Delay += gap;
          NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " send update route --->>>");
        }
      }
      else if(pl.GetnwkCommandIdentifier() == (uint8_t)WsnNwkPayload::WSN_PL_MODEL_RECV)
      {
        if(m_nodeType == NODE_TYPE::COOR)
        {
          FvGModel(p->Copy());
          return;
        }
        if(m_wsnGetModelCallback.IsNull())
        {
          NS_LOG_FUNCTION(this << " i  am not the learning node");
          return;
        }
        if(receiverNwkHeader.GetDestAddr() != NwkShortAddress((uint16_t)0))
          RecvModel(p->Copy());
        NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "s Received model packet of size " << p->GetSize ());
        std::vector<NeighborTable::NeighborEntry> ntable = m_ntable.GetNeighborEntries();
        for(auto it : ntable)
        {
          if(it.extendedAddr == params.m_srcExtAddr) 
          {
            continue;
          }
          // 注意要把包复制出来，要不然会出现浅拷贝
          Ptr<Packet> newp = p->Copy();

          Simulator::Schedule(Seconds(Delay),&WsnNwkProtocol::Send,
                        this,m_addr,it.networkAddr
                        ,newp,NwkHeader::NWK_FRAME_COMMAND,WsnNwkPayload::WSN_PL_MODEL_RECV);
          Delay += gap;
          NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " send update model --->>>");
        }
      }
    }
    else 
    {
      if(m_nodeType == NODE_TYPE::ROUTE || (receiverNwkHeader.GetDestAddr() != m_addr))
      {
        NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " secs | Received DATA packet of size " << p->GetSize () << 
        ",but i am a " << m_nodeType << " m_addr is " << m_addr << " ,so i will forwarding packet");
        // 路由器转发数据包
        // Send(receiverNwkHeader.GetSourceAddr(),receiverNwkHeader.GetDestAddr(),p,NwkHeader::NWK_FRAME_DATA);
          Ptr<Packet> newp = p->Copy();
          Simulator::Schedule(Seconds(0.0),&WsnNwkProtocol::Send,
                this,receiverNwkHeader.GetSourceAddr(),receiverNwkHeader.GetDestAddr(),
                newp,NwkHeader::NWK_FRAME_DATA,WsnNwkPayload::WSN_PL_NONE);          
      }
      else 
      {
        NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " secs | Coor Received DATA packet of size " << p->GetSize ());
        //实现应用层回调 
      }
    }
}

void 
WsnNwkProtocol::TransEndIndication (McpsDataConfirmParams params)
{
    if (params.m_status == LrWpanMcpsDataConfirmStatus::IEEE_802_15_4_SUCCESS)
    {
        NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " secs | Transmission successfully sent");
    }
}

void 
WsnNwkProtocol::DataIndicationCoordinator (McpsDataIndicationParams params, Ptr<Packet> p)
{
    NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "s Coordinator Received DATA packet (size " << p->GetSize () << " bytes)");
}

void 
WsnNwkProtocol::StartConfirm (MlmeStartConfirmParams params)
{
    NS_LOG_FUNCTION(this);
    if (params.m_status == MLMESTART_SUCCESS)
    {
        NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << "Beacon status SUCESSFUL");
    }
}


void 
WsnNwkProtocol::SetCallbackSet()
{
    NS_LOG_FUNCTION(this);
    if(m_nodeType == NODE_TYPE::COOR)
    { 
      m_MlmeStartConfirmCallback = MakeCallback (&WsnNwkProtocol::StartConfirm,this);
      m_netDevice->GetMac ()->SetMlmeStartConfirmCallback (m_MlmeStartConfirmCallback);
    } 
    else
    {
      m_McpsDataConfirmCallback = MakeCallback (&WsnNwkProtocol::TransEndIndication,this);
      m_netDevice->GetMac ()->SetMcpsDataConfirmCallback (m_McpsDataConfirmCallback);

      m_MlmeBeaconNotifyIndicationCallback = MakeCallback (&WsnNwkProtocol::BeaconIndication,this);
      m_netDevice->GetMac ()->SetMlmeBeaconNotifyIndicationCallback (m_MlmeBeaconNotifyIndicationCallback);
    } 
     m_McpsDataIndicationCallback = MakeCallback (&WsnNwkProtocol::DataIndication,this);
     m_netDevice->GetMac ()->SetMcpsDataIndicationCallback (m_McpsDataIndicationCallback);
}

void
WsnNwkProtocol::SetRecvModelCallBack(WsnRecvModelCallback mCb)
{
  m_wsnRecvModelCallback = mCb;
}

void
WsnNwkProtocol::SetGetModelCallBack(WsnGetModelCallback mCb)
{
  m_wsnGetModelCallback = mCb;
}

void 
WsnNwkProtocol::DoInitialize (void)
{
}

void 
WsnNwkProtocol::DoDispose (void)
{
    NS_LOG_FUNCTION (this);
    m_node = 0;
    Object::DoDispose ();
}

void 
WsnNwkProtocol::GetModel(void)
{
  NS_LOG_FUNCTION(this);
  if(m_wsnGetModelCallback.IsNull())
  {
    NS_LOG_FUNCTION(this << "m_wsnGetModelCallback is Null");
    return;
  }
  std::vector<double> model = m_wsnGetModelCallback();
  if(!model.size())
  {
    Simulator::Schedule(Seconds(1.0),&WsnNwkProtocol::GetModel,this);
    return;
  }
  NS_LOG_FUNCTION(this << "Get model is " << model.size());
  for(auto it : model)
  {
    NS_LOG_UNCOND(it << " ");
  }
  NS_LOG_UNCOND("\n");
  
  Ptr<Packet> packet = Create<Packet>(50);
  WsnFedTag wsnFedTag(model);
  packet->AddPacketTag(wsnFedTag);
  Simulator::Schedule(Seconds(0.0),&WsnNwkProtocol::Send,
              this,m_addr,NwkShortAddress((uint16_t)0)
              ,packet,NwkHeader::NWK_FRAME_COMMAND,WsnNwkPayload::WSN_PL_MODEL_RECV);
}

void 
WsnNwkProtocol::RecvModel(Ptr<Packet> model)
{
  NS_LOG_FUNCTION(this);
  WsnFedTag m_model;
  model->RemovePacketTag(m_model);
  std::vector<double> model_;
  model_ = m_model.Get();
  NS_LOG_FUNCTION(this << " Recv model is " << model_.size());
  
  for(auto it : model_)
  {
    NS_LOG_UNCOND(it << " ");
  }
  NS_LOG_UNCOND("\n");

  m_wsnRecvModelCallback(m_model.Get());
  Simulator::Schedule(Seconds(15.0),&WsnNwkProtocol::GetModel,this);
}

void 
WsnNwkProtocol::FvGModel(Ptr<Packet> model)
{
  NS_LOG_FUNCTION(this);
  WsnFedTag m_model;
  model->RemovePacketTag(m_model);
  std::vector<double> m = m_model.Get(); 
  NS_LOG_FUNCTION(this << m.size() << "-------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  if(!m_modelFvg.size())
  {
    m_modelFvg = m; 
    m_modelFvg.push_back(1);
  }
  else
  {
    double &tt = m_modelFvg[m_modelFvg.size()-1];
    for(uint32_t i = 0; i < m_modelFvg.size()-1; ++ i)
    {
      m_modelFvg[i] = m[i]*(m_modelFvg[i]*tt)/(tt+1);
    }
    tt++;
  }
  for(auto it : m_modelFvg)
  {
    NS_LOG_UNCOND(it << " <-> ");
  }
  NS_LOG_UNCOND("\n");
  if(m_modelFvg[m_modelFvg.size()-1] > 1)
  {
    m_modelFvg.pop_back();
    double Delay = 0.1;
    std::vector<NeighborTable::NeighborEntry> ntable = m_ntable.GetNeighborEntries();
    for(auto it : ntable)
    {
      // 注意要把包复制出来，要不然会出现浅拷贝
      Ptr<Packet> newp = Create<Packet>(0);
      WsnFedTag newTag(m_modelFvg);
      newp->AddPacketTag(newTag);
      Simulator::Schedule(Seconds(Delay),&WsnNwkProtocol::Send,
                    this,m_addr,it.networkAddr
                    ,newp,NwkHeader::NWK_FRAME_COMMAND,WsnNwkPayload::WSN_PL_MODEL_RECV);
      Delay += 0.1;
      NS_LOG_UNCOND (m_addr << " " <<Simulator::Now ().GetSeconds () << " Coor send fvg model --->>>");
    }
    m_modelFvg.clear();
  }
}

void
WsnNwkProtocolHelper::CreateAndAggregateObjectFromTypeId (NodeContainer container, const std::string typeId)
{
  NS_LOG_FUNCTION(this << " " << typeId);
  NodeContainer::Iterator i;
  for (i = container.Begin (); i != container.End (); ++i)
  {
    ObjectFactory factory;
    factory.SetTypeId (typeId);
    Ptr<Object> protocol = factory.Create <Object> ();
    (*i)->AggregateObject (protocol);
  }
}

void
WsnNwkProtocolHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  NS_LOG_FUNCTION(this << " " << typeId);
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> protocol = factory.Create <Object> ();
  node->AggregateObject (protocol);
}

}