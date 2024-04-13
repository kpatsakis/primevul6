MagickExport WarningHandler SetWarningHandler(WarningHandler handler)
{
  WarningHandler
    previous_handler;

  previous_handler=warning_handler;
  warning_handler=handler;
  return(previous_handler);
}