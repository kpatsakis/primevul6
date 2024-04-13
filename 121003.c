static void Adam7_deinterlace(unsigned char* out, const unsigned char* in, unsigned w, unsigned h, unsigned bpp)
{
  unsigned passw[7], passh[7];
  size_t filter_passstart[8], padded_passstart[8], passstart[8];
  unsigned i;

  Adam7_getpassvalues(passw, passh, filter_passstart, padded_passstart, passstart, w, h, bpp);

  if(bpp >= 8)
  {
    for(i = 0; i < 7; i++)
    {
      unsigned x, y, b;
      size_t bytewidth = bpp / 8;
      for(y = 0; y < passh[i]; y++)
      for(x = 0; x < passw[i]; x++)
      {
        size_t pixelinstart = passstart[i] + (y * passw[i] + x) * bytewidth;
        size_t pixeloutstart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) * w + ADAM7_IX[i] + x * ADAM7_DX[i]) * bytewidth;
        for(b = 0; b < bytewidth; b++)
        {
          out[pixeloutstart + b] = in[pixelinstart + b];
        }
      }
    }
  }
  else /*bpp < 8: Adam7 with pixels < 8 bit is a bit trickier: with bit pointers*/
  {
    for(i = 0; i < 7; i++)
    {
      unsigned x, y, b;
      unsigned ilinebits = bpp * passw[i];
      unsigned olinebits = bpp * w;
      size_t obp, ibp; /*bit pointers (for out and in buffer)*/
      for(y = 0; y < passh[i]; y++)
      for(x = 0; x < passw[i]; x++)
      {
        ibp = (8 * passstart[i]) + (y * ilinebits + x * bpp);
        obp = (ADAM7_IY[i] + y * ADAM7_DY[i]) * olinebits + (ADAM7_IX[i] + x * ADAM7_DX[i]) * bpp;
        for(b = 0; b < bpp; b++)
        {
          unsigned char bit = readBitFromReversedStream(&ibp, in);
          /*note that this function assumes the out buffer is completely 0, use setBitOfReversedStream otherwise*/
          setBitOfReversedStream0(&obp, out, bit);
        }
      }
    }
  }
}