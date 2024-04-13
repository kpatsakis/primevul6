static inline double GetDrawValue(const char *magick_restrict string,
  char **magick_restrict sentinal)
{
  char
    **magick_restrict q;

  double
    value;

  q=sentinal;
  value=InterpretLocaleValue(string,q);
  sentinal=q;
  return(value);
}