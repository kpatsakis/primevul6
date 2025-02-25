void CLASS kodak_radc_load_raw()
{
  static const signed char src[] = {
      1, 1,   2, 3,   3, 4,   4, 2,   5, 7,   6, 5,   7, 6,  7, 8,   1, 0,   2, 1,  3, 3,  4, 4,  5, 2,   6, 7,   7, 6,
      8, 5,   8, 8,   2, 1,   2, 3,   3, 0,   3, 2,   3, 4,  4, 6,   5, 5,   6, 7,  6, 8,  2, 0,  2, 1,   2, 3,   3, 2,
      4, 4,   5, 6,   6, 7,   7, 5,   7, 8,   2, 1,   2, 4,  3, 0,   3, 2,   3, 3,  4, 7,  5, 5,  6, 6,   6, 8,   2, 3,
      3, 1,   3, 2,   3, 4,   3, 5,   3, 6,   4, 7,   5, 0,  5, 8,   2, 3,   2, 6,  3, 0,  3, 1,  4, 4,   4, 5,   4, 7,
      5, 2,   5, 8,   2, 4,   2, 7,   3, 3,   3, 6,   4, 1,  4, 2,   4, 5,   5, 0,  5, 8,  2, 6,  3, 1,   3, 3,   3, 5,
      3, 7,   3, 8,   4, 0,   5, 2,   5, 4,   2, 0,   2, 1,  3, 2,   3, 3,   4, 4,  4, 5,  5, 6,  5, 7,   4, 8,   1, 0,
      2, 2,   2, -2,  1, -3,  1, 3,   2, -17, 2, -5,  2, 5,  2, 17,  2, -7,  2, 2,  2, 9,  2, 18, 2, -18, 2, -9,  2, -2,
      2, 7,   2, -28, 2, 28,  3, -49, 3, -9,  3, 9,   4, 49, 5, -79, 5, 79,  2, -1, 2, 13, 2, 26, 3, 39,  4, -16, 5, 55,
      6, -37, 6, 76,  2, -26, 2, -13, 2, 1,   3, -39, 4, 16, 5, -55, 6, -76, 6, 37};
  ushort huff[19][256];
  int row, col, tree, nreps, rep, step, i, c, s, r, x, y, val;
  short last[3] = {16, 16, 16}, mul[3], buf[3][3][386];
  static const ushort pt[] = {0, 0, 1280, 1344, 2320, 3616, 3328, 8000, 4095, 16383, 65535, 16383};

  for (i = 2; i < 12; i += 2)
    for (c = pt[i - 2]; c <= pt[i]; c++)
      curve[c] = (float)(c - pt[i - 2]) / (pt[i] - pt[i - 2]) * (pt[i + 1] - pt[i - 1]) + pt[i - 1] + 0.5;
  for (s = i = 0; i < sizeof src; i += 2)
    FORC(256 >> src[i])
  ((ushort *)huff)[s++] = src[i] << 8 | (uchar)src[i + 1];
  s = kodak_cbpp == 243 ? 2 : 3;
  FORC(256) huff[18][c] = (8 - s) << 8 | c >> s << s | 1 << (s - 1);
  getbits(-1);
  for (i = 0; i < sizeof(buf) / sizeof(short); i++)
    ((short *)buf)[i] = 2048;
  for (row = 0; row < height; row += 4)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    FORC3 mul[c] = getbits(6);
#ifdef LIBRAW_LIBRARY_BUILD
    if(!mul[0] || !mul[1] || !mul[2])
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
#endif
    FORC3
    {
      val = ((0x1000000 / last[c] + 0x7ff) >> 12) * mul[c];
      s = val > 65564 ? 10 : 12;
      x = ~((~0u) << (s - 1));
      val <<= 12 - s;
      for (i = 0; i < sizeof(buf[0]) / sizeof(short); i++)
        ((short *)buf[c])[i] = (((short *)buf[c])[i] * val + x) >> s;
      last[c] = mul[c];
      for (r = 0; r <= !c; r++)
      {
        buf[c][1][width / 2] = buf[c][2][width / 2] = mul[c] << 7;
        for (tree = 1, col = width / 2; col > 0;)
        {
          if ((tree = radc_token(tree)))
          {
            col -= 2;
            if (tree == 8)
              FORYX buf[c][y][x] = (uchar)radc_token(18) * mul[c];
            else
              FORYX buf[c][y][x] = radc_token(tree + 10) * 16 + PREDICTOR;
          }
          else
            do
            {
              nreps = (col > 2) ? radc_token(9) + 1 : 1;
              for (rep = 0; rep < 8 && rep < nreps && col > 0; rep++)
              {
                col -= 2;
                FORYX buf[c][y][x] = PREDICTOR;
                if (rep & 1)
                {
                  step = radc_token(10) << 4;
                  FORYX buf[c][y][x] += step;
                }
              }
            } while (nreps == 9);
        }
        for (y = 0; y < 2; y++)
          for (x = 0; x < width / 2; x++)
          {
            val = (buf[c][y + 1][x] << 4) / mul[c];
            if (val < 0)
              val = 0;
            if (c)
              RAW(row + y * 2 + c - 1, x * 2 + 2 - c) = val;
            else
              RAW(row + r * 2 + y, x * 2 + y) = val;
          }
        memcpy(buf[c][0] + !c, buf[c][2], sizeof buf[c][0] - 2 * !c);
      }
    }
    for (y = row; y < row + 4; y++)
      for (x = 0; x < width; x++)
        if ((x + y) & 1)
        {
          r = x ? x - 1 : x + 1;
          s = x + 1 < width ? x + 1 : x - 1;
          val = (RAW(y, x) - 2048) * 2 + (RAW(y, r) + RAW(y, s)) / 2;
          if (val < 0)
            val = 0;
          RAW(y, x) = val;
        }
  }
  for (i = 0; i < height * width; i++)
    raw_image[i] = curve[raw_image[i]];
  maximum = 0x3fff;
}