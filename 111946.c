ModuleExport MagickBooleanType WritePSDLayers(Image * image,
  const ImageInfo *image_info,const PSDInfo *psd_info,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=IsRightsAuthorized(CoderPolicyDomain,WritePolicyRights,"PSD");
  if (status == MagickFalse)
    return(MagickTrue);
  return WritePSDLayersInternal(image,image_info,psd_info,(size_t*) NULL,
    exception);
}