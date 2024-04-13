Variant HHVM_FUNCTION(mb_http_input,
                      const Variant& opt_type) {
  const String type = convertArg(opt_type);

  int n;
  char *name;
  mbfl_encoding **entry;
  mbfl_encoding *result = nullptr;

  if (type.empty()) {
    result = MBSTRG(http_input_identify);
  } else {
    switch (*type.data()) {
    case 'G': case 'g': result = MBSTRG(http_input_identify_get);    break;
    case 'P': case 'p': result = MBSTRG(http_input_identify_post);   break;
    case 'C': case 'c': result = MBSTRG(http_input_identify_cookie); break;
    case 'S': case 's': result = MBSTRG(http_input_identify_string); break;
    case 'I': case 'i':
      {
        Array ret;
        entry = MBSTRG(http_input_list);
        n = MBSTRG(http_input_list_size);
        while (n > 0) {
          name = (char *)(*entry)->name;
          if (name) {
            ret.append(String(name, CopyString));
          }
          entry++;
          n--;
        }
        return ret;
      }
    case 'L': case 'l':
      {
        entry = MBSTRG(http_input_list);
        n = MBSTRG(http_input_list_size);
        StringBuffer list;
        while (n > 0) {
          name = (char *)(*entry)->name;
          if (name) {
            if (list.empty()) {
              list.append(name);
            } else {
              list.append(',');
              list.append(name);
            }
          }
          entry++;
          n--;
        }
        if (list.empty()) {
          return false;
        }
        return list.detach();
      }
    default:
      result = MBSTRG(http_input_identify);
      break;
    }
  }

  if (result != nullptr &&
      (name = (char *)(result)->name) != nullptr) {
    return String(name, CopyString);
  }
  return false;
}