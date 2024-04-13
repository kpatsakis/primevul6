static void *DestroyExceptionElement(void *exception)
{
  register ExceptionInfo
    *p;

  p=(ExceptionInfo *) exception;
  if (p->reason != (char *) NULL)
    p->reason=DestroyString(p->reason);
  if (p->description != (char *) NULL)
    p->description=DestroyString(p->description);
  p=(ExceptionInfo *) RelinquishMagickMemory(p);
  return((void *) NULL);
}