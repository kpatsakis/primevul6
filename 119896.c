MagickExport FatalErrorHandler SetFatalErrorHandler(FatalErrorHandler handler)
{
  FatalErrorHandler
    previous_handler;

  previous_handler=fatal_error_handler;
  fatal_error_handler=handler;
  return(previous_handler);
}