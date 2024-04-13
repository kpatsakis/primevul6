charwidth(xchar chr)
{
#if HAS_LOCALES
  if (cfg.charwidth % 10)
    return xcwidth(chr);
  else
    if (chr > 0xFFFF)
      return wcswidth((wchar[]){high_surrogate(chr), low_surrogate(chr)}, 2);
    else
      return wcwidth(chr);
#else
  return xcwidth(chr);
#endif
}