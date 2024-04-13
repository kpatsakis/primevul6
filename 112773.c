int string_levenshtein(const char *s1, int l1, const char *s2, int l2,
                       int cost_ins, int cost_rep, int cost_del ) {
  int *p1, *p2, *tmp;
  int i1, i2, c0, c1, c2;

  if (l1==0) return l2*cost_ins;
  if (l2==0) return l1*cost_del;

  if ((l1>LEVENSHTEIN_MAX_LENTH)||(l2>LEVENSHTEIN_MAX_LENTH)) {
    raise_warning("levenshtein(): Argument string(s) too long");
    return -1;
  }

  p1 = (int*)smart_malloc((l2+1) * sizeof(int));
  p2 = (int*)smart_malloc((l2+1) * sizeof(int));

  for(i2=0;i2<=l2;i2++) {
    p1[i2] = i2*cost_ins;
  }

  for(i1=0;i1<l1;i1++) {
    p2[0]=p1[0]+cost_del;
    for(i2=0;i2<l2;i2++) {
      c0=p1[i2]+((s1[i1]==s2[i2])?0:cost_rep);
      c1=p1[i2+1]+cost_del; if (c1<c0) c0=c1;
      c2=p2[i2]+cost_ins; if (c2<c0) c0=c2;
      p2[i2+1]=c0;
    }
    tmp=p1; p1=p2; p2=tmp;
  }

  c0=p1[l2];
  smart_free(p1);
  smart_free(p2);
  return c0;
}