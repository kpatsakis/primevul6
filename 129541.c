static void SetSuccessor(CPpmd_State *p, CPpmd_Void_Ref v)
{
  (p)->SuccessorLow = (UInt16)((UInt32)(v) & 0xFFFF);
  (p)->SuccessorHigh = (UInt16)(((UInt32)(v) >> 16) & 0xFFFF);
}