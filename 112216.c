MagickExport MagickBooleanType DeleteImageProfile(Image *image,const char *name)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return(MagickFalse);
  WriteTo8BimProfile(image,name,(StringInfo *) NULL);
  return(DeleteNodeFromSplayTree((SplayTreeInfo *) image->profiles,name));
}