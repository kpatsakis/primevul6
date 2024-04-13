String string_convert_hebrew_string(const String& inStr,
                                    int max_chars_per_line,
                                    int convert_newlines) {
  assert(!inStr.empty());
  auto str = inStr.data();
  auto str_len = inStr.size();
  const char *tmp;
  char *heb_str, *broken_str;
  char *target;
  int block_start, block_end, block_type, block_length, i;
  long max_chars=0;
  int begin, end, char_count, orig_begin;

  tmp = str;
  block_start=block_end=0;

  heb_str = (char *) smart_malloc(str_len + 1);
  SCOPE_EXIT { smart_free(heb_str); };
  target = heb_str+str_len;
  *target = 0;
  target--;

  block_length=0;

  if (isheb(*tmp)) {
    block_type = HEB_BLOCK_TYPE_HEB;
  } else {
    block_type = HEB_BLOCK_TYPE_ENG;
  }

  do {
    if (block_type == HEB_BLOCK_TYPE_HEB) {
      while ((isheb((int)*(tmp+1)) ||
              _isblank((int)*(tmp+1)) ||
              ispunct((int)*(tmp+1)) ||
              (int)*(tmp+1)=='\n' ) && block_end<str_len-1) {
        tmp++;
        block_end++;
        block_length++;
      }
      for (i = block_start; i<= block_end; i++) {
        *target = str[i];
        switch (*target) {
        case '(':  *target = ')';  break;
        case ')':  *target = '(';  break;
        case '[':  *target = ']';  break;
        case ']':  *target = '[';  break;
        case '{':  *target = '}';  break;
        case '}':  *target = '{';  break;
        case '<':  *target = '>';  break;
        case '>':  *target = '<';  break;
        case '\\': *target = '/';  break;
        case '/':  *target = '\\'; break;
        default:
          break;
        }
        target--;
      }
      block_type = HEB_BLOCK_TYPE_ENG;
    } else {
      while (!isheb(*(tmp+1)) &&
             (int)*(tmp+1)!='\n' && block_end < str_len-1) {
        tmp++;
        block_end++;
        block_length++;
      }
      while ((_isblank((int)*tmp) ||
              ispunct((int)*tmp)) && *tmp!='/' &&
             *tmp!='-' && block_end > block_start) {
        tmp--;
        block_end--;
      }
      for (i = block_end; i >= block_start; i--) {
        *target = str[i];
        target--;
      }
      block_type = HEB_BLOCK_TYPE_HEB;
    }
    block_start=block_end+1;
  } while (block_end < str_len-1);

  String brokenStr(str_len, ReserveString);
  broken_str = brokenStr.bufferSlice().ptr;
  begin=end=str_len-1;
  target = broken_str;

  while (1) {
    char_count=0;
    while ((!max_chars || char_count < max_chars) && begin > 0) {
      char_count++;
      begin--;
      if (begin <= 0 || _isnewline(heb_str[begin])) {
        while (begin > 0 && _isnewline(heb_str[begin-1])) {
          begin--;
          char_count++;
        }
        break;
      }
    }
    if (char_count == max_chars) { /* try to avoid breaking words */
      int new_char_count=char_count, new_begin=begin;

      while (new_char_count > 0) {
        if (_isblank(heb_str[new_begin]) || _isnewline(heb_str[new_begin])) {
          break;
        }
        new_begin++;
        new_char_count--;
      }
      if (new_char_count > 0) {
        char_count=new_char_count;
        begin=new_begin;
      }
    }
    orig_begin=begin;

    if (_isblank(heb_str[begin])) {
      heb_str[begin]='\n';
    }
    while (begin <= end && _isnewline(heb_str[begin])) {
      /* skip leading newlines */
      begin++;
    }
    for (i = begin; i <= end; i++) { /* copy content */
      *target = heb_str[i];
      target++;
    }
    for (i = orig_begin; i <= end && _isnewline(heb_str[i]); i++) {
      *target = heb_str[i];
      target++;
    }
    begin=orig_begin;

    if (begin <= 0) {
      *target = 0;
      break;
    }
    begin--;
    end=begin;
  }

  if (convert_newlines) {
    int count;
    auto ret = string_replace(broken_str, str_len, "\n", strlen("\n"),
                              "<br />\n", strlen("<br />\n"), count, true);
    if (!ret.isNull()) {
      return ret;
    }
  }
  brokenStr.setSize(str_len);
  return brokenStr;
}