void CLASS rollei_load_raw()
{
  uchar pixel[10];
  unsigned iten = 0, isix, i, buffer = 0, todo[16];

  isix = raw_width * raw_height * 5 / 8;
  while (fread(pixel, 1, 10, ifp) == 10)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    checkCancel();
#endif
    for (i = 0; i < 10; i += 2)
    {
      todo[i] = iten++;
      todo[i + 1] = pixel[i] << 8 | pixel[i + 1];
      buffer = pixel[i] >> 2 | buffer << 6;
    }
    for (; i < 16; i += 2)
    {
      todo[i] = isix++;
      todo[i + 1] = buffer >> (14 - i) * 5;
    }
    for (i = 0; i < 16; i += 2)
      raw_image[todo[i]] = (todo[i + 1] & 0x3ff);
  }
  maximum = 0x3ff;
}