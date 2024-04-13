MagickExport const char *GetLocaleExceptionMessage(const ExceptionType severity,
  const char *tag)
{
  char
    message[MagickPathExtent];

  const char
    *locale_message;

  assert(tag != (const char *) NULL);
  (void) FormatLocaleString(message,MagickPathExtent,"Exception/%s%s",
    ExceptionSeverityToTag(severity),tag);
  locale_message=GetLocaleMessage(message);
  if (locale_message == (const char *) NULL)
    return(tag);
  if (locale_message == message)
    return(tag);
  return(locale_message);
}