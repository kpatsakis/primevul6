static void memcache_set_type_from_flag(Variant& var, uint32_t flags) {
  switch (flags & MMC_TYPE_MASK) {
  case MMC_TYPE_BOOL:
    var = var.toBoolean();
    break;
  case MMC_TYPE_LONG:
    var = var.toInt64();
    break;
  case MMC_TYPE_DOUBLE:
    var = var.toDouble();
    break;
  }
}