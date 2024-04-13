void CLASS parseSonyLensFeatures(uchar a, uchar b)
{

  ushort features;
  features = (((ushort)a) << 8) | ((ushort)b);

  if ((imgdata.lens.makernotes.LensMount == LIBRAW_MOUNT_Canon_EF) ||
      (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Sigma_X3F) || !features)
    return;

  imgdata.lens.makernotes.LensFeatures_pre[0] = 0;
  imgdata.lens.makernotes.LensFeatures_suf[0] = 0;
  if ((features & 0x0200) && (features & 0x0100))
    strcpy(imgdata.lens.makernotes.LensFeatures_pre, "E");
  else if (features & 0x0200)
    strcpy(imgdata.lens.makernotes.LensFeatures_pre, "FE");
  else if (features & 0x0100)
    strcpy(imgdata.lens.makernotes.LensFeatures_pre, "DT");

  if (!imgdata.lens.makernotes.LensFormat && !imgdata.lens.makernotes.LensMount)
  {
    imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_FF;
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Minolta_A;

    if ((features & 0x0200) && (features & 0x0100))
    {
      imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E;
    }
    else if (features & 0x0200)
    {
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E;
    }
    else if (features & 0x0100)
    {
      imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
    }
  }

  if (features & 0x4000)
    strnXcat(imgdata.lens.makernotes.LensFeatures_pre, " PZ");

  if (features & 0x0008)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " G");
  else if (features & 0x0004)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " ZA");

  if ((features & 0x0020) && (features & 0x0040))
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " Macro");
  else if (features & 0x0020)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " STF");
  else if (features & 0x0040)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " Reflex");
  else if (features & 0x0080)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " Fisheye");

  if (features & 0x0001)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " SSM");
  else if (features & 0x0002)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " SAM");

  if (features & 0x8000)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " OSS");

  if (features & 0x2000)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " LE");

  if (features & 0x0800)
    strnXcat(imgdata.lens.makernotes.LensFeatures_suf, " II");

  if (imgdata.lens.makernotes.LensFeatures_suf[0] == ' ')
    memmove(imgdata.lens.makernotes.LensFeatures_suf, imgdata.lens.makernotes.LensFeatures_suf + 1,
            strbuflen(imgdata.lens.makernotes.LensFeatures_suf) - 1);

  return;
}