MagickExport MagickBooleanType CloneImageProfiles(Image *image,
  const Image *clone_image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(clone_image != (const Image *) NULL);
  assert(clone_image->signature == MagickCoreSignature);
  if (clone_image->profiles != (void *) NULL)
    {
      if (image->profiles != (void *) NULL)
        DestroyImageProfiles(image);
      image->profiles=CloneSplayTree((SplayTreeInfo *) clone_image->profiles,
        (void *(*)(void *)) ConstantString,(void *(*)(void *)) CloneStringInfo);
    }
  return(MagickTrue);
}