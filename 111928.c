ModuleExport MagickBooleanType ReadPSDLayers(Image *image,
  const ImageInfo *image_info,const PSDInfo *psd_info,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  status=IsRightsAuthorized(CoderPolicyDomain,ReadPolicyRights,"PSD");
  if (status == MagickFalse)
    return(MagickTrue);
  return(ReadPSDLayersInternal(image,image_info,psd_info,MagickFalse,
    exception));
}