Variant HHVM_FUNCTION(mb_encode_mimeheader,
                      const String& str,
                      const Variant& opt_charset,
                      const Variant& opt_transfer_encoding,
                      const String& linefeed /* = "\r\n" */,
                      int indent /* = 0 */) {
  const String charset = convertArg(opt_charset);
  const String transfer_encoding = convertArg(opt_transfer_encoding);

  mbfl_no_encoding charsetenc, transenc;
  mbfl_string  string, result, *ret;

  mbfl_string_init(&string);
  string.no_language = MBSTRG(current_language);
  string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  string.val = (unsigned char *)str.data();
  string.len = str.size();

  charsetenc = mbfl_no_encoding_pass;
  transenc = mbfl_no_encoding_base64;

  if (!charset.empty()) {
    charsetenc = mbfl_name2no_encoding(charset.data());
    if (charsetenc == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", charset.data());
      return false;
    }
  } else {
    const mbfl_language *lang = mbfl_no2language(MBSTRG(current_language));
    if (lang != nullptr) {
      charsetenc = lang->mail_charset;
      transenc = lang->mail_header_encoding;
    }
  }

  if (!transfer_encoding.empty()) {
    char ch = *transfer_encoding.data();
    if (ch == 'B' || ch == 'b') {
      transenc = mbfl_no_encoding_base64;
    } else if (ch == 'Q' || ch == 'q') {
      transenc = mbfl_no_encoding_qprint;
    }
  }

  mbfl_string_init(&result);
  ret = mbfl_mime_header_encode(&string, &result, charsetenc, transenc,
                                linefeed.data(), indent);
  if (ret != nullptr) {
    if (ret->len > StringData::MaxSize) {
      raise_warning("String too long, max is %d", StringData::MaxSize);
      return false;
    }
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}