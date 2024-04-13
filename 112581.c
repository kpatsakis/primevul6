Variant HHVM_FUNCTION(mb_regex_encoding,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);

  if (encoding.empty()) {
    const char *retval = php_mb_regex_mbctype2name(MBSTRG(current_mbctype));
    if (retval != nullptr) {
      return String(retval, CopyString);
    }
    return false;
  }

  OnigEncoding mbctype = php_mb_regex_name2mbctype(encoding.data());
  if (mbctype == ONIG_ENCODING_UNDEF) {
    raise_warning("Unknown encoding \"%s\"", encoding.data());
    return false;
  }

  MBSTRG(current_mbctype) = mbctype;
  return true;
}