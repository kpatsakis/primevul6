MagickExport ExceptionInfo *DestroyExceptionInfo(ExceptionInfo *exception)
{
  MagickBooleanType
    relinquish;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (exception->semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&exception->semaphore);
  LockSemaphoreInfo(exception->semaphore);
  exception->severity=UndefinedException;
  if (exception->relinquish != MagickFalse)
    {
      exception->signature=(~MagickCoreSignature);
      if (exception->exceptions != (void *) NULL)
        exception->exceptions=(void *) DestroyLinkedList((LinkedListInfo *)
          exception->exceptions,DestroyExceptionElement);
    }
  else if (exception->exceptions != (void *) NULL)
    ClearLinkedList((LinkedListInfo *) exception->exceptions,
      DestroyExceptionElement);
  relinquish=exception->relinquish;
  UnlockSemaphoreInfo(exception->semaphore);
  if (relinquish != MagickFalse)
    {
      RelinquishSemaphoreInfo(&exception->semaphore);
      exception=(ExceptionInfo *) RelinquishMagickMemory(exception);
    }
  return(exception);
}