static Variant php_mb_numericentity_exec(const String& str,
                                         const Variant& convmap,
                                         const String& encoding,
                                         bool is_hex, int type) {
  int mapsize=0;
  mbfl_string string, result, *ret;
  mbfl_no_encoding no_encoding;

  mbfl_string_init(&string);
  string.no_language = MBSTRG(current_language);
  string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  string.val = (unsigned char *)str.data();
  string.len = str.size();

  if (type == 0 && is_hex) {
    type = 2; /* output in hex format */
  }

  /* encoding */
  if (!encoding.empty()) {
    no_encoding = mbfl_name2no_encoding(encoding.data());
    if (no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", encoding.data());
      return false;
    } else {
      string.no_encoding = no_encoding;
    }
  }

  /* conversion map */
  int *iconvmap = nullptr;
  if (convmap.isArray()) {
    Array convs = convmap.toArray();
    mapsize = convs.size();
    if (mapsize > 0) {
      iconvmap = (int*)req::malloc_noptrs(mapsize * sizeof(int));
      int *mapelm = iconvmap;
      for (ArrayIter iter(convs); iter; ++iter) {
        *mapelm++ = iter.second().toInt32();
      }
    }
  }
  if (iconvmap == nullptr) {
    return false;
  }
  mapsize /= 4;

  ret = mbfl_html_numeric_entity(&string, &result, iconvmap, mapsize, type);
  req::free(iconvmap);
  if (ret != nullptr) {
    if (ret->len > StringData::MaxSize) {
      raise_warning("String too long, max is %d", StringData::MaxSize);
      return false;
    }
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}