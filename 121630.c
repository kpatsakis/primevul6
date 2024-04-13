void CLASS parseOlympus_ImageProcessing (unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
// uptag 0x2040

  int i, c, wb[4], nWB, tWB, wbG;
  ushort CT;
  short sorder;

    if ((tag == 0x0100) && (dng_writer == nonDNG)) {
      cam_mul[0] = get2() / 256.0;
      cam_mul[2] = get2() / 256.0;

  } else if ((tag == 0x0101) &&
             (len == 2)      &&
             (!strncasecmp(model, "E-410", 5) ||
              !strncasecmp(model, "E-510", 5))) {
      for (c = 0; i < 64; i++) {
        imgdata.color.WBCT_Coeffs[i][2] =
          imgdata.color.WBCT_Coeffs[i][4] =
          imgdata.color.WB_Coeffs[i][1] =
          imgdata.color.WB_Coeffs[i][3] = 0x100;
      }
      for (c = 64; i < 256; i++) {
        imgdata.color.WB_Coeffs[i][1] =
          imgdata.color.WB_Coeffs[i][3] = 0x100;
      }

  } else if ((tag > 0x0101) &&
             (tag <= 0x0111)) {
      nWB = tag - 0x0101;
      tWB = Oly_wb_list2[nWB << 1];
      CT = Oly_wb_list2[(nWB << 1) | 1];
      wb[0] = get2();
      wb[2] = get2();
      if (tWB != 0x100) {
        imgdata.color.WB_Coeffs[tWB][0] = wb[0];
        imgdata.color.WB_Coeffs[tWB][2] = wb[2];
      }
      if (CT) {
        imgdata.color.WBCT_Coeffs[nWB - 1][0] = CT;
        imgdata.color.WBCT_Coeffs[nWB - 1][1] = wb[0];
        imgdata.color.WBCT_Coeffs[nWB - 1][3] = wb[2];
      }
      if (len == 4) {
        wb[1] = get2();
        wb[3] = get2();
        if (tWB != 0x100) {
          imgdata.color.WB_Coeffs[tWB][1] = wb[1];
          imgdata.color.WB_Coeffs[tWB][3] = wb[3];
        }
        if (CT) {
          imgdata.color.WBCT_Coeffs[nWB - 1][2] = wb[1];
          imgdata.color.WBCT_Coeffs[nWB - 1][4] = wb[3];
        }
      }

  } else if ((tag >= 0x0112) &&
             (tag <= 0x011e)) {
      nWB = tag - 0x0112;
      wbG = get2();
      tWB = Oly_wb_list2[nWB << 1];
      if (nWB)
        imgdata.color.WBCT_Coeffs[nWB - 1][2] =
          imgdata.color.WBCT_Coeffs[nWB - 1][4] = wbG;
      if (tWB != 0x100)
        imgdata.color.WB_Coeffs[tWB][1] =
          imgdata.color.WB_Coeffs[tWB][3] = wbG;

  } else if (tag == 0x011f) {
      wbG = get2();
      if (imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][0])
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][1] =
          imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][3] = wbG;
      FORC4 if (imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + c][0])
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + c][1] =
          imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + c][3] = wbG;

  } else if (tag == 0x0121) {
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][0] = get2();
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][2] = get2();
      if (len == 4) {
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][1] = get2();
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][3] = get2();
      }

  } else if ((tag == 0x0200)        &&
             (dng_writer == nonDNG) &&
             strcmp(software, "v757-71")) {
      for (i = 0; i < 3; i++) {
        if (!imgdata.makernotes.olympus.ColorSpace) {
          FORC3 cmatrix[i][c] = ((short)get2()) / 256.0;
        } else {
          FORC3 imgdata.color.ccm[i][c] = ((short)get2()) / 256.0;
        }
      }

  } else if ((tag == 0x0600) && (dng_writer == nonDNG)) {
      FORC4 cblack[c ^ c >> 1] = get2();
  } else if ((tag == 0x0612) && (dng_writer == nonDNG)) {
        imgdata.sizes.raw_crop.cleft = get2();
  } else if ((tag == 0x0613)  && (dng_writer == nonDNG)){
        imgdata.sizes.raw_crop.ctop = get2();
  } else if ((tag == 0x0614)  && (dng_writer == nonDNG)){
        imgdata.sizes.raw_crop.cwidth = get2();
  } else if ((tag == 0x0615)  && (dng_writer == nonDNG)){
        imgdata.sizes.raw_crop.cheight = get2();

  } else if ((tag == 0x0805) && (len == 2)) {
      imgdata.makernotes.olympus.OlympusSensorCalibration[0] = getreal(type);
      imgdata.makernotes.olympus.OlympusSensorCalibration[1] = getreal(type);
      if (dng_writer == nonDNG)
        FORC4 imgdata.color.linear_max[c] = imgdata.makernotes.olympus.OlympusSensorCalibration[0];

  } else if (tag == 0x1112) {
        sorder = order;
        order = 0x4d4d;
        imgdata.makernotes.olympus.OlympusCropID = get2();
        order = sorder;
  } else if (tag == 0x1113) {
        FORC4 imgdata.makernotes.olympus.OlympusFrame[c] = get2();

  } else if (tag == 0x1306) {
       c = get2();
       if ((c != 0) && (c != 100)) {
          if (c < 61)
            imgdata.other.CameraTemperature = (float)c;
          else
            imgdata.other.CameraTemperature = (float)(c - 32) / 1.8f;
          if ((OlyID == 0x4434353933ULL) && // TG-5
              (imgdata.other.exifAmbientTemperature > -273.15f))
            imgdata.other.CameraTemperature += imgdata.other.exifAmbientTemperature;
       }
  }

  return;
}