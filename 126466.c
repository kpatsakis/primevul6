static uint32_t blk0(union char64long16 *block, int i) {
/* Forrest: SHA expect BIG_ENDIAN, swap if LITTLE_ENDIAN */
#if BYTE_ORDER == LITTLE_ENDIAN
  block->l[i] =
      (rol(block->l[i], 24) & 0xFF00FF00) | (rol(block->l[i], 8) & 0x00FF00FF);
#endif
  return block->l[i];
}