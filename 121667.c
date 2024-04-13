void CLASS panasonic_load_raw()
{
  int row, col, i, j, sh = 0, pred[2], nonz[2];
  unsigned bytes[16];
  ushort *raw_block_data;
  int enc_blck_size = pana_bpp == 12 ? 10 : 9;

  pana_data(0, 0);
  if (pana_encoding == 5)
  {
    for (row = 0; row < raw_height; row++)
    {
      raw_block_data = raw_image + row * raw_width;

#ifdef LIBRAW_LIBRARY_BUILD
      checkCancel();
#endif
      for (col = 0; col < raw_width; col += enc_blck_size)
      {
        pana_data(0, bytes);

        if (pana_bpp == 12)
        {
          raw_block_data[col] = ((bytes[1] & 0xF) << 8) + bytes[0];
          raw_block_data[col + 1] = 16 * bytes[2] + (bytes[1] >> 4);
          raw_block_data[col + 2] = ((bytes[4] & 0xF) << 8) + bytes[3];
          raw_block_data[col + 3] = 16 * bytes[5] + (bytes[4] >> 4);
          raw_block_data[col + 4] = ((bytes[7] & 0xF) << 8) + bytes[6];
          raw_block_data[col + 5] = 16 * bytes[8] + (bytes[7] >> 4);
          raw_block_data[col + 6] = ((bytes[10] & 0xF) << 8) + bytes[9];
          raw_block_data[col + 7] = 16 * bytes[11] + (bytes[10] >> 4);
          raw_block_data[col + 8] = ((bytes[13] & 0xF) << 8) + bytes[12];
          raw_block_data[col + 9] = 16 * bytes[14] + (bytes[13] >> 4);
        }
        else if (pana_bpp == 14)
        {
          raw_block_data[col] = bytes[0] + ((bytes[1] & 0x3F) << 8);
          raw_block_data[col + 1] = (bytes[1] >> 6) + 4 * (bytes[2]) +
                                    ((bytes[3] & 0xF) << 10);
          raw_block_data[col + 2] = (bytes[3] >> 4) + 16 * (bytes[4]) +
                                    ((bytes[5] & 3) << 12);
          raw_block_data[col + 3] = ((bytes[5] & 0xFC) >> 2) + (bytes[6] << 6);
          raw_block_data[col + 4] = bytes[7] + ((bytes[8] & 0x3F) << 8);
          raw_block_data[col + 5] = (bytes[8] >> 6) + 4 * bytes[9] + ((bytes[10] & 0xF) << 10);
          raw_block_data[col + 6] = (bytes[10] >> 4) + 16 * bytes[11] + ((bytes[12] & 3) << 12);
          raw_block_data[col + 7] = ((bytes[12] & 0xFC) >> 2) + (bytes[13] << 6);
          raw_block_data[col + 8] = bytes[14] + ((bytes[15] & 0x3F) << 8);
        }
      }
    }
  }
  else
  {
    for (row = 0; row < raw_height; row++)
    {
#ifdef LIBRAW_LIBRARY_BUILD
      checkCancel();
#endif
      for (col = 0; col < raw_width; col++)
      {
        if ((i = col % 14) == 0)
          pred[0] = pred[1] = nonz[0] = nonz[1] = 0;
        if (i % 3 == 2)
          sh = 4 >> (3 - pana_data(2, 0));
        if (nonz[i & 1])
        {
          if ((j = pana_data(8, 0)))
          {
            if ((pred[i & 1] -= 0x80 << sh) < 0 || sh == 4)
              pred[i & 1] &= ~((~0u) << sh);
            pred[i & 1] += j << sh;
          }
        }
        else if ((nonz[i & 1] = pana_data(8, 0)) || i > 11)
          pred[i & 1] = nonz[i & 1] << 4 | pana_data(4, 0);
        if ((RAW(row, col) = pred[col & 1]) > 4098 && col < width && row < height)
          derror();
      }
    }
  }
}