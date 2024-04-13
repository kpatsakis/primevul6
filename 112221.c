MagickExport StringInfo *RemoveImageProfile(Image *image,const char *name)
{
  StringInfo
    *profile;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return((StringInfo *) NULL);
  WriteTo8BimProfile(image,name,(StringInfo *) NULL);
  profile=(StringInfo *) RemoveNodeFromSplayTree((SplayTreeInfo *)
    image->profiles,name);
  return(profile);
}