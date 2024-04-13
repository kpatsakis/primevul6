void CLASS remove_zeroes()
{
  unsigned row, col, tot, n, r, c;

#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_REMOVE_ZEROES, 0, 2);
#endif

  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++)
      if (BAYER(row, col) == 0)
      {
        tot = n = 0;
        for (r = row - 2; r <= row + 2; r++)
          for (c = col - 2; c <= col + 2; c++)
            if (r < height && c < width && FC(r, c) == FC(row, col) && BAYER(r, c))
              tot += (n++, BAYER(r, c));
        if (n)
          BAYER(row, col) = tot / n;
      }
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_REMOVE_ZEROES, 1, 2);
#endif
}