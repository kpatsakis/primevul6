static uint32_t memcache_get_flag_for_type(const Variant& var) {
  switch (var.getType()) {
    case KindOfBoolean:
      return MMC_TYPE_BOOL;
    case KindOfInt64:
      return MMC_TYPE_LONG;
    case KindOfDouble:
      return MMC_TYPE_DOUBLE;

    case KindOfUninit:
    case KindOfNull:
    case KindOfPersistentString:
    case KindOfString:
    case KindOfPersistentVec:
    case KindOfVec:
    case KindOfPersistentDict:
    case KindOfDict:
    case KindOfPersistentKeyset:
    case KindOfKeyset:
    case KindOfPersistentShape:
    case KindOfShape:
    case KindOfPersistentArray:
    case KindOfArray:
    case KindOfObject:
    case KindOfResource:
    case KindOfRef:
    case KindOfFunc:
    case KindOfClass:
      return MMC_TYPE_STRING;
  }
  not_reached();
}