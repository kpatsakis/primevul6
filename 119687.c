void CLASS bad_pixels(const char *cfname)
{
  FILE *fp = NULL;
#ifndef LIBRAW_LIBRARY_BUILD
  char *fname, *cp, line[128];
  int len, time, row, col, r, c, rad, tot, n, fixed = 0;
#else
  char *cp, line[128];
  int time, row, col, r, c, rad, tot, n;
#ifdef DCRAW_VERBOSE
  int fixed = 0;
#endif
#endif

  if (!filters)
    return;
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_BAD_PIXELS, 0, 2);
#endif
  if (cfname)
    fp = fopen(cfname, "r");
  // @end FILEIO
  else
  {
    for (len = 32;; len *= 2)
    {
      fname = (char *)malloc(len);
      if (!fname)
        return;
      if (getcwd(fname, len - 16))
        break;
      free(fname);
      if (errno != ERANGE)
        return;
    }
#if defined(WIN32) || defined(DJGPP)
    if (fname[1] == ':')
      memmove(fname, fname + 2, len - 2);
    for (cp = fname; *cp; cp++)
      if (*cp == '\\')
        *cp = '/';
#endif
    cp = fname + strlen(fname);
    if (cp[-1] == '/')
      cp--;
    while (*fname == '/')
    {
      strcpy(cp, "/.badpixels");
      if ((fp = fopen(fname, "r")))
        break;
      if (cp == fname)
        break;
      while (*--cp != '/')
        ;
    }
    free(fname);
  }
  // @out FILEIO
  if (!fp)
  {
#ifdef LIBRAW_LIBRARY_BUILD
    imgdata.process_warnings |= LIBRAW_WARN_NO_BADPIXELMAP;
#endif
    return;
  }
  while (fgets(line, 128, fp))
  {
    cp = strchr(line, '#');
    if (cp)
      *cp = 0;
    if (sscanf(line, "%d %d %d", &col, &row, &time) != 3)
      continue;
    if ((unsigned)col >= width || (unsigned)row >= height)
      continue;
    if (time > timestamp)
      continue;
    for (tot = n = 0, rad = 1; rad < 3 && n == 0; rad++)
      for (r = row - rad; r <= row + rad; r++)
        for (c = col - rad; c <= col + rad; c++)
          if ((unsigned)r < height && (unsigned)c < width && (r != row || c != col) && fcol(r, c) == fcol(row, col))
          {
            tot += BAYER2(r, c);
            n++;
          }
    BAYER2(row, col) = tot / n;
#ifdef DCRAW_VERBOSE
    if (verbose)
    {
      if (!fixed++)
        fprintf(stderr, _("Fixed dead pixels at:"));
      fprintf(stderr, " %d,%d", col, row);
    }
#endif
  }
#ifdef DCRAW_VERBOSE
  if (fixed)
    fputc('\n', stderr);
#endif
  fclose(fp);
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_BAD_PIXELS, 1, 2);
#endif
}