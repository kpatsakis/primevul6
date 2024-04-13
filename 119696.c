void CLASS parseSonyMakernotes(unsigned tag, unsigned type, unsigned len, unsigned dng_writer,
                               uchar *&table_buf_0x9050, ushort &table_buf_0x9050_present,
                               uchar *&table_buf_0x940c, ushort &table_buf_0x940c_present,
                               uchar *&table_buf_0x0116, ushort &table_buf_0x0116_present,
                               uchar *&table_buf_0x9402, ushort &table_buf_0x9402_present,
                               uchar *&table_buf_0x9403, ushort &table_buf_0x9403_present,
                               uchar *&table_buf_0x9406, ushort &table_buf_0x9406_present)
{

  ushort lid;
  uchar *table_buf;

  if (tag == 0xb001) // Sony ModelID
  {
    unique_id = get2();
    setSonyBodyFeatures(unique_id);

    if (table_buf_0x0116_present) {
      process_Sony_0x0116(table_buf_0x0116, unique_id);
      free(table_buf_0x0116);
      table_buf_0x0116_present = 0;
    }

    if (table_buf_0x9050_present) {
      process_Sony_0x9050(table_buf_0x9050, unique_id);
      free(table_buf_0x9050);
      table_buf_0x9050_present = 0;
    }

    if (table_buf_0x9402_present) {
      process_Sony_0x9402(table_buf_0x9402);
      free(table_buf_0x9402);
      table_buf_0x9402_present = 0;
    }

    if (table_buf_0x9403_present) {
      process_Sony_0x9403(table_buf_0x9403);
      free(table_buf_0x9403);
      table_buf_0x9403_present = 0;
    }

    if (table_buf_0x9406_present) {
      process_Sony_0x9406(table_buf_0x9406);
      free(table_buf_0x9406);
      table_buf_0x9406_present = 0;
    }

    if (table_buf_0x940c_present) {
      if (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E) {
        process_Sony_0x940c(table_buf_0x940c);
      }
      free(table_buf_0x940c);
      table_buf_0x940c_present = 0;
    }
  }

  else if ((tag == 0x0010) && // CameraInfo
           strncasecmp(model, "DSLR-A100", 9) && strncasecmp(model, "NEX-5C", 6) && !strncasecmp(make, "SONY", 4) &&
           ((len == 368) ||  // a700
            (len == 5478) || // a850, a900
            (len == 5506) || // a200, a300, a350
            (len == 6118) || // a230, a290, a330, a380, a390
                             // a450, a500, a550, a560, a580
                             // a33, a35, a55
                             // NEX3, NEX5, NEX5C, NEXC3, VG10E
            (len == 15360)))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    if (memcmp(table_buf, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) &&
        memcmp(table_buf, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
    {
      switch (len)
      {
      case 368:
      case 5478:
        // a700, a850, a900: CameraInfo
        if ((!dng_writer) ||
            (saneSonyCameraInfo(table_buf[0], table_buf[3], table_buf[2], table_buf[5], table_buf[4], table_buf[7])))
        {
          if (table_buf[0] | table_buf[3])
            imgdata.lens.makernotes.MinFocal = bcd2dec(table_buf[0]) * 100 + bcd2dec(table_buf[3]);
          if (table_buf[2] | table_buf[5])
            imgdata.lens.makernotes.MaxFocal = bcd2dec(table_buf[2]) * 100 + bcd2dec(table_buf[5]);
          if (table_buf[4])
            imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[4]) / 10.0f;
          if (table_buf[4])
            imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[7]) / 10.0f;
          parseSonyLensFeatures(table_buf[1], table_buf[6]);
        }
        break;
      default:
        // CameraInfo2 & 3
        if ((!dng_writer) ||
            (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3], table_buf[4], table_buf[5], table_buf[6])))
        {
          if (table_buf[1] | table_buf[2])
            imgdata.lens.makernotes.MinFocal = bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
          if (table_buf[3] | table_buf[4])
            imgdata.lens.makernotes.MaxFocal = bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
          if (table_buf[5])
            imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
          if (table_buf[6])
            imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
          parseSonyLensFeatures(table_buf[0], table_buf[7]);
        }
      }
    }
    free(table_buf);
  }

  else if ((!dng_writer) && (tag == 0x0020) && // WBInfoA100, needs 0xb028 processing
           !strncasecmp(model, "DSLR-A100", 9))
  {
    fseek(ifp, 0x49dc, SEEK_CUR);
    stmread(imgdata.shootinginfo.InternalBodySerial, 12, ifp);
  }

  else if (tag == 0x0104)
  {
    imgdata.other.FlashEC = getreal(type);
  }

  else if (tag == 0x0105) // Teleconverter
  {
    imgdata.lens.makernotes.TeleconverterID = get2();
  }

  else if (tag == 0x0114 && len < 256000) // CameraSettings
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    switch (len)
    {
    case 280:
    case 364:
    case 332:
      // CameraSettings and CameraSettings2 are big endian
      if (table_buf[2] | table_buf[3])
      {
        lid = (((ushort)table_buf[2]) << 8) | ((ushort)table_buf[3]);
        imgdata.lens.makernotes.CurAp = powf64(2.0f, ((float)lid / 8.0f - 1.0f) / 2.0f);
      }
      break;
    case 1536:
    case 2048:
      // CameraSettings3 are little endian
      parseSonyLensType2(table_buf[1016], table_buf[1015]);
      if (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Canon_EF)
      {
        switch (table_buf[153])
        {
        case 16:
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Minolta_A;
          break;
        case 17:
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E;
          break;
        }
      }
      break;
    }
    free(table_buf);
  }

  else if (tag == 0x0116  && len < 256000) {
    table_buf_0x0116 = (uchar *)malloc(len);
    table_buf_0x0116_present = 1;
    fread(table_buf_0x0116, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID) {
      process_Sony_0x0116 (table_buf_0x0116, unique_id);
      free(table_buf_0x0116);
      table_buf_0x0116_present = 0;
    }
  }

  else if (tag == 0x9050 && len < 256000) // little endian
  {
    table_buf_0x9050 = (uchar *)malloc(len);
    table_buf_0x9050_present = 1;
    fread(table_buf_0x9050, len, 1, ifp);

    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x9050(table_buf_0x9050, imgdata.lens.makernotes.CamID);
      free(table_buf_0x9050);
      table_buf_0x9050_present = 0;
    }
  }

  else if (tag == 0x9402 && len < 256000) {
    table_buf_0x9402 = (uchar *)malloc(len);
    table_buf_0x9402_present = 1;
    fread(table_buf_0x9402, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID) {
      process_Sony_0x9402(table_buf_0x9402);
      free(table_buf_0x9402);
      table_buf_0x9402_present = 0;
    }
  }

    else if (tag == 0x9403 && len < 256000) {
    table_buf_0x9403 = (uchar *)malloc(len);
    table_buf_0x9403_present = 1;
    fread(table_buf_0x9403, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID) {
      process_Sony_0x9403(table_buf_0x9403);
      free(table_buf_0x9403);
      table_buf_0x9403_present = 0;
    }
  }

  else if (tag == 0x9406 && len < 256000) {
    table_buf_0x9406 = (uchar *)malloc(len);
    table_buf_0x9406_present = 1;
    fread(table_buf_0x9406, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID) {
      process_Sony_0x9406(table_buf_0x9406);
      free(table_buf_0x9406);
      table_buf_0x9406_present = 0;
    }
  }

  else if (tag == 0x940c && len < 256000)
  {
    table_buf_0x940c = (uchar *)malloc(len);
    table_buf_0x940c_present = 1;
    fread(table_buf_0x940c, len, 1, ifp);
    if ((imgdata.lens.makernotes.CamID) && (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E))
    {
      process_Sony_0x940c(table_buf_0x940c);
      free(table_buf_0x940c);
      table_buf_0x940c_present = 0;
    }
  }

  else if (((tag == 0xb027) || (tag == 0x010c)) && (imgdata.lens.makernotes.LensID == -1))
  {
    imgdata.lens.makernotes.LensID = get4();
    if ((imgdata.lens.makernotes.LensID > 0x4900) && (imgdata.lens.makernotes.LensID <= 0x5900))
    {
      imgdata.lens.makernotes.AdapterID = 0x4900;
      imgdata.lens.makernotes.LensID -= imgdata.lens.makernotes.AdapterID;
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sigma_X3F;
      strcpy(imgdata.lens.makernotes.Adapter, "MC-11");
    }

    else if ((imgdata.lens.makernotes.LensID > 0xEF00) && (imgdata.lens.makernotes.LensID < 0xFFFF) &&
             (imgdata.lens.makernotes.LensID != 0xFF00))
    {
      imgdata.lens.makernotes.AdapterID = 0xEF00;
      imgdata.lens.makernotes.LensID -= imgdata.lens.makernotes.AdapterID;
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
    }
    if (tag == 0x010c)
      imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Minolta_A;
  }

  else if (tag == 0xb02a && len < 256000) // Sony LensSpec
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    if ((!dng_writer) ||
        (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3], table_buf[4], table_buf[5], table_buf[6])))
    {
      if (table_buf[1] | table_buf[2])
        imgdata.lens.makernotes.MinFocal = bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
      if (table_buf[3] | table_buf[4])
        imgdata.lens.makernotes.MaxFocal = bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
      if (table_buf[5])
        imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
      if (table_buf[6])
        imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
      parseSonyLensFeatures(table_buf[0], table_buf[7]);
    }
    free(table_buf);
  }
}