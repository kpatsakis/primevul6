MagickExport void MagickWarning(const ExceptionType warning,const char *reason,
  const char *description)
{
  if (warning_handler != (WarningHandler) NULL)
    (*warning_handler)(warning,reason,description);
}