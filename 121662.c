void CLASS parseSonyMakernotes(int base, unsigned tag, unsigned type, unsigned len, unsigned dng_writer,
                               uchar *&table_buf_0x0116, ushort &table_buf_0x0116_len,
                               uchar *&table_buf_0x2010, ushort &table_buf_0x2010_len,
                               uchar *&table_buf_0x9050, ushort &table_buf_0x9050_len,
                               uchar *&table_buf_0x9400, ushort &table_buf_0x9400_len,
                               uchar *&table_buf_0x9402, ushort &table_buf_0x9402_len,
                               uchar *&table_buf_0x9403, ushort &table_buf_0x9403_len,
                               uchar *&table_buf_0x9406, ushort &table_buf_0x9406_len,
                               uchar *&table_buf_0x940c, ushort &table_buf_0x940c_len,
                               uchar *&table_buf_0x940e, ushort &table_buf_0x940e_len)
{

  ushort lid;
  uchar *table_buf;

  if (tag == 0xb001) // Sony ModelID
  {
    unique_id = get2();
    setSonyBodyFeatures(unique_id);

    if (table_buf_0x0116_len)
    {
      process_Sony_0x0116(table_buf_0x0116, table_buf_0x0116_len, unique_id);
      free(table_buf_0x0116);
      table_buf_0x0116_len = 0;
    }

    if (table_buf_0x2010_len)
    {
      process_Sony_0x2010(table_buf_0x2010, table_buf_0x2010_len);
      free(table_buf_0x2010);
      table_buf_0x2010_len = 0;
    }

    if (table_buf_0x9050_len)
    {
      process_Sony_0x9050(table_buf_0x9050, table_buf_0x9050_len, unique_id);
      free(table_buf_0x9050);
      table_buf_0x9050_len = 0;
    }

    if (table_buf_0x9400_len)
    {
      process_Sony_0x9400(table_buf_0x9400, table_buf_0x9400_len, unique_id);
      free(table_buf_0x9400);
      table_buf_0x9400_len = 0;
    }

    if (table_buf_0x9402_len)
    {
      process_Sony_0x9402(table_buf_0x9402, table_buf_0x9402_len);
      free(table_buf_0x9402);
      table_buf_0x9402_len = 0;
    }

    if (table_buf_0x9403_len)
    {
      process_Sony_0x9403(table_buf_0x9403, table_buf_0x9403_len);
      free(table_buf_0x9403);
      table_buf_0x9403_len = 0;
    }

    if (table_buf_0x9406_len)
    {
      process_Sony_0x9406(table_buf_0x9406, table_buf_0x9406_len);
      free(table_buf_0x9406);
      table_buf_0x9406_len = 0;
    }

    if (table_buf_0x940c_len)
    {
      process_Sony_0x940c(table_buf_0x940c, table_buf_0x940c_len);
      free(table_buf_0x940c);
      table_buf_0x940c_len = 0;
    }

    if (table_buf_0x940e_len)
    {
      process_Sony_0x940e(table_buf_0x940e, table_buf_0x940e_len, unique_id);
      free(table_buf_0x940e);
      table_buf_0x940e_len = 0;
    }
  }

  else if (((tag == 0x0001)  ||  // Minolta CameraSettings, big endian
            (tag == 0x0003)) &&
           (len >= 196))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);

    lid = 0x01<<2;
      imgdata.shootinginfo.ExposureMode =
          (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
          (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x06<<2;
      imgdata.shootinginfo.DriveMode =
          (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
          (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x07<<2;
      imgdata.shootinginfo.MeteringMode =
          (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
          (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x25<<2;
      imgdata.makernotes.sony.MinoltaCamID =
          (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
          (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    lid = 0x30<<2;
      imgdata.shootinginfo.FocusMode =
          (unsigned)table_buf[lid] << 24 | (unsigned)table_buf[lid + 1] << 16 |
          (unsigned)table_buf[lid + 2] << 8 | (unsigned)table_buf[lid + 3];

    free(table_buf);
  }

  else if ((tag == 0x0004)  &&  // Minolta CameraSettings7D, big endian
           (len >= 227))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);

    lid = 0x0;
      imgdata.shootinginfo.ExposureMode =
          (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    lid = 0x0e<<1;
      imgdata.shootinginfo.FocusMode =
          (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    lid = 0x10<<1;
      imgdata.shootinginfo.AFPoint =
          (ushort)table_buf[lid] << 8 | (ushort)table_buf[lid + 1];

    lid = 0x71<<1;
      imgdata.shootinginfo.ImageStabilization =
          (ushort)table_buf[lid] << 24 | (ushort)table_buf[lid + 1];

    free(table_buf);
  }

  else if ((tag == 0x0010) && // CameraInfo
           strncasecmp(model, "DSLR-A100", 9) &&
           strncasecmp(model, "NEX-5C", 6) &&
           !strncasecmp(make, "SONY", 4) &&
           ((len == 368)  || // a700
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

          if (len == 5478)
          {
            imgdata.makernotes.sony.AFMicroAdjValue = table_buf[304] - 20;
            imgdata.makernotes.sony.AFMicroAdjOn = (((table_buf[305] & 0x80) == 0x80) ? 1 : 0);
            imgdata.makernotes.sony.AFMicroAdjRegisteredLenses = table_buf[305] & 0x7f;
          }
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

          if (!strncasecmp(model, "DSLR-A450", 9) ||
              !strncasecmp(model, "DSLR-A500", 9) ||
              !strncasecmp(model, "DSLR-A550", 9))
          imgdata.shootinginfo.FocusMode = table_buf[0x15];
        }
      }
    }
    free(table_buf);
  }

  else if ((!dng_writer) &&
           ((tag == 0x0020) ||
            (tag == 0xb0280020)))
  {
    if (!strncasecmp(model, "DSLR-A100", 9))  // WBInfoA100
    {
      fseek(ifp, 0x49dc, SEEK_CUR);
      stmread(imgdata.shootinginfo.InternalBodySerial, 13, ifp);
    }
    else if ((len == 19154) ||
             (len == 19148))
    {
      table_buf = (uchar *)malloc(128);
      fread(table_buf, 128, 1, ifp);
      imgdata.shootinginfo.DriveMode = table_buf[14];
      imgdata.shootinginfo.ImageStabilization = table_buf[20];
      imgdata.shootinginfo.ExposureProgram = table_buf[63];
      free(table_buf);
    }
    else
    {
      ushort a = get2();
      ushort b = get2();
      ushort c = get2();
      ushort d = get2();
      if ((a) && (c == 1))
      {
        fseek(ifp, d-8, SEEK_CUR);
        table_buf = (uchar *)malloc(256);
        fread(table_buf, 256, 1, ifp);
        imgdata.shootinginfo.DriveMode = table_buf[1];
        imgdata.shootinginfo.ExposureProgram = table_buf[2];
        imgdata.shootinginfo.MeteringMode = table_buf[3];
        if (strncasecmp(model, "DSLR-A450", 9) &&
            strncasecmp(model, "DSLR-A500", 9) &&
            strncasecmp(model, "DSLR-A550", 9))
          imgdata.shootinginfo.FocusMode = table_buf[19];
        free(table_buf);
      }
    }
  }

  else if (tag == 0x0104)
  {
    imgdata.other.FlashEC = getreal(type);
  }

  else if (tag == 0x0105) // Teleconverter
  {
    imgdata.lens.makernotes.TeleconverterID = get4();
  }

  else if (tag == 0x0107)
  {
    imgdata.shootinginfo.ImageStabilization = get4();
  }

  else if ((tag == 0xb0280088) && (dng_writer == nonDNG))
  {
    thumb_offset = get4() + base;
  }

  else if ((tag == 0xb0280089) && (dng_writer == nonDNG))
  {
    thumb_length = get4();
  }

  else if (((tag == 0x0114) ||     // CameraSettings
            (tag == 0xb0280114)) &&
           (len < 256000))
  {
    table_buf = (uchar *)malloc(len);
    fread(table_buf, len, 1, ifp);
    switch (len)
    {
    case 260:  // Sony A100, big endian
      imgdata.shootinginfo.ExposureMode =
          ((ushort)table_buf[0]) << 8 | ((ushort)table_buf[1]);
      lid = 0x0a<<1;
      imgdata.shootinginfo.DriveMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0x0c<<1;
      imgdata.shootinginfo.FocusMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0x0d<<1;
      imgdata.shootinginfo.AFPoint =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0x12<<1;
      imgdata.shootinginfo.MeteringMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0x57<<1;
      imgdata.shootinginfo.ImageStabilization =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      break;
    case 448:  // Minolta "DYNAX 5D" / "MAXXUM 5D" / "ALPHA SWEET", big endian
      lid = 0x0a<<1;
      imgdata.shootinginfo.ExposureMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0x25<<1;
      imgdata.shootinginfo.MeteringMode =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0xbd<<1;
      imgdata.shootinginfo.ImageStabilization =
          ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      break;
    case 280:
    case 364:
      // CameraSettings and CameraSettings2 are big endian
      if (table_buf[2] | table_buf[3])
      {
        lid = (((ushort)table_buf[2]) << 8) | ((ushort)table_buf[3]);
        imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, ((float)lid / 8.0f - 1.0f) / 2.0f);
      }
      lid = 0x04<<1;
      imgdata.shootinginfo.DriveMode = table_buf[lid+1];
      lid = 0x4d<<1;
      imgdata.shootinginfo.FocusMode = ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      break;
    case 332:
      // CameraSettings and CameraSettings2 are big endian
      if (table_buf[2] | table_buf[3])
      {
        lid = (((ushort)table_buf[2]) << 8) | ((ushort)table_buf[3]);
        imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, ((float)lid / 8.0f - 1.0f) / 2.0f);
      }
      lid = 0x4d<<1;
      imgdata.shootinginfo.FocusMode = ((ushort)table_buf[lid]) << 8 | ((ushort)table_buf[lid+1]);
      lid = 0x7e<<1;
      imgdata.shootinginfo.DriveMode = table_buf[lid+1];
      break;
    case 1536:
    case 2048:
      // CameraSettings3 are little endian
      imgdata.shootinginfo.DriveMode = table_buf[0x34];
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

  else if ((tag == 0x3000) && (len < 256000))
  {
    uchar *table_buf_0x3000;
    table_buf_0x3000 = (uchar *)malloc(len);
    fread(table_buf_0x3000, len, 1, ifp);
    for (int i = 0; i < 20; i++)
      imgdata.makernotes.sony.SonyDateTime[i] = table_buf_0x3000[6 + i];
  }

  else if (tag == 0x0116 && len < 256000)
  {
    table_buf_0x0116 = (uchar *)malloc(len);
    table_buf_0x0116_len = len;
    fread(table_buf_0x0116, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x0116(table_buf_0x0116, table_buf_0x0116_len, imgdata.lens.makernotes.CamID);
      free(table_buf_0x0116);
      table_buf_0x0116_len = 0;
    }
  }

  else if (tag == 0x2010 && len < 256000)
  {
    table_buf_0x2010 = (uchar *)malloc(len);
    table_buf_0x2010_len = len;
    fread(table_buf_0x2010, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x2010(table_buf_0x2010, table_buf_0x2010_len);
      free(table_buf_0x2010);
      table_buf_0x2010_len = 0;
    }
  }

  else if (tag == 0x201a)
  {
    imgdata.makernotes.sony.ElectronicFrontCurtainShutter = get4();
  }

  else if (tag == 0x201b)
  {
    uchar uc;
    fread(&uc, 1, 1, ifp);
    imgdata.shootinginfo.FocusMode = (short)uc;
  }

  else if (tag == 0x202c)
  {
    imgdata.makernotes.sony.MeteringMode2 = get2();
  }

  else if (tag == 0x202f)
  {
    imgdata.makernotes.sony.PixelShiftGroupID = get4();
    imgdata.makernotes.sony.PixelShiftGroupPrefix = imgdata.makernotes.sony.PixelShiftGroupID >> 22;
    imgdata.makernotes.sony.PixelShiftGroupID =
      ((imgdata.makernotes.sony.PixelShiftGroupID >> 17) & (unsigned) 0x1f) * (unsigned) 1000000 +
      ((imgdata.makernotes.sony.PixelShiftGroupID >> 12) & (unsigned) 0x1f) * (unsigned) 10000 +
      ((imgdata.makernotes.sony.PixelShiftGroupID >>  6) & (unsigned) 0x3f) * (unsigned) 100 +
      (imgdata.makernotes.sony.PixelShiftGroupID         & (unsigned) 0x3f);

    imgdata.makernotes.sony.numInPixelShiftGroup = fgetc(ifp);
    imgdata.makernotes.sony.nShotsInPixelShiftGroup = fgetc(ifp);
  }

  else if (tag == 0x9050 && len < 256000) // little endian
  {
    table_buf_0x9050 = (uchar *)malloc(len);
    table_buf_0x9050_len = len;
    fread(table_buf_0x9050, len, 1, ifp);

    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x9050(table_buf_0x9050, table_buf_0x9050_len, imgdata.lens.makernotes.CamID);
      free(table_buf_0x9050);
      table_buf_0x9050_len = 0;
    }
  }

  else if (tag == 0x9400 && len < 256000)
  {
    table_buf_0x9400 = (uchar *)malloc(len);
    table_buf_0x9400_len = len;
    fread(table_buf_0x9400, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x9400(table_buf_0x9400, table_buf_0x9400_len, unique_id);
      free(table_buf_0x9400);
      table_buf_0x9400_len = 0;
    }
  }

  else if (tag == 0x9402 && len < 256000)
  {
    table_buf_0x9402 = (uchar *)malloc(len);
    table_buf_0x9402_len = len;
    fread(table_buf_0x9402, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x9402(table_buf_0x9402, table_buf_0x9402_len);
      free(table_buf_0x9402);
      table_buf_0x9402_len = 0;
    }
  }

  else if (tag == 0x9403 && len < 256000)
  {
    table_buf_0x9403 = (uchar *)malloc(len);
    table_buf_0x9403_len = len;
    fread(table_buf_0x9403, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x9403(table_buf_0x9403, table_buf_0x9403_len);
      free(table_buf_0x9403);
      table_buf_0x9403_len = 0;
    }
  }

  else if ((tag == 0x9405) && (len < 256000) && (len > 0x64))
  {
    uchar *table_buf_0x9405;
    table_buf_0x9405 = (uchar *)malloc(len);
    fread(table_buf_0x9405, len, 1, ifp);
    uchar bufx = table_buf_0x9405[0x0];
    if (imgdata.other.real_ISO < 0.1f)
    {
      if ((bufx == 0x25) || (bufx == 0x3a) || (bufx == 0x76) || (bufx == 0x7e) || (bufx == 0x8b) || (bufx == 0x9a) ||
          (bufx == 0xb3) || (bufx == 0xe1))
      {
        uchar s[2];
        s[0] = SonySubstitution[table_buf_0x9405[0x04]];
        s[1] = SonySubstitution[table_buf_0x9405[0x05]];
        imgdata.other.real_ISO = 100.0f * libraw_powf64l(2.0f, (16 - ((float)sget2(s)) / 256.0f));
      }
    }
    free(table_buf_0x9405);
  }

  else if (tag == 0x9406 && len < 256000)
  {
    table_buf_0x9406 = (uchar *)malloc(len);
    table_buf_0x9406_len = len;
    fread(table_buf_0x9406, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x9406(table_buf_0x9406, table_buf_0x9406_len);
      free(table_buf_0x9406);
      table_buf_0x9406_len = 0;
    }
  }

  else if (tag == 0x940c && len < 256000)
  {
    table_buf_0x940c = (uchar *)malloc(len);
    table_buf_0x940c_len = len;
    fread(table_buf_0x940c, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x940c(table_buf_0x940c, table_buf_0x940c_len);
      free(table_buf_0x940c);
      table_buf_0x940c_len = 0;
    }
  }

  else if (tag == 0x940e && len < 256000)
  {
    table_buf_0x940e = (uchar *)malloc(len);
    table_buf_0x940e_len = len;
    fread(table_buf_0x940e, len, 1, ifp);
    if (imgdata.lens.makernotes.CamID)
    {
      process_Sony_0x940e(table_buf_0x940e, table_buf_0x940e_len, imgdata.lens.makernotes.CamID);
      free(table_buf_0x940e);
      table_buf_0x940e_len = 0;
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

  else if ((tag == 0xb02b) && !imgdata.sizes.raw_crop.cwidth && (len == 2))
  {
    imgdata.sizes.raw_crop.cheight = get4();
    imgdata.sizes.raw_crop.cwidth = get4();
  }

  else if (tag == 0xb041)
  {
    imgdata.shootinginfo.ExposureMode = get2();
  }

// MetaVersion: (unique_id >= 286)

}