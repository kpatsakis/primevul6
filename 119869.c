MagickExport void InheritException(ExceptionInfo *exception,
  const ExceptionInfo *relative)
{
  register const ExceptionInfo
    *p;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  assert(relative != (ExceptionInfo *) NULL);
  assert(relative->signature == MagickCoreSignature);
  assert(exception != relative);
  if (relative->exceptions == (void *) NULL)
    return;
  LockSemaphoreInfo(relative->semaphore);
  ResetLinkedListIterator((LinkedListInfo *) relative->exceptions);
  p=(const ExceptionInfo *) GetNextValueInLinkedList((LinkedListInfo *)
    relative->exceptions);
  while (p != (const ExceptionInfo *) NULL)
  {
    (void) ThrowException(exception,p->severity,p->reason,p->description);
    p=(const ExceptionInfo *) GetNextValueInLinkedList((LinkedListInfo *)
      relative->exceptions);
  }
  UnlockSemaphoreInfo(relative->semaphore);
}