MagickPrivate void InitializeExceptionInfo(ExceptionInfo *exception)
{
  assert(exception != (ExceptionInfo *) NULL);
  (void) ResetMagickMemory(exception,0,sizeof(*exception));
  exception->severity=UndefinedException;
  exception->exceptions=(void *) NewLinkedList(0);
  exception->semaphore=AcquireSemaphoreInfo();
  exception->signature=MagickCoreSignature;
}