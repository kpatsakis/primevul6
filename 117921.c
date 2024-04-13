static char **SamplingFactorToList(const char *text)
{
  char
    **textlist;

  register char
    *q;

  register const char
    *p;

  register ssize_t
    i;

  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Convert string to an ASCII list.
  */
  textlist=(char **) AcquireQuantumMemory((size_t) MAX_COMPONENTS,
    sizeof(*textlist));
  if (textlist == (char **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
  p=text;
  for (i=0; i < (ssize_t) MAX_COMPONENTS; i++)
  {
    for (q=(char *) p; *q != '\0'; q++)
      if (*q == ',')
        break;
    textlist[i]=(char *) AcquireQuantumMemory((size_t) (q-p)+MagickPathExtent,
      sizeof(*textlist[i]));
    if (textlist[i] == (char *) NULL)
      ThrowFatalException(ResourceLimitFatalError,"UnableToConvertText");
    (void) CopyMagickString(textlist[i],p,(size_t) (q-p+1));
    if (*q == '\r')
      q++;
    if (*q == '\0')
      break;
    p=q+1;
  }
  for (i++; i < (ssize_t) MAX_COMPONENTS; i++)
    textlist[i]=ConstantString("1x1");
  return(textlist);
}