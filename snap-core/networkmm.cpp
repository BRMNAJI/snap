////////////////////////////////////////////////
// Mutimodal Network

int TMMNet::AddMode(const TStr& ModeName, TInt& ModeId) {

  //Book-keeping for new mode id and the hash lookups
  ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  PModeNet NewGraph = TModeNet.New();
  PModeNetV.Append(NewGraph);

  return ModeId;
}

int TMMNet::AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeNameToIdH.IsKey(ModeName1) || !ModeNameToIdH.IsKey(ModeName2)) { return -1; }
  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddLinkType(ModeId1, ModeId2, EdgeTypeName, EdgeTypeId)
}


int TMMNet::AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeIdToNameH.IsKey(ModeId1) || !ModeIdToNameH.IsKey(ModeId2)) { return -1; }
  EdgeTypeId = TInt(MxLinkTypeId);
  MxLinkTypeId++;
  LinkIdToNameH.AddDat(EdgeTypeId, EdgeTypeName);
  LinkNameToIdH.AddDat(EdgeTypeName, EdgeTypeId);

  TCrossNet Link = TCrossNet();
  TCrossNetH.AddDat(EdgeTypeId, Link);
  return 0;
}

TIntPr TMMNet::GetOrderedLinkPair(const TStr& Mode1, const TStr& Mode2) {
  TInt ModeId1 = GetModeId(Mode1);
  TInt ModeId2 = GetModeId(Mode2);
  return GetOrderedLinkPair(ModeId1, ModeId2);
}

TIntPr TMMNet::GetOrderedLinkPair(const TInt& Mode1, const TInt& Mode2) {
  if (Mode1 < Mode2) {
    return TIntPr(Mode1, Mode2);
  }
  return TIntPr(Mode2, Mode1);
}


int TCrossNet::AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& destNModeId, const int& EId) {
  int type1NId;
  int type2NId;
  bool direction = true;
  if (Mode1 == sourceNModeId && Mode2 == destNModeId) {
    type1NId = sourceNId;
    type2NId = destNId;
  } else if (Mode2 == sourceNModeId && Mode1 == destNModeId) {
    type1NId = destNId;
    type2NId = sourceNId;
    direction = false;
  } else {
    return -1;
  }
  return AddLink(type1NId, type2NId, direction, EId);
}

int TCrossNet::AddLink (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId) {
  if (EId == -1) {
    MxEId++;
    int newEId = MxEId;
  }
  else {
    IAssertR(!LinkH.IsKey(EId), TStr::Fmt("The edge with id %d already exists", EId ));
    int newEId = EId;
    if (newEId > MxEId) MxEId = newEId; //TODO: Figure this out
  }
  TCrossEdge newEdge(newEId, NIdType1, NIdType2, direction);
  LinkH.AddDat(newEid, newEdge);
  return 0;

}

int TCrossNet::DelLink(const int& EId) {
  if (LinkH.DelIfKey(EId)) { return 0; }
  return -1;
}

TCrossEdge TCrossNet::GetLink (const int& EId) const {
  IAssertR(LinkH.IsKey(EId),TStr::Fmt("No link with id %d exists", EId));
  return LinkH.GetDat(EId);
}

int TModeNet::AddEdge(const int& SrcNId, const int& DstNId, const int& OtherModeId, const bool& OtherIsSrc, const TInt& EId=-1){
  if (OtherIsSrc) {
    IAssertR(NodeH.IsKey(DstNId),TStr::Fmt("No node with id %d exists", DstNId));
    return MMNet->AddEdge(OtherModeId, NTypeId, SrcNId, DstNId, EId);
  }
  IAssertR(NodeH.IsKey(SrcNId),TStr::Fmt("No node with id %d exists", SrcNId));
  return MMNet->AddEdge(NTypeId, OtherModeId, SrcNId, DstNId, EId);
}

int TModeNet::AddEdge(const int& SrcNId, const int& DstNId, const TStr& OtherMode, const bool& OtherIsSrc, const TInt& EId=-1){
  TStr NMode = MMNet->GetModeName(NTypeId);
  if (OtherIsSrc) {
    IAssertR(NodeH.IsKey(DstNId),TStr::Fmt("No node with id %d exists", DstNId));
    return MMNet->AddEdge(OtherMode, NMode, SrcNId, DstNId, EId);
  }
  IAssertR(NodeH.IsKey(SrcNId),TStr::Fmt("No node with id %d exists", SrcNId));
  return MMNet->AddEdge(NMode, OtherMode, SrcNId, DstNId, EId);
}

int TModeNet::AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TStr& LinkTypeName, const TInt& EId=-1) {

  return MMNet->AddEdge(CurrModeNId, OtherModeNId, direction, LinkTypeName, EId);
}

int TModeNet::AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TInt& LinkTypeId, const TInt& EId=-1) {
  return MMNet->AddEdge(CurrModeNId, OtherModeNId, direction, LinkTypeId, EId);
}


int TModeNet::DelEdge(const TStr& LinkTypeName, const TInt& EId){
}


TStr TModeNet::GetNeighborLinkName(TStr& LinkName, bool isOutEdge) {
  TStr Cpy(LinkName);
  if (isOutEdge) {
    Cpy += ":OUT";
  } else {
    Cpy += ":IN";
  }
  return Cpy;
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const int linkId){
  TStr LinkName = MMNet->GetLinkName(linkId);
  return AddNeighbor(NId, EId, outEdge, linkName);
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName){
  TStr Name = GetNeighborLinkName(LinkName, outEdge);
  if (!NeighborTypes.IsKey(Name)) {
    NeighborTypes.AddKey(name);
  }
  return AppendIntVAttrDatN(NId, EId, Name); 
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName){
  TStr Name = GetNeighborLinkName(LinkName, outEdge);
  if (!NeighborTypes.IsKey(Name)) {
    return -1;
  }
  return DelFromIntVAttrDatN(NId, EId, Name);
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const int linkId){
  TStr LinkName = MMNet->GetLinkName(linkId);
  return DelNeighbor(NId, EId, outEdge, LinkName);
}

//TODO: Does TMMNet implement any of the logic for this? 
int TModeNet::DelNode ( const int& NId){

}
int TMMNet::AddEdge(int& NId, int& OtherTypeNId, bool& direction, TStr& LinkTypeName, TInt& EId) {
  int index = LinkNameToIdH.GetDat(LinkTypeName);
  TCrossNet& Link = TCrossNetV[index];
  Link.AddLink(NId, OtherTypeNId, direction, EId);
  //TODO: Finish this method
}
