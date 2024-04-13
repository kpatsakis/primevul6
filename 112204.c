MagickExport MagickBooleanType SetImageProfile(Image *image,const char *name,
  const StringInfo *profile,ExceptionInfo *exception)
{
  return(SetImageProfileInternal(image,name,profile,MagickFalse,exception));
}