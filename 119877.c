MagickExport void MagickFatalError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (fatal_error_handler != (ErrorHandler) NULL)
    (*fatal_error_handler)(error,reason,description);
}