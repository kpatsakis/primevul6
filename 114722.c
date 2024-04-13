void NDPI_DUMP_BITMASK(NDPI_PROTOCOL_BITMASK a) {
  int i;

  for (i = 0; i < NDPI_NUM_FDS_BITS; i++)
    printf("[%d=%u]", i, a.fds_bits[i]);

  printf("\n");
}