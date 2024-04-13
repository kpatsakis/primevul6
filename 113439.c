static Variant php_imagettftext_common(int mode, int extended,
                                       const Variant& arg1,
                                       const Variant& arg2,
                                       const Variant& arg3,
                                       const Variant& arg4,
                                       const Variant& arg5 = uninit_variant,
                                       const Variant& arg6 = uninit_variant,
                                       const Variant& arg7 = uninit_variant,
                                       const Variant& arg8 = uninit_variant,
                                       const Variant& arg9 = uninit_variant) {
  gdImagePtr im=nullptr;
  long col = -1, x = -1, y = -1;
  int brect[8];
  double ptsize, angle;
  String str;
  String fontname;
  Array extrainfo;
  char *error = nullptr;
  gdFTStringExtra strex = {0};

  if (mode == TTFTEXT_BBOX) {
    ptsize = arg1.toDouble();
    angle = arg2.toDouble();
    fontname = arg3.toString();
    str = arg4.toString();
    extrainfo = arg5;
  } else {
    Resource image = arg1.toResource();
    ptsize = arg2.toDouble();
    angle = arg3.toDouble();
    x = arg4.toInt64();
    y = arg5.toInt64();
    col = arg6.toInt64();
    fontname = arg7.toString();
    str = arg8.toString();
    extrainfo = arg9;
    im = get_valid_image_resource(image);
    if (!im) return false;
  }

  /* convert angle to radians */
  angle = angle * (M_PI/180);

  if (extended && !extrainfo.empty()) {  /* parse extended info */

    /* walk the assoc array */
    for (ArrayIter iter(extrainfo); iter; ++iter) {
      Variant key = iter.first();
      if (!key.isString()) continue;
      Variant item = iter.second();
      if (equal(key, s_linespacing)) {
        strex.flags |= gdFTEX_LINESPACE;
        strex.linespacing = item.toDouble();
      }
    }
  }

  FILE *fp = nullptr;
  if (!RuntimeOption::FontPath.empty()) {
    fontname = String(RuntimeOption::FontPath.c_str()) +
               HHVM_FN(basename)(fontname);
  }
  auto stream = php_open_plain_file(fontname, "rb", &fp);
  if (!stream) {
    raise_warning("Invalid font filename %s", fontname.c_str());
    return false;
  }
  stream->close();

#ifdef USE_GD_IMGSTRTTF
  if (extended) {
    error = gdImageStringFTEx(im, brect, col, (char*)fontname.c_str(),
                              ptsize, angle, x, y, (char*)str.c_str(),
                              &strex);
  }
  else {
    error = gdImageStringFT(im, brect, col, (char*)fontname.c_str(),
                            ptsize, angle, x, y, (char*)str.c_str());
  }
#else /* !USE_GD_IMGSTRTTF */
  error = gdttf(im, brect, col, fontname.c_str(),
                ptsize, angle, x, y, str.c_str());
#endif

  if (error) {
    raise_warning("%s", error);
    return false;
  }

  /* return array with the text's bounding box */
  Array ret = Array::CreateDArray();
  for (int i = 0; i < 8; i++) {
    ret.set(i, brect[i]);
  }
  return ret;
}