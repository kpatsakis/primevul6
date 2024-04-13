void CLASS parse_makernote_0xc634(int base, int uptag, unsigned dng_writer)
{
/*
#ifdef LIBRAW_LIBRARY_BUILD
  if (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SKIP_MAKERNOTES)
  	return;
#endif
*/
  unsigned ver97 = 0, offset = 0, entries, tag, type, len, save, c;
  unsigned i;

  uchar NikonKey, ci, cj, ck;
  unsigned serial = 0;
  unsigned custom_serial = 0;
  unsigned NikonLensDataVersion = 0;
  unsigned lenNikonLensData = 0;
  unsigned NikonFlashInfoVersion = 0;

  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;
  unsigned typeCanonCameraInfo = 0;

  uchar *table_buf;
  uchar *table_buf_0x0116;
  ushort table_buf_0x0116_len = 0;
  uchar *table_buf_0x2010;
  ushort table_buf_0x2010_len = 0;
  uchar *table_buf_0x9050;
  ushort table_buf_0x9050_len = 0;
  uchar *table_buf_0x9400;
  ushort table_buf_0x9400_len = 0;
  uchar *table_buf_0x9402;
  ushort table_buf_0x9402_len = 0;
  uchar *table_buf_0x9403;
  ushort table_buf_0x9403_len = 0;
  uchar *table_buf_0x9406;
  ushort table_buf_0x9406_len = 0;
  uchar *table_buf_0x940c;
  ushort table_buf_0x940c_len = 0;
  uchar *table_buf_0x940e;
  ushort table_buf_0x940e_len = 0;

  short morder, sorder = order;
  char buf[10];
  INT64 fsize = ifp->size();

  fread(buf, 1, 10, ifp);

  if (!strcmp(buf, "Nikon"))
  {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42)
      goto quit;
    offset = get4();
    fseek(ifp, offset - 8, SEEK_CUR);
  }
  else if (!strcmp(buf, "OLYMPUS") || !strcmp(buf, "PENTAX ") ||
           (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG)))
  {
    base = ftell(ifp) - 10;
    fseek(ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O')
      get2();
  }
  else if (!strncmp(buf, "SONY", 4) || !strcmp(buf, "Panasonic"))
  {
    goto nf;
  }
  else if (!strncmp(buf, "FUJIFILM", 8))
  {
    base = ftell(ifp) - 10;
  nf:
    order = 0x4949;
    fseek(ifp, 2, SEEK_CUR);
  }
  else if (!strcmp(buf, "OLYMP") || !strcmp(buf, "LEICA") || !strcmp(buf, "Ricoh") || !strcmp(buf, "EPSON"))
    fseek(ifp, -2, SEEK_CUR);
  else if (!strcmp(buf, "AOC") || !strcmp(buf, "QVC"))
    fseek(ifp, -4, SEEK_CUR);
  else
  {
    fseek(ifp, -10, SEEK_CUR);
    if ((!strncmp(make, "SAMSUNG", 7) && (dng_writer == AdobeDNG)))
      base = ftell(ifp);
  }

  entries = get2();
  if (entries > 1000)
    return;
  morder = order;

  while (entries--)
  {
    order = morder;
    tiff_get(base, &tag, &type, &len, &save);
    INT64 pos = ifp->tell();
    if (len > 8 && pos + len > 2 * fsize)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    tag |= uptag << 16;
    if (len > 100 * 1024 * 1024)
      goto next; // 100Mb tag? No!

    if (!strncmp(make, "Canon", 5))
    {
      if (tag == 0x000d && len < 256000) // camera info
      {
        if (type != 4)
        {
          CanonCameraInfo = (uchar *)malloc(MAX(16, len));
          fread(CanonCameraInfo, len, 1, ifp);
        }
        else
        {
          CanonCameraInfo = (uchar *)malloc(MAX(16, len * 4));
          fread(CanonCameraInfo, len, 4, ifp);
        }
        lenCanonCameraInfo = len;
        typeCanonCameraInfo = type;
      }

      else if (tag == 0x10) // Canon ModelID
      {
        unique_id = get4();
        unique_id = setCanonBodyFeatures(unique_id);
        if (lenCanonCameraInfo)
        {
          processCanonCameraInfo(unique_id, CanonCameraInfo, lenCanonCameraInfo, typeCanonCameraInfo);
          free(CanonCameraInfo);
          CanonCameraInfo = 0;
          lenCanonCameraInfo = 0;
        }
      }

      else
        parseCanonMakernotes(tag, type, len);
    }

    else if (!strncmp(make, "FUJI", 4))
      parseFujiMakernotes(tag, type, len, AdobeDNG);

    else if (!strncasecmp(make, "LEICA", 5))
    {

      if ((tag == 0x0320) && (type == 9) && (len == 1) && !strncasecmp(make, "Leica Camera AG", 15) &&
          !strncmp(buf, "LEICA", 5) && (buf[5] == 0) && (buf[6] == 0) && (buf[7] == 0))
        imgdata.other.CameraTemperature = getreal(type);

      if (tag == 0x34003402)
        imgdata.other.CameraTemperature = getreal(type);

      if (((tag == 0x035e) || (tag == 0x035f)) && (type == 10) && (len == 9))
      {
        int ind = tag == 0x035e ? 0 : 1;
        for (int j = 0; j < 3; j++)
          FORCC imgdata.color.dng_color[ind].forwardmatrix[j][c] = getreal(type);
        imgdata.color.dng_color[ind].parsedfields |= LIBRAW_DNGFM_FORWARDMATRIX;
      }
      if ((tag == 0x0303) && (type != 4))
      {
        stmread(imgdata.lens.makernotes.Lens, len, ifp);
      }

      if ((tag == 0x3405) || (tag == 0x0310) || (tag == 0x34003405))
      {
        imgdata.lens.makernotes.LensID = get4();
        imgdata.lens.makernotes.LensID =
            ((imgdata.lens.makernotes.LensID >> 2) << 8) | (imgdata.lens.makernotes.LensID & 0x3);
        if (imgdata.lens.makernotes.LensID != -1)
        {
          if ((model[0] == 'M') || !strncasecmp(model, "LEICA M", 7))
          {
            imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_M;
            if (imgdata.lens.makernotes.LensID)
              imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Leica_M;
          }
          else if ((model[0] == 'S') || !strncasecmp(model, "LEICA S", 7))
          {
            imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_S;
            if (imgdata.lens.makernotes.Lens[0])
              imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Leica_S;
          }
        }
      }

      else if (((tag == 0x0313) || (tag == 0x34003406)) && (fabs(imgdata.lens.makernotes.CurAp) < 0.17f) &&
               ((type == 10) || (type == 5)))
      {
        imgdata.lens.makernotes.CurAp = getreal(type);
        if (imgdata.lens.makernotes.CurAp > 126.3)
          imgdata.lens.makernotes.CurAp = 0.0f;
      }

      else if (tag == 0x3400)
      {
        parse_makernote(base, 0x3400);
      }
    }

    else if (!strncmp(make, "NIKON", 5))
    {
      if (tag == 0x1d) // serial number
        while ((c = fgetc(ifp)) && c != EOF)
        {
          if ((!custom_serial) && (!isdigit(c)))
          {
            if ((strbuflen(model) == 3) && (!strcmp(model, "D50")))
            {
              custom_serial = 34;
            }
            else
            {
              custom_serial = 96;
            }
          }
          serial = serial * 10 + (isdigit(c) ? c - '0' : c % 10);
        }
      else if (tag == 0x000a)
      {
        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
        imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
      }
      else if (tag == 0x0082) // lens attachment
      {
        stmread(imgdata.lens.makernotes.Attachment, len, ifp);
      }
      else if (tag == 0x0083) // lens type
      {
        imgdata.lens.nikon.NikonLensType = fgetc(ifp);
      }
      else if (tag == 0x0084) // lens
      {
        imgdata.lens.makernotes.MinFocal = getreal(type);
        imgdata.lens.makernotes.MaxFocal = getreal(type);
        imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type);
        imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type);
      }
      else if (tag == 0x008b) // lens f-stops
      {
        uchar a, b, c;
        a = fgetc(ifp);
        b = fgetc(ifp);
        c = fgetc(ifp);
        if (c)
        {
          imgdata.lens.nikon.NikonLensFStops = a * b * (12 / c);
          imgdata.lens.makernotes.LensFStops = (float)imgdata.lens.nikon.NikonLensFStops / 12.0f;
        }
      }
      else if (tag == 0x0093)
      {
        imgdata.makernotes.nikon.NEFCompression = i = get2();
        if ((i == 7) || (i == 9))
        {
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
          imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
        }
      }
      else if (tag == 0x0097)
      {
        for (i = 0; i < 4; i++)
          ver97 = ver97 * 10 + fgetc(ifp) - '0';
        if (ver97 == 601) // Coolpix A
        {
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
          imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
        }
      }
      else if (tag == 0x0098) // contains lens data
      {
        for (i = 0; i < 4; i++)
        {
          NikonLensDataVersion = NikonLensDataVersion * 10 + fgetc(ifp) - '0';
        }
        switch (NikonLensDataVersion)
        {
        case 100:
          lenNikonLensData = 9;
          break;
        case 101:
        case 201: // encrypted, starting from v.201
        case 202:
        case 203:
          lenNikonLensData = 15;
          break;
        case 204:
          lenNikonLensData = 16;
          break;
        case 400:
          lenNikonLensData = 459;
          break;
        case 401:
          lenNikonLensData = 590;
          break;
        case 402:
          lenNikonLensData = 509;
          break;
        case 403:
          lenNikonLensData = 879;
          break;
        }
        if (lenNikonLensData)
        {
          table_buf = (uchar *)malloc(lenNikonLensData);
          fread(table_buf, lenNikonLensData, 1, ifp);
          if ((NikonLensDataVersion < 201) && lenNikonLensData)
          {
            processNikonLensData(table_buf, lenNikonLensData);
            free(table_buf);
            lenNikonLensData = 0;
          }
        }
      }

      else if (tag == 0xa7) // shutter count
      {
        NikonKey = fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp);
        if ((NikonLensDataVersion > 200) && lenNikonLensData)
        {
          if (custom_serial)
          {
            ci = xlat[0][custom_serial];
          }
          else
          {
            ci = xlat[0][serial & 0xff];
          }
          cj = xlat[1][NikonKey];
          ck = 0x60;
          for (i = 0; i < lenNikonLensData; i++)
            table_buf[i] ^= (cj += ci * ck++);
          processNikonLensData(table_buf, lenNikonLensData);
          lenNikonLensData = 0;
          free(table_buf);
        }
      }
      else if (tag == 0x00a8) // contains flash data
      {
        for (i = 0; i < 4; i++)
        {
          NikonFlashInfoVersion = NikonFlashInfoVersion * 10 + fgetc(ifp) - '0';
        }
      }

      else if (tag == 0x00b0)
      {
        get4(); // ME tag version, 4 symbols
        imgdata.makernotes.nikon.ExposureMode = get4();
        imgdata.makernotes.nikon.nMEshots = get4();
        imgdata.makernotes.nikon.MEgainOn = get4();
      }

      else if (tag == 0x00b9)
      {
        uchar uc;
        int8_t sc;
        fread(&uc, 1, 1, ifp);
        imgdata.makernotes.nikon.AFFineTune = uc;
        fread(&uc, 1, 1, ifp);
        imgdata.makernotes.nikon.AFFineTuneIndex = uc;
        fread(&sc, 1, 1, ifp);
        imgdata.makernotes.nikon.AFFineTuneAdj = sc;
      }

      else if (tag == 37 && (!iso_speed || iso_speed == 65535))
      {
        unsigned char cc;
        fread(&cc, 1, 1, ifp);
        iso_speed = (int)(100.0 * libraw_powf64l(2.0, (double)(cc) / 12.0 - 5.0));
        break;
      }
    }

    else if (!strncmp(make, "OLYMPUS", 7)) {

      int SubDirOffsetValid =
          strncmp(model, "E-300", 5) &&
          strncmp(model, "E-330", 5) &&
          strncmp(model, "E-400", 5) &&
          strncmp(model, "E-500", 5) &&
          strncmp(model, "E-1", 3);

      if ((tag == 0x2010) ||
          (tag == 0x2020) ||
          (tag == 0x2030) ||
          (tag == 0x2031) ||
          (tag == 0x2040) ||
          (tag == 0x2050) ||
          (tag == 0x3000)) {
        fseek(ifp, save - 4, SEEK_SET);
        fseek(ifp, base + get4(), SEEK_SET);
        parse_makernote_0xc634(base, tag, dng_writer);
      }

      if (!SubDirOffsetValid &&
          ((len > 4) ||
           (((type == 3) || (type == 8)) && (len > 2)) ||
           (((type == 4) || (type == 9)) && (len > 1)) ||
           (type == 5) ||
           (type > 9)))
        goto skip_Oly_broken_tags;

      else if ((tag >= 0x20100000) && (tag <= 0x2010ffff))
        parseOlympus_Equipment ((tag & 0x0000ffff), type, len, AdobeDNG);

      else if ((tag >= 0x20200000) && (tag <= 0x2020ffff))
        parseOlympus_CameraSettings (base, (tag & 0x0000ffff), type, len, AdobeDNG);

      else if ((tag == 0x20300108) || (tag == 0x20310109))
        imgdata.makernotes.olympus.ColorSpace = get2();

      else if ((tag >= 0x20400000) && (tag <= 0x2040ffff))
        parseOlympus_ImageProcessing ((tag & 0x0000ffff), type, len, AdobeDNG);

      else if ((tag >= 0x30000000) && (tag <= 0x3000ffff))
        parseOlympus_RawInfo ((tag & 0x0000ffff), type, len, AdobeDNG);

      else switch (tag) {
      case 0x0207:
        getOlympus_CameraType2();
        break;
      case 0x1002:
        imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, getreal(type) / 2);
        break;
      case 0x1007:
        imgdata.other.SensorTemperature = (float)get2();
        break;
      case 0x1008:
        imgdata.other.LensTemperature = (float)get2();
        break;

      case 0x20501500:
        getOlympus_SensorTemperature(len);
        break;
      }

