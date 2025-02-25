void CLASS parse_kodak_ifd(int base)
{
  unsigned entries, tag, type, len, save;
  int j, c, wbi = -1;
  float mul[3] = {1, 1, 1}, num;

  static const int wbtag_kdc[] = {
    LIBRAW_WBI_Auto,         // 64037 / 0xfa25
    LIBRAW_WBI_Fluorescent,  // 64040 / 0xfa28
    LIBRAW_WBI_Tungsten,     // 64039 / 0xfa27
    LIBRAW_WBI_Daylight,     // 64041 / 0xfa29
    -1,
    -1,
    LIBRAW_WBI_Shade         // 64042 / 0xfa2a
  };

  static const int wbtag_dcr[] = {
    LIBRAW_WBI_Daylight,     // 2120 / 0x0848
    LIBRAW_WBI_Tungsten,     // 2121 / 0x0849
    LIBRAW_WBI_Fluorescent,  // 2122 / 0x084a
    LIBRAW_WBI_Flash,        // 2123 / 0x084b
    LIBRAW_WBI_Custom,       // 2124 / 0x084c
    LIBRAW_WBI_Auto          // 2125 / 0x084d
  };

//  int a_blck = 0;

  entries = get2();
  if (entries > 1024)
    return;
  INT64 fsize = ifp->size();
  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
    INT64 savepos = ftell(ifp);
    if (len > 8 && len + savepos > 2 * fsize)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    if (callbacks.exif_cb)
    {
      callbacks.exif_cb(callbacks.exifparser_data, tag | 0x20000, type, len, order, ifp);
      fseek(ifp, savepos, SEEK_SET);
    }
    if (tag == 1003)       // 1003
      imgdata.sizes.raw_crop.cleft = get2();
    else if (tag == 1004)  // 1004
      imgdata.sizes.raw_crop.ctop = get2();
    else if (tag == 1005)  // 1005
      imgdata.sizes.raw_crop.cwidth = get2();
    else if (tag == 1006)  // 1006
      imgdata.sizes.raw_crop.cheight = get2();
    else if (tag == 1007)  // 1007 / 0x03ef
    {
      if (!strcmp(model, "EOS D2000C"))
        black = get2();
      else
        imgdata.makernotes.kodak.BlackLevelTop = get2();
    }
    else if (tag == 1008)  // 1008 / 0x03f0
    {
      if (!strcmp(model, "EOS D2000C"))
      {
        if (black) // already set by tag 1007 (0x03ef)
          black = (black + get2()) / 2;
        else
          black = get2();
      }
      else
        imgdata.makernotes.kodak.BlackLevelBottom = get2();
    }
    else if (tag == 1011)  // 1011
      imgdata.other.FlashEC = getreal(type);

    else if (tag == 1020)  // 1020
    {
      wbi = getint(type);
      if ((wbi >= 0) && (wbi < 6) && (wbi != -2)) wbi = wbtag_dcr[wbi];
    }
    else if (tag == 1021 && len == 72)     // 1021
    { /* WB set in software */
      fseek(ifp, 40, SEEK_CUR);
      FORC3 cam_mul[c] = 2048.0 / fMAX(1.0f, get2());
      wbi = -2;
    }

    else if ((tag == 1030) && (len == 1))  // 1030
      imgdata.other.CameraTemperature = getreal(type);
    else if ((tag == 1043) && (len == 1))  // 1043
      imgdata.other.SensorTemperature = getreal(type);
    else if (tag == 0x0848)  // 2120
      Kodak_DCR_WBtags(LIBRAW_WBI_Daylight, type, wbi);
    else if (tag == 0x0849)  // 2121
      Kodak_DCR_WBtags(LIBRAW_WBI_Tungsten, type, wbi);
    else if (tag == 0x084a)  // 2122
      Kodak_DCR_WBtags(LIBRAW_WBI_Fluorescent, type, wbi);
    else if (tag == 0x084b)  // 2123
      Kodak_DCR_WBtags(LIBRAW_WBI_Flash, type, wbi);
    else if (tag == 0x084c)  // 2124
      Kodak_DCR_WBtags(LIBRAW_WBI_Custom, type, wbi);
    else if (tag == 0x084d)  // 2125
    {
      if (wbi == -1) wbi = LIBRAW_WBI_Auto;
      Kodak_DCR_WBtags(LIBRAW_WBI_Auto, type, wbi);
    }
    else if (tag == 0x0903)  // 2307
      iso_speed = getreal(type);
    else if (tag == 2317)    // 2317
      linear_table(len);
    else if (tag == 0x09ce)  // 2510
      stmread(imgdata.shootinginfo.InternalBodySerial, len, ifp);
    else if (tag == 0x0e92)  // 3730
    {
      imgdata.makernotes.kodak.val018percent = get2();
      imgdata.color.linear_max[0] = imgdata.color.linear_max[1] =
          imgdata.color.linear_max[2] = imgdata.color.linear_max[3] =
          (int)(((float)imgdata.makernotes.kodak.val018percent) / 18.0f * 170.0f);
    }
    else if (tag == 0x0e93)  // 3731
      imgdata.color.linear_max[0] = imgdata.color.linear_max[1] =
          imgdata.color.linear_max[2] = imgdata.color.linear_max[3] =
          imgdata.makernotes.kodak.val170percent = get2();
    else if (tag == 0x0e94)  // 3732
      imgdata.makernotes.kodak.val100percent = get2();
/*
    else if (tag == 6020)    // 6020
      iso_speed = getint(type);
*/
    else if (tag == 0xfa00)  // 64000
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
    else if (tag == 0xfa0d)  // 64013
    {
      wbi = fgetc(ifp);
      if ((wbi >= 0) && (wbi < 7)) wbi = wbtag_kdc[wbi];
    }
    else if (tag == 0xfa13)  // 64019
      width = getint(type);
    else if (tag == 0xfa14)  // 64020
      height = (getint(type) + 1) & -2;
/*
      height = getint(type);

    else if (tag == 0xfa16)  // 64022
      raw_width = get2();
    else if (tag == 0xfa17)  // 64023
      raw_height = get2();
*/
    else if (tag == 0xfa18)  // 64024
    {
      imgdata.makernotes.kodak.offset_left = getint(8);
      if (type != 8)
        imgdata.makernotes.kodak.offset_left += 1;
    }
    else if (tag == 0xfa19)  // 64025
    {
      imgdata.makernotes.kodak.offset_top = getint(8);
      if (type != 8)
        imgdata.makernotes.kodak.offset_top += 1;
    }

    else if (tag == 0xfa25)  // 64037
      Kodak_KDC_WBtags(LIBRAW_WBI_Auto, wbi);
    else if (tag == 0xfa27)  // 64039
      Kodak_KDC_WBtags(LIBRAW_WBI_Tungsten, wbi);
    else if (tag == 0xfa28)  // 64040
      Kodak_KDC_WBtags(LIBRAW_WBI_Fluorescent, wbi);
    else if (tag == 0xfa29)  // 64041
      Kodak_KDC_WBtags(LIBRAW_WBI_Daylight, wbi);
    else if (tag == 0xfa2a)  // 64042
      Kodak_KDC_WBtags(LIBRAW_WBI_Shade, wbi);

    else if (tag == 0xfa31)  // 64049
      imgdata.sizes.raw_crop.cwidth = get2();
    else if (tag == 0xfa32)  // 64050
      imgdata.sizes.raw_crop.cheight = get2();
    else if (tag == 0xfa3e)  // 64062
      imgdata.sizes.raw_crop.cleft = get2();
    else if (tag == 0xfa3f)  // 64063
      imgdata.sizes.raw_crop.ctop = get2();

    else if (((tag == 0x07e4) || (tag == 0xfb01)) && (len == 9))  // 2020 or 64257
    {
      if (KodakIllumMatrix (type, (float *)imgdata.makernotes.kodak.romm_camDaylight))
      {
        romm_coeff(imgdata.makernotes.kodak.romm_camDaylight);
      }
    }
    else if (((tag == 0x07e5) || (tag == 0xfb02)) && (len == 9))  // 2021 or 64258
      KodakIllumMatrix (type, (float *)imgdata.makernotes.kodak.romm_camTungsten);
    else if (((tag == 0x07e6) || (tag == 0xfb03)) && (len == 9))  // 2022 or 64259
      KodakIllumMatrix (type, (float *)imgdata.makernotes.kodak.romm_camFluorescent);
    else if (((tag == 0x07e7) || (tag == 0xfb04)) && (len == 9))  // 2023 or 64260
      KodakIllumMatrix (type, (float *)imgdata.makernotes.kodak.romm_camFlash);
    else if (((tag == 0x07e8) || (tag == 0xfb05)) && (len == 9))  // 2024 or 64261
      KodakIllumMatrix (type, (float *)imgdata.makernotes.kodak.romm_camCustom);
    else if (((tag == 0x07e9) || (tag == 0xfb06)) && (len == 9))  // 2025 or 64262
      KodakIllumMatrix (type, (float *)imgdata.makernotes.kodak.romm_camAuto);

    fseek(ifp, save, SEEK_SET);
  }
}