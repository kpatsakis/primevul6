inline uint64_t make_type(FMT_GEN15(FMT_ARG_TYPE_DEFAULT)) {
  return t0.type | (t1.type << 4) | (t2.type << 8) | (t3.type << 12) |
      (t4.type << 16) | (t5.type << 20) | (t6.type << 24) | (t7.type << 28) |
      (t8.type << 32) | (t9.type << 36) | (t10.type << 40) | (t11.type << 44) |
      (t12.type << 48) | (t13.type << 52) | (t14.type << 56);
}