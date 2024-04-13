MagickExport ErrorHandler SetErrorHandler(ErrorHandler handler)
{
  ErrorHandler
    previous_handler;

  previous_handler=error_handler;
  error_handler=handler;
  return(previous_handler);
}