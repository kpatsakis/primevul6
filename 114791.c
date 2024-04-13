static int fill_prefix_v4(prefix_t *p, const struct in_addr *a, int b, int mb) {
  if(b < 0 || b > mb)
    return(-1);

  memset(p, 0, sizeof(prefix_t));
  memcpy(&p->add.sin, a, (mb + 7) / 8);
  p->family = AF_INET;
  p->bitlen = b;
  p->ref_count = 0;

  return(0);
}