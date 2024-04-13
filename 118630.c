static void CMSExceptionHandler(cmsContext context,cmsUInt32Number severity,
  const char *message)
{
  CMSExceptionInfo
    *cms_exception;

  ExceptionInfo
    *exception;

  Image
    *image;

  cms_exception=(CMSExceptionInfo *) context;
  if (cms_exception == (CMSExceptionInfo *) NULL)
    return;
  exception=cms_exception->exception;
  if (exception == (ExceptionInfo *) NULL)
    return;
  image=cms_exception->image;
  if (image == (Image *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageWarning,
        "UnableToTransformColorspace","`%s'","unknown context");
      return;
    }
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TransformEvent,GetMagickModule(),"lcms: #%u, %s",
      severity,message != (char *) NULL ? message : "no message");
  (void) ThrowMagickException(exception,GetMagickModule(),ImageWarning,
    "UnableToTransformColorspace","`%s'",image->filename);
}