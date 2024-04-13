bool HHVM_FUNCTION(array_key_exists,
                   const Variant& key,
                   const Variant& search) {
  const ArrayData *ad;

  auto const searchCell = search.asCell();
  if (LIKELY(isArrayType(searchCell->m_type))) {
    ad = searchCell->m_data.parr;
  } else if (searchCell->m_type == KindOfObject) {
    ObjectData* obj = searchCell->m_data.pobj;
    if (obj->isCollection()) {
      return collections::contains(obj, key);
    }
    return HHVM_FN(array_key_exists)(key, toArray(search));
  } else {
    throw_bad_type_exception("array_key_exists expects an array or an object; "
                             "false returned.");
    return false;
  }

  auto const cell = key.asCell();

  switch (cell->m_type) {
    case KindOfUninit:
    case KindOfNull:
      return ad->useWeakKeys() && ad->exists(staticEmptyString());

    case KindOfBoolean:
    case KindOfDouble:
    case KindOfPersistentArray:
    case KindOfArray:
    case KindOfObject:
    case KindOfResource:
      if (!ad->useWeakKeys()) throwInvalidArrayKeyException(cell, ad);
      raise_warning("Array key should be either a string or an integer");
      return false;

    case KindOfPersistentString:
    case KindOfString: {
      int64_t n = 0;
      if (ad->convertKey(cell->m_data.pstr, n)) {
        return ad->exists(n);
      }
      return ad->exists(StrNR(cell->m_data.pstr));
    }
    case KindOfInt64:
      return ad->exists(cell->m_data.num);
    case KindOfRef:
    case KindOfClass:
      break;
  }
  not_reached();
}