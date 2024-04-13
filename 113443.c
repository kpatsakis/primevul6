Variant HHVM_FUNCTION(iptcembed, const String& iptcdata,
    const String& jpeg_file_name, int64_t spool /* = 0 */) {
  char psheader[] = "\xFF\xED\0\0Photoshop 3.0\08BIM\x04\x04\0\0\0";
  static_assert(sizeof(psheader) == 28, "psheader must be 28 bytes");
  unsigned int iptcdata_len = iptcdata.length();
  unsigned int marker, inx;
  unsigned char *spoolbuf = nullptr, *poi = nullptr;
  bool done = false;
  bool written = false;

  auto file = File::Open(jpeg_file_name, "rb");
  if (!file) {
    raise_warning("failed to open file: %s", jpeg_file_name.c_str());
    return false;
  }

  if (spool < 2) {
    auto stat = HHVM_FN(fstat)(Resource(file));
    // TODO(t7561579) until we can properly handle non-file streams here, don't
    // pretend we can and crash.
    if (!stat.isArray()) {
      raise_warning("unable to stat input");
      return false;
    }

    auto& stat_arr = stat.toCArrRef();
    auto st_size = stat_arr[s_size].toInt64();
    if (st_size < 0) {
      raise_warning("unsupported stream type");
      return false;
    }

    if (iptcdata_len >= (INT64_MAX - sizeof(psheader) - st_size - 1024 - 1)) {
      raise_warning("iptcdata too long");
      return false;
    }

    auto malloc_size = iptcdata_len + sizeof(psheader) + st_size + 1024 + 1;
    poi = spoolbuf = (unsigned char *)IM_MALLOC(malloc_size);
    CHECK_ALLOC_R(poi, malloc_size, false);
    memset(poi, 0, malloc_size);
  }
  if (php_iptc_get1(file, spool, poi?&poi:0) != 0xFF) {
    file->close();
    if (spoolbuf) {
      IM_FREE(spoolbuf);
    }
    return false;
  }

  if (php_iptc_get1(file, spool, poi?&poi:0) != 0xD8) {
    file->close();
    if (spoolbuf) {
      IM_FREE(spoolbuf);
    }
    return false;
  }

  while (!done) {
    marker = php_iptc_next_marker(file, spool, poi?&poi:0);
    if (marker == M_EOI) { /* EOF */
      break;
    } else if (marker != M_APP13) {
      php_iptc_put1(file, spool, (unsigned char)marker, poi?&poi:0);
    }
    switch (marker) {
    case M_APP13:
      /* we are going to write a new APP13 marker, so don't
         output the old one */
      php_iptc_skip_variable(file, 0, 0);
      php_iptc_read_remaining(file, spool, poi?&poi:0);
      done = true;
      break;

    case M_APP0:
      /* APP0 is in each and every JPEG, so when we hit APP0
         we insert our new APP13! */
    case M_APP1:
      if (written) {
        /* don't try to write the data twice */
        break;
      }
      written = true;

      php_iptc_skip_variable(file, spool, poi?&poi:0);

      if (iptcdata_len & 1) {
        iptcdata_len++; /* make the length even */
      }

      psheader[2] = (iptcdata_len + sizeof(psheader)) >> 8;
      psheader[3] = (iptcdata_len + sizeof(psheader)) & 0xff;

      for (inx = 0; inx < sizeof(psheader); inx++) {
        php_iptc_put1(file, spool, psheader[inx], poi ? &poi : 0);
      }

      php_iptc_put1(file, spool, (unsigned char)(iptcdata_len>>8),
                    poi?&poi:0);
      php_iptc_put1(file, spool, (unsigned char)(iptcdata_len&0xff),
                    poi?&poi:0);

      for (inx = 0; inx < iptcdata_len; inx++) {
        php_iptc_put1(file, spool, iptcdata.c_str()[inx], poi?&poi:0);
      }
      break;

    case M_SOS:
      /* we hit data, no more marker-inserting can be done! */
      php_iptc_read_remaining(file, spool, poi?&poi:0);
      done = true;
      break;

    default:
      php_iptc_skip_variable(file, spool, poi?&poi:0);
      break;
    }
  }

  file->close();

  if (spool < 2) {
    return String((char *)spoolbuf, poi - spoolbuf, AttachString);
  }
  return true;
}