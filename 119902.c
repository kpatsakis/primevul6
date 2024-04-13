MagickExport void MagickError(const ExceptionType error,const char *reason,
  const char *description)
{
  if (error_handler != (ErrorHandler) NULL)
    (*error_handler)(error,reason,description);
}