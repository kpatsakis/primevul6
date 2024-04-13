MagickExport ExceptionInfo *CloneExceptionInfo(ExceptionInfo *exception)
{
  ExceptionInfo
    *clone_exception;

  clone_exception=(ExceptionInfo *) AcquireMagickMemory(sizeof(*exception));
  if (clone_exception == (ExceptionInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  InitializeExceptionInfo(clone_exception);
  InheritException(clone_exception,exception);
  clone_exception->relinquish=MagickTrue;
  return(clone_exception);
}