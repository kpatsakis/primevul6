void CLASS process_Sony_0x0116(uchar *buf, unsigned id) {

  if ((id == 257) ||
      (id == 262) ||
      (id == 269) ||
      (id == 270))
    imgdata.other.BatteryTemperature = (float) (buf[1]-32) / 1.8f;
  else if ((id != 263) &&
           (id != 264) &&
           (id != 265) &&
           (id != 266))
    imgdata.other.BatteryTemperature = (float) (buf[2]-32) / 1.8f;
  return;
}