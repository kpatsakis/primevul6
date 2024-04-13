initialize_cclass(CClassNode* cc)
{
  BITSET_CLEAR(cc->bs);
  /* cc->base.flags = 0; */
  cc->flags = 0;
  cc->mbuf  = NULL;
}