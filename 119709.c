void CLASS process_Sony_0x9402(uchar *buf) {

  if (buf[2] != 0xff) return;
  short bufx = SonySubstitution[buf[0]];
  if ((bufx < 0x0f) ||
      (bufx > 0x1a) ||
      (bufx == 0x16) ||
      (bufx == 0x18)) return;

  imgdata.other.AmbientTemperature = (float) ((short) SonySubstitution[buf[4]]);

  return;
}