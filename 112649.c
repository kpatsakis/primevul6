Variant HHVM_FUNCTION(mb_list_encodings_alias_names,
                      const Variant& opt_name) {
  const String name = convertArg(opt_name);

  const mbfl_encoding **encodings;
  const mbfl_encoding *encoding;
  mbfl_no_encoding no_encoding;
  int i, j;

  Array ret;
  if (name.isNull()) {
    i = 0;
    encodings = mbfl_get_supported_encodings();
    while ((encoding = encodings[i++]) != nullptr) {
      Array row;
      if (encoding->aliases != nullptr) {
        j = 0;
        while ((*encoding->aliases)[j] != nullptr) {
          row.append(String((*encoding->aliases)[j], CopyString));
          j++;
        }
      }
      ret.set(String(encoding->name, CopyString), row);
    }
  } else {
    no_encoding = mbfl_name2no_encoding(name.data());
    if (no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", name.data());
      return false;
    }

    char *encodingName = (char *)mbfl_no_encoding2name(no_encoding);
    if (encodingName != nullptr) {
      i = 0;
      encodings = mbfl_get_supported_encodings();
      while ((encoding = encodings[i++]) != nullptr) {
        if (strcmp(encoding->name, encodingName) != 0) continue;

        if (encoding->aliases != nullptr) {
          j = 0;
          while ((*encoding->aliases)[j] != nullptr) {
            ret.append(String((*encoding->aliases)[j], CopyString));
            j++;
          }
        }

        break;
      }
    } else {
      return false;
    }
  }
  return ret;
}