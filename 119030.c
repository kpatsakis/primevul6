static MagickBooleanType JPEGWarningHandler(j_common_ptr jpeg_info,int level)
{
#define JPEGExcessiveWarnings  1000

  char
    message[JMSG_LENGTH_MAX];

  ErrorManager
    *error_manager;

  ExceptionInfo
    *exception;

  Image
    *image;

  *message='\0';
  error_manager=(ErrorManager *) jpeg_info->client_data;
  exception=error_manager->exception;
  image=error_manager->image;
  if (level < 0)
    {
      /*
        Process warning message.
      */
      (jpeg_info->err->format_message)(jpeg_info,message);
      if (jpeg_info->err->num_warnings++ > JPEGExcessiveWarnings)
        JPEGErrorHandler(jpeg_info);
      ThrowBinaryException(CorruptImageWarning,(char *) message,
        image->filename);
    }
  else
    if ((image->debug != MagickFalse) &&
        (level >= jpeg_info->err->trace_level))
      {
        /*
          Process trace message.
        */
        (jpeg_info->err->format_message)(jpeg_info,message);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "[%s] JPEG Trace: \"%s\"",image->filename,message);
      }
  return(MagickTrue);
}