static int fill_prefix_v6(prefix_t *prefix, const struct in6_addr *addr, int bits, int maxbits) {
#ifdef PATRICIA_IPV6
  if(bits < 0 || bits > maxbits)
    return -1;

  memcpy(&prefix->add.sin6, addr, (maxbits + 7) / 8);
  prefix->family = AF_INET6, prefix->bitlen = bits, prefix->ref_count = 0;

  return 0;
#else
  return(-1);
#endif
}