int NDPI_BITMASK_COMPARE(NDPI_PROTOCOL_BITMASK a, NDPI_PROTOCOL_BITMASK b) {
  int i;

  for (i = 0; i < NDPI_NUM_FDS_BITS; i++) {
    if(a.fds_bits[i] & b.fds_bits[i])
      return(1);
  }

  return(0);
}