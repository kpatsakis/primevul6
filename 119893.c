MagickExport MagickBooleanType ThrowException(ExceptionInfo *exception,
  const ExceptionType severity,const char *reason,const char *description)
{
  register ExceptionInfo
    *p;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  LockSemaphoreInfo(exception->semaphore);
  p=(ExceptionInfo *) GetLastValueInLinkedList((LinkedListInfo *)
    exception->exceptions);
  if ((p != (ExceptionInfo *) NULL) && (p->severity == severity) &&
      (LocaleCompare(exception->reason,reason) == 0) &&
      (LocaleCompare(exception->description,description) == 0))
    {
      UnlockSemaphoreInfo(exception->semaphore);
      return(MagickTrue);
    }
  p=(ExceptionInfo *) AcquireMagickMemory(sizeof(*p));
  if (p == (ExceptionInfo *) NULL)
    {
      UnlockSemaphoreInfo(exception->semaphore);
      ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
    }
  (void) ResetMagickMemory(p,0,sizeof(*p));
  p->severity=severity;
  if (reason != (const char *) NULL)
    p->reason=ConstantString(reason);
  if (description != (const char *) NULL)
    p->description=ConstantString(description);
  p->signature=MagickCoreSignature;
  (void) AppendValueToLinkedList((LinkedListInfo *) exception->exceptions,p);
  if (p->severity >= exception->severity)
    {
      exception->severity=p->severity;
      exception->reason=p->reason;
      exception->description=p->description;
    }
  UnlockSemaphoreInfo(exception->semaphore);
  return(MagickTrue);
}