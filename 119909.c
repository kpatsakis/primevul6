MagickExport MagickBooleanType ThrowMagickExceptionList(
  ExceptionInfo *exception,const char *module,const char *function,
  const size_t line,const ExceptionType severity,const char *tag,
  const char *format,va_list operands)
{
  char
    message[MagickPathExtent],
    path[MagickPathExtent],
    reason[MagickPathExtent];

  const char
    *locale,
    *type;

  int
    n;

  MagickBooleanType
    status;

  size_t
    length;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  locale=GetLocaleExceptionMessage(severity,tag);
  (void) CopyMagickString(reason,locale,MagickPathExtent);
  (void) ConcatenateMagickString(reason," ",MagickPathExtent);
  length=strlen(reason);
#if defined(MAGICKCORE_HAVE_VSNPRINTF)
  n=vsnprintf(reason+length,MagickPathExtent-length,format,operands);
#else
  n=vsprintf(reason+length,format,operands);
#endif
  if (n < 0)
    reason[MagickPathExtent-1]='\0';
  status=LogMagickEvent(ExceptionEvent,module,function,line,"%s",reason);
  GetPathComponent(module,TailPath,path);
  type="undefined";
  if ((severity >= WarningException) && (severity < ErrorException))
    type="warning";
  if ((severity >= ErrorException) && (severity < FatalErrorException))
    type="error";
  if (severity >= FatalErrorException)
    type="fatal";
  (void) FormatLocaleString(message,MagickPathExtent,"%s @ %s/%s/%s/%.20g",reason,
    type,path,function,(double) line);
  (void) ThrowException(exception,severity,message,(char *) NULL);
  return(status);
}