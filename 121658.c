void CLASS parseCanonMakernotes(unsigned tag, unsigned type, unsigned len)
{

  if (tag == 0x0001)
    Canon_CameraSettings();
  else if (tag == 0x0002) // focal length
  {
    imgdata.lens.makernotes.FocalType = get2();
    imgdata.lens.makernotes.CurFocal = get2();
    if (imgdata.lens.makernotes.CanonFocalUnits > 1)
    {
      imgdata.lens.makernotes.CurFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
    }
  }

  else if (tag == 0x0004) // shot info
  {
    short tempAp;

    fseek(ifp, 24, SEEK_CUR);
    tempAp = get2();
    if (tempAp != 0)
      imgdata.other.CameraTemperature = (float)(tempAp - 128);
    tempAp = get2();
    if (tempAp != -1)
      imgdata.other.FlashGN = ((float)tempAp) / 32;
    get2();

    // fseek(ifp, 30, SEEK_CUR);
    imgdata.other.FlashEC = _CanonConvertEV((signed short)get2());
    fseek(ifp, 8 - 32, SEEK_CUR);
    if ((tempAp = get2()) != 0x7fff)
      imgdata.lens.makernotes.CurAp = _CanonConvertAperture(tempAp);
    if (imgdata.lens.makernotes.CurAp < 0.7f)
    {
      fseek(ifp, 32, SEEK_CUR);
      imgdata.lens.makernotes.CurAp = _CanonConvertAperture(get2());
    }
    if (!aperture)
      aperture = imgdata.lens.makernotes.CurAp;
  }

  else if (tag == 0x000c)
  {
    unsigned tS = get4();
    sprintf (imgdata.shootinginfo.BodySerial, "%d", tS);
  }

  else if (tag == 0x0095 && // lens model tag
           !imgdata.lens.makernotes.Lens[0])
  {
    fread(imgdata.lens.makernotes.Lens, 2, 1, ifp);
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
    if (imgdata.lens.makernotes.Lens[0] < 65) // non-Canon lens
      fread(imgdata.lens.makernotes.Lens + 2, 62, 1, ifp);
    else
    {
      char efs[2];
      imgdata.lens.makernotes.LensFeatures_pre[0] = imgdata.lens.makernotes.Lens[0];
      imgdata.lens.makernotes.LensFeatures_pre[1] = imgdata.lens.makernotes.Lens[1];
      fread(efs, 2, 1, ifp);
      if (efs[0] == 45 && (efs[1] == 83 || efs[1] == 69 || efs[1] == 77))
      { // "EF-S, TS-E, MP-E, EF-M" lenses
        imgdata.lens.makernotes.Lens[2] = imgdata.lens.makernotes.LensFeatures_pre[2] = efs[0];
        imgdata.lens.makernotes.Lens[3] = imgdata.lens.makernotes.LensFeatures_pre[3] = efs[1];
        imgdata.lens.makernotes.Lens[4] = 32;
        if (efs[1] == 83)
        {
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF_S;
          imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
        }
        else if (efs[1] == 77)
        {
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF_M;
        }
      }
      else
      { // "EF" lenses
        imgdata.lens.makernotes.Lens[2] = 32;
        imgdata.lens.makernotes.Lens[3] = efs[0];
        imgdata.lens.makernotes.Lens[4] = efs[1];
      }
      fread(imgdata.lens.makernotes.Lens + 5, 58, 1, ifp);
    }
  }

  else if (tag == 0x009a)
  {
    get4();
    imgdata.sizes.raw_crop.cwidth = get4();
    imgdata.sizes.raw_crop.cheight = get4();
    imgdata.sizes.raw_crop.cleft = get4();
    imgdata.sizes.raw_crop.ctop = get4();
  }

  else if (tag == 0x00a9)
  {
    long int save1 = ftell(ifp);
    int c;
    fseek(ifp, (0x1 << 1), SEEK_CUR);
    FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
    Canon_WBpresets(0, 0);
    fseek(ifp, save1, SEEK_SET);
  }

  else if (tag == 0x00e0) // sensor info
  {
    imgdata.makernotes.canon.SensorWidth = (get2(), get2());
    imgdata.makernotes.canon.SensorHeight = get2();
    imgdata.makernotes.canon.SensorLeftBorder = (get2(), get2(), get2());
    imgdata.makernotes.canon.SensorTopBorder = get2();
    imgdata.makernotes.canon.SensorRightBorder = get2();
    imgdata.makernotes.canon.SensorBottomBorder = get2();
    imgdata.makernotes.canon.BlackMaskLeftBorder = get2();
    imgdata.makernotes.canon.BlackMaskTopBorder = get2();
    imgdata.makernotes.canon.BlackMaskRightBorder = get2();
    imgdata.makernotes.canon.BlackMaskBottomBorder = get2();
  }

  else if (tag == 0x4013)
  {
    get4();
    imgdata.makernotes.canon.AFMicroAdjMode = get4();
    imgdata.makernotes.canon.AFMicroAdjValue = ((float)get4()) / ((float)get4());
  }

  else if (tag == 0x4001 && len > 500) {
    int c;
    int bls = 0;
    long int offsetChannelBlackLevel = 0L;
    long int offsetWhiteLevels = 0L;
    long int save1 = ftell(ifp);

    switch (len) {

    case 582:
      imgdata.makernotes.canon.CanonColorDataVer = 1; // 20D / 350D

      fseek(ifp, save1 + (0x001e << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0041 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0046 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom2][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0023 << 1), SEEK_SET);
      Canon_WBpresets(2, 2);
      fseek(ifp, save1 + (0x004b << 1), SEEK_SET);
      Canon_WBCTpresets(1); // ABCT
      offsetChannelBlackLevel = save1 + (0x00a6 << 1);
      break;

    case 653:
      imgdata.makernotes.canon.CanonColorDataVer = 2; // 1Dmk2 / 1DsMK2

      fseek(ifp, save1 + (0x0018 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0090 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0095 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom2][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x009a << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom3][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0027 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00a4 << 1), SEEK_SET);
      Canon_WBCTpresets(1); // ABCT
      offsetChannelBlackLevel = save1 + (0x011e << 1);
      break;

    case 796:
      imgdata.makernotes.canon.CanonColorDataVer = 3; // 1DmkIIN / 5D / 30D / 400D
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0071 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0076 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom2][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x007b << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom3][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0080 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x004e << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x0085 << 1), SEEK_SET);
      Canon_WBCTpresets(0);                       // BCAT
      offsetChannelBlackLevel = save1 + (0x00c4 << 1);
      break;

    // 1DmkIII / 1DSmkIII / 1DmkIV / 5DmkII
    // 7D / 40D / 50D / 60D / 450D / 500D
    // 550D / 1000D / 1100D
    case 674:
    case 692:
    case 702:
    case 1227:
    case 1250:
    case 1251:
    case 1337:
    case 1338:
    case 1346:
      imgdata.makernotes.canon.CanonColorDataVer = 4;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0053 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00a8 << 1), SEEK_SET);
      Canon_WBCTpresets(0);                       // BCAT

      if ((imgdata.makernotes.canon.CanonColorDataSubVer == 4) ||
          (imgdata.makernotes.canon.CanonColorDataSubVer == 5))
      {
        offsetChannelBlackLevel = save1 + (0x02b4 << 1);
        offsetWhiteLevels = save1 + (0x02b8 << 1);
      }
      else if ((imgdata.makernotes.canon.CanonColorDataSubVer == 6) ||
               (imgdata.makernotes.canon.CanonColorDataSubVer == 7))
      {
        offsetChannelBlackLevel = save1 + (0x02cb << 1);
        offsetWhiteLevels = save1 + (0x02cf << 1);
      }
      else if (imgdata.makernotes.canon.CanonColorDataSubVer == 9)
      {
        offsetChannelBlackLevel = save1 + (0x02cf << 1);
        offsetWhiteLevels = save1 + (0x02d3 << 1);
      }
      else offsetChannelBlackLevel = save1 + (0x00e7 << 1);
      break;

    case 5120:
      imgdata.makernotes.canon.CanonColorDataVer = 5; // PowerSot G10, G12, G5 X, G7 X, G9 X, EOS M3, EOS M5, EOS M6
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      if (imgdata.makernotes.canon.CanonColorDataSubVer == 0xfffc) // -4
      { // G7 X Mark II, G9 X Mark II, G1 X Mark III, M5, M100, M6
        fseek(ifp, save1 + (0x004f << 1), SEEK_SET);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
        fseek(ifp, 8, SEEK_CUR);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();
        fseek(ifp, 8, SEEK_CUR);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Other][c ^ (c >> 1)] = get2();
        fseek(ifp, 8, SEEK_CUR);
        Canon_WBpresets(8, 24);
        fseek(ifp, 168, SEEK_CUR);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_WW][c ^ (c >> 1)] = get2();
        fseek(ifp, 24, SEEK_CUR);
        Canon_WBCTpresets(2); // BCADT
        offsetChannelBlackLevel = save1 + (0x014d << 1);
      }
      else if (imgdata.makernotes.canon.CanonColorDataSubVer == 0xfffd) // -3
      { // M10/M3/G1 X/G1 X II/G10/G11/G12/G15/G16/G3 X/G5 X/G7 X/G9 X/S100/S110/S120/S90/S95/SX1 IX/SX50 HS/SX60 HS
        fseek(ifp, save1 + (0x004c << 1), SEEK_SET);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
        get2();
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();
        get2();
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Other][c ^ (c >> 1)] = get2();
        get2();
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0x00ba << 1), SEEK_SET);
        Canon_WBCTpresets(2);                       // BCADT
        offsetChannelBlackLevel = save1 + (0x0108 << 1);
      }
      break;

    case 1273:
    case 1275:
      imgdata.makernotes.canon.CanonColorDataVer = 6; // 600D / 1200D
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0067 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00bc << 1), SEEK_SET);
      Canon_WBCTpresets(0);                       // BCAT
      offsetChannelBlackLevel = save1 + (0x01df << 1);
      offsetWhiteLevels = save1 + (0x01e3 << 1);
      break;

    // 1DX / 5DmkIII / 6D / 100D / 650D / 700D / EOS M / 7DmkII / 750D / 760D
    case 1312:
    case 1313:
    case 1316:
    case 1506:
      imgdata.makernotes.canon.CanonColorDataVer = 7;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0080 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00d5 << 1), SEEK_SET);
      Canon_WBCTpresets(0);                       // BCAT

      if (imgdata.makernotes.canon.CanonColorDataSubVer == 10)
      {
        offsetChannelBlackLevel = save1 + (0x01f8 << 1);
        offsetWhiteLevels = save1 + (0x01fc << 1);
      }
      else if (imgdata.makernotes.canon.CanonColorDataSubVer == 11)
      {
        offsetChannelBlackLevel = save1 + (0x02d8 << 1);
        offsetWhiteLevels = save1 + (0x02dc << 1);
      }
      break;

    // 5DS / 5DS R / 80D / 1300D / 2000D / 4000D / 5D4 / 800D / 77D / 6D II / 200D
    case 1560:
    case 1592:
    case 1353:
    case 1602:
      imgdata.makernotes.canon.CanonColorDataVer = 8;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0085 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x0107 << 1), SEEK_SET);
      Canon_WBCTpresets(0);                       // BCAT

      if (imgdata.makernotes.canon.CanonColorDataSubVer == 14) // 1300D / 2000D / 4000D
      {
        offsetChannelBlackLevel = save1 + (0x022c << 1);
        offsetWhiteLevels = save1 + (0x0230 << 1);
      }
      else
      {
        offsetChannelBlackLevel = save1 + (0x030a << 1);
        offsetWhiteLevels = save1 + (0x030e << 1);
      }
      break;

    case 1820:	// M50
      imgdata.makernotes.canon.CanonColorDataVer = 9;
      imgdata.makernotes.canon.CanonColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0047 << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      get2();
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      get2();
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0088 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x010a << 1), SEEK_SET);
      Canon_WBCTpresets(0);
      offsetChannelBlackLevel = save1 + (0x0318 << 1);
      offsetWhiteLevels = save1 + (0x031c << 1);
      break;
    }

    if (offsetChannelBlackLevel) {
      fseek(ifp, offsetChannelBlackLevel, SEEK_SET);
      FORC4
        bls += (imgdata.makernotes.canon.ChannelBlackLevel[c ^ (c >> 1)] = get2());
      imgdata.makernotes.canon.AverageBlackLevel = bls / 4;
    }
    if (offsetWhiteLevels) {
      if ((offsetWhiteLevels - offsetChannelBlackLevel) != 8L)
        fseek(ifp, offsetWhiteLevels, SEEK_SET);
      imgdata.makernotes.canon.NormalWhiteLevel = get2();
      imgdata.makernotes.canon.SpecularWhiteLevel = get2();
      FORC4
        imgdata.color.linear_max[c] = imgdata.makernotes.canon.SpecularWhiteLevel;
    }
    fseek(ifp, save1, SEEK_SET);
  }
}