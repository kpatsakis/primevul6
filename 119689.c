void CLASS wavelet_denoise()
{
  float *fimg = 0, *temp, thold, mul[2], avg, diff;
  int scale = 1, size, lev, hpass, lpass, row, col, nc, c, i, wlast, blk[2];
  ushort *window[4];
  static const float noise[] = {0.8002, 0.2735, 0.1202, 0.0585, 0.0291, 0.0152, 0.0080, 0.0044};

#ifdef DCRAW_VERBOSE
  if (verbose)
    fprintf(stderr, _("Wavelet denoising...\n"));
#endif

  while (maximum << scale < 0x10000)
    scale++;
  maximum <<= --scale;
  black <<= scale;
  FORC4 cblack[c] <<= scale;
  if ((size = iheight * iwidth) < 0x15550000)
    fimg = (float *)malloc((size * 3 + iheight + iwidth) * sizeof *fimg);
  merror(fimg, "wavelet_denoise()");
  temp = fimg + size * 3;
  if ((nc = colors) == 3 && filters)
    nc++;
#ifdef LIBRAW_LIBRARY_BUILD
#pragma omp parallel default(shared) private(i, col, row, thold, lev, lpass, hpass, temp, c) firstprivate(scale, size)
#endif
  {
    temp = (float *)malloc((iheight + iwidth) * sizeof *fimg);
    FORC(nc)
    { /* denoise R,G1,B,G3 individually */
#ifdef LIBRAW_LIBRARY_BUILD
#pragma omp for
#endif
      for (i = 0; i < size; i++)
        fimg[i] = 256 * sqrt((double)(image[i][c] << scale));
      for (hpass = lev = 0; lev < 5; lev++)
      {
        lpass = size * ((lev & 1) + 1);
#ifdef LIBRAW_LIBRARY_BUILD
#pragma omp for
#endif
        for (row = 0; row < iheight; row++)
        {
          hat_transform(temp, fimg + hpass + row * iwidth, 1, iwidth, 1 << lev);
          for (col = 0; col < iwidth; col++)
            fimg[lpass + row * iwidth + col] = temp[col] * 0.25;
        }
#ifdef LIBRAW_LIBRARY_BUILD
#pragma omp for
#endif
        for (col = 0; col < iwidth; col++)
        {
          hat_transform(temp, fimg + lpass + col, iwidth, iheight, 1 << lev);
          for (row = 0; row < iheight; row++)
            fimg[lpass + row * iwidth + col] = temp[row] * 0.25;
        }
        thold = threshold * noise[lev];
#ifdef LIBRAW_LIBRARY_BUILD
#pragma omp for
#endif
        for (i = 0; i < size; i++)
        {
          fimg[hpass + i] -= fimg[lpass + i];
          if (fimg[hpass + i] < -thold)
            fimg[hpass + i] += thold;
          else if (fimg[hpass + i] > thold)
            fimg[hpass + i] -= thold;
          else
            fimg[hpass + i] = 0;
          if (hpass)
            fimg[i] += fimg[hpass + i];
        }
        hpass = lpass;
      }
#ifdef LIBRAW_LIBRARY_BUILD
#pragma omp for
#endif
      for (i = 0; i < size; i++)
        image[i][c] = CLIP(SQR(fimg[i] + fimg[lpass + i]) / 0x10000);
    }
    free(temp);
  } /* end omp parallel */
  /* the following loops are hard to parallize, no idea yes,
   * problem is wlast which is carrying dependency
   * second part should be easyer, but did not yet get it right.
   */
  if (filters && colors == 3)
  { /* pull G1 and G3 closer together */
    for (row = 0; row < 2; row++)
    {
      mul[row] = 0.125 * pre_mul[FC(row + 1, 0) | 1] / pre_mul[FC(row, 0) | 1];
      blk[row] = cblack[FC(row, 0) | 1];
    }
    for (i = 0; i < 4; i++)
      window[i] = (ushort *)fimg + width * i;
    for (wlast = -1, row = 1; row < height - 1; row++)
    {
      while (wlast < row + 1)
      {
        for (wlast++, i = 0; i < 4; i++)
          window[(i + 3) & 3] = window[i];
        for (col = FC(wlast, 1) & 1; col < width; col += 2)
          window[2][col] = BAYER(wlast, col);
      }
      thold = threshold / 512;
      for (col = (FC(row, 0) & 1) + 1; col < width - 1; col += 2)
      {
        avg = (window[0][col - 1] + window[0][col + 1] + window[2][col - 1] + window[2][col + 1] - blk[~row & 1] * 4) *
                  mul[row & 1] +
              (window[1][col] + blk[row & 1]) * 0.5;
        avg = avg < 0 ? 0 : sqrt(avg);
        diff = sqrt((double)BAYER(row, col)) - avg;
        if (diff < -thold)
          diff += thold;
        else if (diff > thold)
          diff -= thold;
        else
          diff = 0;
        BAYER(row, col) = CLIP(SQR(avg + diff) + 0.5);
      }
    }
  }
  free(fimg);
}