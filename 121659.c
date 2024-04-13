void CLASS process_Sony_0x2010(uchar *buf, ushort len)
{
  if (!imgdata.makernotes.sony.group2010)
    return;

  if ((imgdata.makernotes.sony.real_iso_offset != 0xffff)    &&
      (len >= (imgdata.makernotes.sony.real_iso_offset + 2)) &&
      (imgdata.other.real_ISO < 0.1f)) {
    uchar s[2];
    s[0] = SonySubstitution[buf[imgdata.makernotes.sony.real_iso_offset]];
    s[1] = SonySubstitution[buf[imgdata.makernotes.sony.real_iso_offset + 1]];
    imgdata.other.real_ISO =
       100.0f * libraw_powf64l(2.0f, (16 - ((float)sget2(s)) / 256.0f));
  }

  if (len >= (imgdata.makernotes.sony.MeteringMode_offset + 2)) {
    imgdata.shootinginfo.MeteringMode =
       SonySubstitution[buf[imgdata.makernotes.sony.MeteringMode_offset]];
    imgdata.shootinginfo.ExposureProgram =
      SonySubstitution[buf[imgdata.makernotes.sony.ExposureProgram_offset]];
  }

  if (len >= (imgdata.makernotes.sony.ReleaseMode2_offset + 2)) {
    imgdata.shootinginfo.DriveMode =
       SonySubstitution[buf[imgdata.makernotes.sony.ReleaseMode2_offset]];
  }

}