int NDPI_BITMASK_IS_EMPTY(NDPI_PROTOCOL_BITMASK a) {
  int i;

  for (i = 0; i < NDPI_NUM_FDS_BITS; i++)
    if(a.fds_bits[i] != 0)
      return(0);

  return(1);
}