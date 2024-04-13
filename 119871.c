MagickExport MagickBooleanType ThrowMagickException(ExceptionInfo *exception,
  const char *module,const char *function,const size_t line,
  const ExceptionType severity,const char *tag,const char *format,...)
{
  MagickBooleanType
    status;

  va_list
    operands;

  va_start(operands,format);
  status=ThrowMagickExceptionList(exception,module,function,line,severity,tag,
    format,operands);
  va_end(operands);
  return(status);
}