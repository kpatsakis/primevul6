void CLASS process_Sony_0x940e(uchar *buf, ushort len, unsigned id)
{
  if (((imgdata.makernotes.sony.SonyCameraType != LIBRAW_SONY_SLT) &&
       (imgdata.makernotes.sony.SonyCameraType != LIBRAW_SONY_ILCA)) ||
      (id == 280) ||
      (id == 281) ||
      (id == 285))
    return;

  if (imgdata.makernotes.sony.SonyCameraType == LIBRAW_SONY_ILCA)
  {
    if (len >= 0x06)
    {
      imgdata.shootinginfo.FocusMode = SonySubstitution[buf[0x05]];
    }
    if (len >= 0x0051)
    {
      imgdata.makernotes.sony.AFMicroAdjValue = SonySubstitution[buf[0x0050]];
    }
  }
  else
  {
    if (len >= 0x0c)
    {
      imgdata.shootinginfo.FocusMode = SonySubstitution[buf[0x0b]];
    }
    if (len >= 0x017e)
    {
      imgdata.makernotes.sony.AFMicroAdjValue = SonySubstitution[buf[0x017d]];
    }
  }

  if (imgdata.makernotes.sony.AFMicroAdjValue != 0)
    imgdata.makernotes.sony.AFMicroAdjOn = 1;
}