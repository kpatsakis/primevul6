Variant HHVM_FUNCTION(mb_convert_kana,
                      const String& str,
                      const Variant& opt_option,
                      const Variant& opt_encoding) {
  const String option = convertArg(opt_option);
  const String encoding = convertArg(opt_encoding);

  mbfl_string string, result, *ret;
  mbfl_string_init(&string);
  string.no_language = MBSTRG(current_language);
  string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  string.val = (unsigned char *)str.data();
  string.len = str.size();

  int opt = 0x900;
  if (!option.empty()) {
    const char *p = option.data();
    int n = option.size();
    int i = 0;
    opt = 0;
    while (i < n) {
      i++;
      switch (*p++) {
      case 'A': opt |= 0x1;      break;
      case 'a': opt |= 0x10;     break;
      case 'R': opt |= 0x2;      break;
      case 'r': opt |= 0x20;     break;
      case 'N': opt |= 0x4;      break;
      case 'n': opt |= 0x40;     break;
      case 'S': opt |= 0x8;      break;
      case 's': opt |= 0x80;     break;
      case 'K': opt |= 0x100;    break;
      case 'k': opt |= 0x1000;   break;
      case 'H': opt |= 0x200;    break;
      case 'h': opt |= 0x2000;   break;
      case 'V': opt |= 0x800;    break;
      case 'C': opt |= 0x10000;  break;
      case 'c': opt |= 0x20000;  break;
      case 'M': opt |= 0x100000; break;
      case 'm': opt |= 0x200000; break;
      }
    }
  }

  /* encoding */
  if (!encoding.empty()) {
    string.no_encoding = mbfl_name2no_encoding(encoding.data());
    if (string.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", encoding.data());
      return false;
    }
  }

  ret = mbfl_ja_jp_hantozen(&string, &result, opt);
  if (ret != nullptr) {
    if (ret->len > StringData::MaxSize) {
      raise_warning("String too long, max is %d", StringData::MaxSize);
      return false;
    }
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}