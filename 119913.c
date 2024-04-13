static void DefaultFatalErrorHandler(const ExceptionType severity,
  const char *reason,const char *description)
{
  if (reason == (char *) NULL)
    return;
  (void) FormatLocaleFile(stderr,"%s: %s",GetClientName(),reason);
  if (description != (char *) NULL)
    (void) FormatLocaleFile(stderr," (%s)",description);
  (void) FormatLocaleFile(stderr,".\n");
  (void) fflush(stderr);
  MagickCoreTerminus();
  exit((int) (severity-FatalErrorException)+1);
}