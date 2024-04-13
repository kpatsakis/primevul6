MagickExport ExceptionInfo *AcquireExceptionInfo(void)
{
  ExceptionInfo
    *exception;

  exception=(ExceptionInfo *) AcquireMagickMemory(sizeof(*exception));
  if (exception == (ExceptionInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  InitializeExceptionInfo(exception);
  exception->relinquish=MagickTrue;
  return(exception);
}