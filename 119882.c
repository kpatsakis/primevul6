MagickExport void CatchException(ExceptionInfo *exception)
{
  register const ExceptionInfo
    *p;

  ssize_t
    i;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (exception->exceptions  == (void *) NULL)
    return;
  LockSemaphoreInfo(exception->semaphore);
  ResetLinkedListIterator((LinkedListInfo *) exception->exceptions);
  p=(const ExceptionInfo *) GetNextValueInLinkedList((LinkedListInfo *)
    exception->exceptions);
  for (i=0; p != (const ExceptionInfo *) NULL; i++)
  {
    if (i < MaxExceptions)
      {
        if ((p->severity >= WarningException) && (p->severity < ErrorException))
          MagickWarning(p->severity,p->reason,p->description);
        if ((p->severity >= ErrorException) &&
            (p->severity < FatalErrorException))
          MagickError(p->severity,p->reason,p->description);
      }
    else
      if (i == MaxExceptions)
        MagickError(ResourceLimitError,"too many exceptions",
          "exception processing is suspended");
    if (p->severity >= FatalErrorException)
      MagickFatalError(p->severity,p->reason,p->description);
    p=(const ExceptionInfo *) GetNextValueInLinkedList((LinkedListInfo *)
      exception->exceptions);
  }
  UnlockSemaphoreInfo(exception->semaphore);
  ClearMagickException(exception);
}