skip_Oly_broken_tags:;
    }

    else if (!strncmp(make, "PENTAX", 6) || !strncmp(model, "PENTAX", 6) ||
             (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG)))
    {
      parsePentaxMakernotes(base, tag, type, len, dng_writer);
    }

    else if (!strncmp(make, "SAMSUNG", 7) && (dng_writer == AdobeDNG))
    {
      parseSamsungMakernotes(base, tag, type, len, dng_writer);
    }

    else if (!strncasecmp(make, "SONY", 4)    ||
             !strncasecmp(make, "Konica", 6)  ||
             !strncasecmp(make, "Minolta", 7) ||
             (!strncasecmp(make, "Hasselblad", 10) &&
              (!strncasecmp(model, "Stellar", 7) ||
               !strncasecmp(model, "Lunar", 5)   ||
               !strncasecmp(model, "Lusso", 5)   ||
               !strncasecmp(model, "HV", 2))))
    {
      parseSonyMakernotes(base, tag, type, len, AdobeDNG,
                          table_buf_0x0116, table_buf_0x0116_len,
                          table_buf_0x2010, table_buf_0x2010_len,
                          table_buf_0x9050, table_buf_0x9050_len,
                          table_buf_0x9400, table_buf_0x9400_len,
                          table_buf_0x9402, table_buf_0x9402_len,
                          table_buf_0x9403, table_buf_0x9403_len,
                          table_buf_0x9406, table_buf_0x9406_len,
                          table_buf_0x940c, table_buf_0x940c_len,
                          table_buf_0x940e, table_buf_0x940e_len);
    }
  next:
    fseek(ifp, save, SEEK_SET);
  }
quit:
  order = sorder;
}