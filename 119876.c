static void DefaultErrorHandler(const ExceptionType magick_unused(severity),
  const char *reason,const char *description)
{
  magick_unreferenced(severity);

  if (reason == (char *) NULL)
    return;
  (void) FormatLocaleFile(stderr,"%s: %s",GetClientName(),reason);
  if (description != (char *) NULL)
    (void) FormatLocaleFile(stderr," (%s)",description);
  (void) FormatLocaleFile(stderr,".\n");
  (void) fflush(stderr);
}