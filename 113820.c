onig_reduce_nested_quantifier(Node* pnode, Node* cnode)
{
  int pnum, cnum;
  QtfrNode *p, *c;

  p = NQTFR(pnode);
  c = NQTFR(cnode);
  pnum = popular_quantifier_num(p);
  cnum = popular_quantifier_num(c);
  if (pnum < 0 || cnum < 0) return ;

  switch(ReduceTypeTable[cnum][pnum]) {
  case RQ_DEL:
    *pnode = *cnode;
    break;
  case RQ_A:
    p->target = c->target;
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 1;
    break;
  case RQ_AQ:
    p->target = c->target;
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 0;
    break;
  case RQ_QQ:
    p->target = c->target;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    break;
  case RQ_P_QQ:
    p->target = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 1;
    return ;
    break;
  case RQ_PQ_Q:
    p->target = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 1;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 0;
    return ;
    break;
  case RQ_ASIS:
    p->target = cnode;
    return ;
    break;
  }

  c->target = NULL_NODE;
  onig_node_free(cnode);
}