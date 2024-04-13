Variant HHVM_FUNCTION(imageloadfont, const String& /*file*/) {
  // TODO: ind = 5 + zend_list_insert(font, le_gd_font);
  throw_not_supported(__func__, "NYI");
#ifdef NEVER
  Variant stream;
  zval **file;
  int hdr_size = sizeof(gdFont) - sizeof(char *);
  int ind, body_size, n = 0, b, i, body_size_check;
  gdFontPtr font;
  php_stream *stream;


  stream = File::Open(file, "rb");
  if (!stream) {
    raise_warning("failed to open file: %s", file.c_str());
    return false;
  }

  /* Only supports a architecture-dependent binary dump format
   * at the moment.
   * The file format is like this on machines with 32-byte integers:
   *
   * byte 0-3:   (int) number of characters in the font
   * byte 4-7:   (int) value of first character in the font (often 32, space)
   * byte 8-11:  (int) pixel width of each character
   * byte 12-15: (int) pixel height of each character
   * bytes 16-:  (char) array with character data, one byte per pixel
   *                    in each character, for a total of
   *                    (nchars*width*height) bytes.
   */
  font = (gdFontPtr) IM_MALLOC(sizeof(gdFont));
  CHECK_ALLOC_R(font, sizeof(gdFont), false);
  b = 0;
  String hdr = stream->read(hdr_size);
  if (hdr.length() < hdr_size) {
    IM_FREE(font);
    if (stream->eof()) {
      raise_warning("End of file while reading header");
    } else {
      raise_warning("Error while reading header");
    }
    stream->close();
    return false;
  }
  memcpy((void*)font, hdr.c_str(), hdr.length());
  i = int64_t(f_tell(stream));
  stream->seek(0, SEEK_END);
  body_size_check = int64_t(f_tell(stream)) - hdr_size;
  stream->seek(i, SEEK_SET);

  body_size = font->w * font->h * font->nchars;
  if (body_size != body_size_check) {
    font->w = FLIPWORD(font->w);
    font->h = FLIPWORD(font->h);
    font->nchars = FLIPWORD(font->nchars);
    body_size = font->w * font->h * font->nchars;
  }

  if (font->nchars <= 0 ||
      font->h <= 0 ||
      font->nchars >= INT_MAX || font->h >= INT_MAX) {
    raise_warning("Error reading font, invalid font header");
    IM_FREE(font);
    stream->close();
    return false;
  }

  if ((font->nchars * font->h) <= 0 ||
      font->w <= 0 ||
      (font->nchars * font->h) >= INT_MAX || font->w >= INT_MAX) {
    raise_warning("Error reading font, invalid font header");
    IM_FREE(font);
    stream->close();
    return false;
  }

  if (body_size != body_size_check) {
    raise_warning("Error reading font");
    IM_FREE(font);
    stream->close();
    return false;
  }

  String body = stream->read(body_size);
  if (body.length() < body_size) {
    IM_FREE(font);
    if (stream->eof()) {
      raise_warning("End of file while reading body");
    } else {
      raise_warning("Error while reading body");
    }
    stream->close();
    return false;
  }
  font->data = IM_MALLOC(body_size);
  CHECK_ALLOC_R(font->data, body_size, false);
  memcpy((void*)font->data, body.c_str(), body.length());
  stream->close();

  /* Adding 5 to the font index so we will never have font indices
   * that overlap with the old fonts (with indices 1-5).  The first
   * list index given out is always 1.
   */
  // ind = 5 + zend_list_insert(font, le_gd_font);

  return ind;
#endif
}