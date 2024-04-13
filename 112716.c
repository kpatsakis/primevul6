String StringUtil::HtmlEncodeExtra(const String& input, QuoteStyle quoteStyle,
                                   const char *charset, bool nbsp,
                                   Array extra) {
  if (input.empty()) return input;

  assert(charset);
  int flags = STRING_HTML_ENCODE_UTF8;
  if (nbsp) {
    flags |= STRING_HTML_ENCODE_NBSP;
  }
  if (RuntimeOption::Utf8izeReplace) {
    flags |= STRING_HTML_ENCODE_UTF8IZE_REPLACE;
  }
  if (!*charset || strcasecmp(charset, "UTF-8") == 0) {
  } else if (strcasecmp(charset, "ISO-8859-1") == 0) {
    flags &= ~STRING_HTML_ENCODE_UTF8;
  } else {
    throw_not_implemented(charset);
  }

  const AsciiMap *am;
  AsciiMap tmp;

  switch (quoteStyle) {
    case QuoteStyle::FBUtf8Only:
      am = &mapNothing;
      flags |= STRING_HTML_ENCODE_HIGH;
      break;
    case QuoteStyle::FBUtf8:
      am = &mapBothQuotes;
      flags |= STRING_HTML_ENCODE_HIGH;
      break;
    case QuoteStyle::Both:
      am = &mapBothQuotes;
      break;
    case QuoteStyle::Double:
      am = &mapDoubleQuotes;
      break;
    case QuoteStyle::No:
      am = &mapNoQuotes;
      break;
    default:
      am = &mapNothing;
      raise_error("Unknown quote style: %d", (int)quoteStyle);
  }

  if (quoteStyle != QuoteStyle::FBUtf8Only && extra.toBoolean()) {
    tmp = *am;
    am = &tmp;
    for (ArrayIter iter(extra); iter; ++iter) {
      String item = iter.second().toString();
      char c = item.data()[0];
      tmp.map[c & 64 ? 1 : 0] |= 1uLL << (c & 63);
    }
  }

  int len = input.size();
  char *ret = string_html_encode_extra(input.data(), len,
                                       (StringHtmlEncoding)flags, am);
  if (!ret) {
    raise_error("HtmlEncode called on too large input (%d)", len);
  }
  return String(ret, len, AttachString);
}