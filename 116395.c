scriptfont(ucschar ch)
{
  if (!*cfg.font_choice)
    return 0;
  if (!scriptfonts_init)
    init_scriptfonts();

  int i, j, k;

  if (use_blockfonts) {
    i = -1;
    j = lengthof(blockfonts);
    while (j - i > 1) {
      k = (i + j) / 2;
      if (ch < blockfonts[k].first)
        j = k;
      else if (ch > blockfonts[k].last)
        i = k;
      else {
        uchar f = blockfonts[k].font;
        if (f)
          return f;
        break;
      }
    }
  }

  i = -1;
  j = lengthof(scriptfonts);
  while (j - i > 1) {
    k = (i + j) / 2;
    if (ch < scriptfonts[k].first)
      j = k;
    else if (ch > scriptfonts[k].last)
      i = k;
    else
      return scriptfonts[k].font;
  }
  return 0;
}