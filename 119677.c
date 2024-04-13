void CLASS parseFujiMakernotes(unsigned tag, unsigned type)
{
  switch (tag)
  {
  case 0x1002:
    imgdata.makernotes.fuji.WB_Preset = get2();
    break;
  case 0x1011:
    imgdata.other.FlashEC = getreal(type);
    break;
  case 0x1020:
    imgdata.makernotes.fuji.Macro = get2();
    break;
  case 0x1021:
    imgdata.makernotes.fuji.FocusMode = get2();
    break;
  case 0x1022:
    imgdata.makernotes.fuji.AFMode = get2();
    break;
  case 0x1023:
    imgdata.makernotes.fuji.FocusPixel[0] = get2();
    imgdata.makernotes.fuji.FocusPixel[1] = get2();
    break;
  case 0x1034:
    imgdata.makernotes.fuji.ExrMode = get2();
    break;
  case 0x1050:
    imgdata.makernotes.fuji.ShutterType = get2();
    break;
  case 0x1400:
    imgdata.makernotes.fuji.FujiDynamicRange = get2();
    break;
  case 0x1401:
    imgdata.makernotes.fuji.FujiFilmMode = get2();
    break;
  case 0x1402:
    imgdata.makernotes.fuji.FujiDynamicRangeSetting = get2();
    break;
  case 0x1403:
    imgdata.makernotes.fuji.FujiDevelopmentDynamicRange = get2();
    break;
  case 0x140b:
    imgdata.makernotes.fuji.FujiAutoDynamicRange = get2();
    break;
  case 0x1404:
    imgdata.lens.makernotes.MinFocal = getreal(type);
    break;
  case 0x1405:
    imgdata.lens.makernotes.MaxFocal = getreal(type);
    break;
  case 0x1406:
    imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type);
    break;
  case 0x1407:
    imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type);
    break;
  case 0x1422:
    imgdata.makernotes.fuji.ImageStabilization[0] = get2();
    imgdata.makernotes.fuji.ImageStabilization[1] = get2();
    imgdata.makernotes.fuji.ImageStabilization[2] = get2();
    imgdata.shootinginfo.ImageStabilization =
        (imgdata.makernotes.fuji.ImageStabilization[0] << 9) + imgdata.makernotes.fuji.ImageStabilization[1];
    break;
  case 0x1431:
    imgdata.makernotes.fuji.Rating = get4();
    break;
  case 0x3820:
    imgdata.makernotes.fuji.FrameRate = get2();
    break;
  case 0x3821:
    imgdata.makernotes.fuji.FrameWidth = get2();
    break;
  case 0x3822:
    imgdata.makernotes.fuji.FrameHeight = get2();
    break;
  }
  return;
}