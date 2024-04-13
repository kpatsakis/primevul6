MagickExport void ClearMagickException(ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (exception->exceptions == (void *) NULL)
    return;
  LockSemaphoreInfo(exception->semaphore);
  ClearLinkedList((LinkedListInfo *) exception->exceptions,
    DestroyExceptionElement);
  exception->severity=UndefinedException;
  exception->reason=(char *) NULL;
  exception->description=(char *) NULL;
  UnlockSemaphoreInfo(exception->semaphore);
  errno=0;
}