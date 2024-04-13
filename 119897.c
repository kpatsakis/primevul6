MagickExport char *GetExceptionMessage(const int error)
{
  char
    exception[MagickPathExtent];

  *exception='\0';
#if defined(MAGICKCORE_HAVE_STRERROR_R)
#if !defined(MAGICKCORE_STRERROR_R_CHAR_P)
  (void) strerror_r(error,exception,sizeof(exception));
#else
  (void) CopyMagickString(exception,strerror_r(error,exception,
    sizeof(exception)),sizeof(exception));
#endif
#else
  (void) CopyMagickString(exception,strerror(error),sizeof(exception));
#endif
  return(ConstantString(exception));
}