void CLASS process_Sony_0x9403(uchar *buf) {

  short bufx = SonySubstitution[buf[4]];
  if ((bufx == 0x00) ||
      (bufx == 0x94)) return;

  imgdata.other.SensorTemperature = (float) ((short) SonySubstitution[buf[5]]);

  return;
}