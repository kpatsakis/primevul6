static MagickBooleanType WritePSDImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  const StringInfo
    *icc_profile;

  MagickBooleanType
    status;

  PSDInfo
    psd_info;

  ssize_t
    i;

  size_t
    length,
    num_channels;

  StringInfo
    *bim_profile;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  psd_info.version=1;
  if ((LocaleCompare(image_info->magick,"PSB") == 0) ||
      (image->columns > 30000) || (image->rows > 30000))
    psd_info.version=2;
  (void) WriteBlob(image,4,(const unsigned char *) "8BPS");
  (void) WriteBlobMSBShort(image,psd_info.version);  /* version */
  for (i=1; i <= 6; i++)
    (void) WriteBlobByte(image, 0);  /* 6 bytes of reserved */
  if ((GetImageProfile(image,"icc") == (StringInfo *) NULL) &&
      (SetImageGray(image,exception) != MagickFalse))
    num_channels=(image->alpha_trait != UndefinedPixelTrait ? 2UL : 1UL);
  else
    if ((image_info->type != TrueColorType) &&
        (image_info->type != TrueColorAlphaType) &&
        (image->storage_class == PseudoClass))
      num_channels=(image->alpha_trait != UndefinedPixelTrait ? 2UL : 1UL);
    else
      {
        if (image->storage_class == PseudoClass)
          (void) SetImageStorageClass(image,DirectClass,exception);
        if (image->colorspace != CMYKColorspace)
          num_channels=(image->alpha_trait != UndefinedPixelTrait ? 4UL : 3UL);
        else
          num_channels=(image->alpha_trait != UndefinedPixelTrait ? 5UL : 4UL);
      }
  (void) WriteBlobMSBShort(image,(unsigned short) num_channels);
  (void) WriteBlobMSBLong(image,(unsigned int) image->rows);
  (void) WriteBlobMSBLong(image,(unsigned int) image->columns);
  if (IsImageGray(image) != MagickFalse)
    {
      MagickBooleanType
        monochrome;

      /*
        Write depth & mode.
      */
      monochrome=IsImageMonochrome(image) && (image->depth == 1) ?
        MagickTrue : MagickFalse;
      (void) WriteBlobMSBShort(image,(unsigned short)
        (monochrome != MagickFalse ? 1 : image->depth > 8 ? 16 : 8));
      (void) WriteBlobMSBShort(image,(unsigned short)
        (monochrome != MagickFalse ? BitmapMode : GrayscaleMode));
    }
  else
    {
      (void) WriteBlobMSBShort(image,(unsigned short) (image->storage_class ==
        PseudoClass ? 8 : image->depth > 8 ? 16 : 8));

      if (((image_info->colorspace != UndefinedColorspace) ||
           (image->colorspace != CMYKColorspace)) &&
          (image_info->colorspace != CMYKColorspace))
        {
          (void) TransformImageColorspace(image,sRGBColorspace,exception);
          (void) WriteBlobMSBShort(image,(unsigned short)
            (image->storage_class == PseudoClass ? IndexedMode : RGBMode));
        }
      else
        {
          if (image->colorspace != CMYKColorspace)
            (void) TransformImageColorspace(image,CMYKColorspace,exception);
          (void) WriteBlobMSBShort(image,CMYKMode);
        }
    }
  if ((IsImageGray(image) != MagickFalse) ||
      (image->storage_class == DirectClass) || (image->colors > 256))
    (void) WriteBlobMSBLong(image,0);
  else
    {
      /*
        Write PSD raster colormap.
      */
      (void) WriteBlobMSBLong(image,768);
      for (i=0; i < (ssize_t) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(ClampToQuantum(
          image->colormap[i].red)));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (ssize_t) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(ClampToQuantum(
          image->colormap[i].green)));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (ssize_t) image->colors; i++)
        (void) WriteBlobByte(image,ScaleQuantumToChar(ClampToQuantum(
          image->colormap[i].blue)));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
    }
  /*
    Image resource block.
  */
  length=28; /* 0x03EB */
  bim_profile=(StringInfo *) GetImageProfile(image,"8bim");
  icc_profile=GetImageProfile(image,"icc");
  if (bim_profile != (StringInfo *) NULL)
    {
      bim_profile=CloneStringInfo(bim_profile);
      if (icc_profile != (StringInfo *) NULL)
        RemoveICCProfileFromResourceBlock(bim_profile);
      RemoveResolutionFromResourceBlock(bim_profile);
      length+=PSDQuantum(GetStringInfoLength(bim_profile));
    }
  if (icc_profile != (const StringInfo *) NULL)
    length+=PSDQuantum(GetStringInfoLength(icc_profile))+12;
  (void) WriteBlobMSBLong(image,(unsigned int) length);
  WriteResolutionResourceBlock(image);
  if (bim_profile != (StringInfo *) NULL)
    {
      (void) WriteBlob(image,GetStringInfoLength(bim_profile),
        GetStringInfoDatum(bim_profile));
      bim_profile=DestroyStringInfo(bim_profile);
    }
  if (icc_profile != (StringInfo *) NULL)
    {
      (void) WriteBlob(image,4,(const unsigned char *) "8BIM");
      (void) WriteBlobMSBShort(image,0x0000040F);
      (void) WriteBlobMSBShort(image,0);
      (void) WriteBlobMSBLong(image,(unsigned int) GetStringInfoLength(
        icc_profile));
      (void) WriteBlob(image,GetStringInfoLength(icc_profile),
        GetStringInfoDatum(icc_profile));
      if ((ssize_t) GetStringInfoLength(icc_profile) != PSDQuantum(GetStringInfoLength(icc_profile)))
        (void) WriteBlobByte(image,0);
    }
  if (status != MagickFalse)
    {
      const char
        *option;

      CompressionType
        compression;

      MagickOffsetType
        size_offset;

      size_t
        size;

      size_offset=TellBlob(image);
      (void) SetPSDSize(&psd_info,image,0);
      option=GetImageOption(image_info,"psd:write-layers");
      if (IsStringFalse(option) != MagickTrue)
        {
          status=WritePSDLayersInternal(image,image_info,&psd_info,&size,
            exception);
          (void) WritePSDSize(&psd_info,image,size+
            (psd_info.version == 1 ? 8 : 12),size_offset);
          (void) WriteBlobMSBLong(image,0);  /* user mask data */
        }
      /*
        Write composite image.
      */
      compression=image->compression;
      if (image_info->compression != UndefinedCompression)
        image->compression=image_info->compression;
      if (image->compression == ZipCompression)
        image->compression=RLECompression;
      if (WritePSDChannels(&psd_info,image_info,image,image,0,MagickFalse,
          exception) == 0)
        status=MagickFalse;
      image->compression=compression;
    }
  (void) CloseBlob(image);
  return(status);
}