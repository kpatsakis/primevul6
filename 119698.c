void CLASS parse_kodak_ifd(int base)
{
  unsigned entries, tag, type, len, save;
  int i, c, wbi = -2, wbtemp = 6500;
  float mul[3] = {1, 1, 1}, num;
  static const int wbtag[] = {64037, 64040, 64039, 64041, -1, -1, 64042};

  entries = get2();
  if (entries > 1024)
    return;
  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
    if (tag == 1020)
      wbi = getint(type);
    if (tag == 1021 && len == 72)
    { /* WB set in software */
      fseek(ifp, 40, SEEK_CUR);
      FORC3 cam_mul[c] = 2048.0 / fMAX(1.0, get2());
      wbi = -2;
    }
    if (tag == 2118)
      wbtemp = getint(type);
    if (tag == 2120 + wbi && wbi >= 0)
      FORC3 cam_mul[c] = 2048.0 / fMAX(1.0, getreal(type));
    if (tag == 2130 + wbi)
      FORC3 mul[c] = getreal(type);
    if (tag == 2140 + wbi && wbi >= 0)
      FORC3
      {
        for (num = i = 0; i < 4; i++)
          num += getreal(type) * pow(wbtemp / 100.0, i);
        cam_mul[c] = 2048 / fMAX(1.0, (num * mul[c]));
      }
    if (tag == 2317)
      linear_table(len);
    if (tag == 6020)
      iso_speed = getint(type);
    if (tag == 64013)
      wbi = fgetc(ifp);
    if ((unsigned)wbi < 7 && tag == wbtag[wbi])
      FORC3 cam_mul[c] = get4();
    if (tag == 64019)
      width = getint(type);
    if (tag == 64020)
      height = (getint(type) + 1) & -2;
    fseek(ifp, save, SEEK_SET);
  }
}