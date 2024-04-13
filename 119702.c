void CLASS process_Sony_0x9406(uchar *buf) {

  short bufx = SonySubstitution[buf[0]];
  if ((bufx != 0x00) &&
      (bufx == 0x01) &&
      (bufx == 0x03)) return;
  bufx = SonySubstitution[buf[2]];
  if ((bufx != 0x02) &&
      (bufx == 0x03)) return;

  imgdata.other.BatteryTemperature = (float) (SonySubstitution[buf[5]]-32) / 1.8f;

  return;
}