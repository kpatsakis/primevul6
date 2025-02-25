static Image *ReadPSDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    skip_layers;

  MagickOffsetType
    offset;

  MagickSizeType
    length;

  MagickBooleanType
    status;

  PSDInfo
    psd_info;

  ssize_t
    i;

  size_t
    image_list_length;

  ssize_t
    count;

  StringInfo
    *profile;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);

  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read image header.
  */
  image->endian=MSBEndian;
  count=ReadBlob(image,4,(unsigned char *) psd_info.signature);
  psd_info.version=ReadBlobMSBShort(image);
  if ((count != 4) || (LocaleNCompare(psd_info.signature,"8BPS",4) != 0) ||
      ((psd_info.version != 1) && (psd_info.version != 2)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  (void) ReadBlob(image,6,psd_info.reserved);
  psd_info.channels=ReadBlobMSBShort(image);
  if (psd_info.channels < 1)
    ThrowReaderException(CorruptImageError,"MissingImageChannel");
  if (psd_info.channels > MaxPSDChannels)
    ThrowReaderException(CorruptImageError,"MaximumChannelsExceeded");
  psd_info.rows=ReadBlobMSBLong(image);
  psd_info.columns=ReadBlobMSBLong(image);
  if ((psd_info.version == 1) && ((psd_info.rows > 30000) ||
      (psd_info.columns > 30000)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  psd_info.depth=ReadBlobMSBShort(image);
  if ((psd_info.depth != 1) && (psd_info.depth != 8) &&
      (psd_info.depth != 16) && (psd_info.depth != 32))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  psd_info.mode=ReadBlobMSBShort(image);
  if ((psd_info.mode == IndexedMode) && (psd_info.channels > 3))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Image is %.20g x %.20g with channels=%.20g, depth=%.20g, mode=%s",
      (double) psd_info.columns,(double) psd_info.rows,(double)
      psd_info.channels,(double) psd_info.depth,ModeToString((PSDImageType)
      psd_info.mode));
  if (EOFBlob(image) != MagickFalse)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  /*
    Initialize image.
  */
  image->depth=psd_info.depth;
  image->columns=psd_info.columns;
  image->rows=psd_info.rows;
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  status=ResetImagePixels(image,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  psd_info.min_channels=3;
  switch (psd_info.mode)
  {
    case LabMode:
    {
      (void) SetImageColorspace(image,LabColorspace,exception);
      break;
    }
    case CMYKMode:
    {
      psd_info.min_channels=4;
      (void) SetImageColorspace(image,CMYKColorspace,exception);
      break;
    }
    case BitmapMode:
    case GrayscaleMode:
    case DuotoneMode:
    {
      if (psd_info.depth != 32)
        {
          status=AcquireImageColormap(image,MagickMin((size_t)
            (psd_info.depth < 16 ? 256 : 65536), MaxColormapSize),exception);
          if (status == MagickFalse)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          if (image->debug != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Image colormap allocated");
        }
      psd_info.min_channels=1;
      (void) SetImageColorspace(image,GRAYColorspace,exception);
      break;
    }
    case IndexedMode:
    {
      psd_info.min_channels=1;
      break;
    }
    case MultichannelMode:
    {
      if ((psd_info.channels > 0) && (psd_info.channels < 3))
        {
          psd_info.min_channels=psd_info.channels;
          (void) SetImageColorspace(image,GRAYColorspace,exception);
        }
      break;
    }
  }
  if (psd_info.channels < psd_info.min_channels)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  /*
    Read PSD raster colormap only present for indexed and duotone images.
  */
  length=ReadBlobMSBLong(image);
  if ((psd_info.mode == IndexedMode) && (length < 3))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (length != 0)
    {
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  reading colormap");
      if ((psd_info.mode == DuotoneMode) || (psd_info.depth == 32))
        {
          /*
            Duotone image data;  the format of this data is undocumented.
            32 bits per pixel;  the colormap is ignored.
          */
          (void) SeekBlob(image,(const MagickOffsetType) length,SEEK_CUR);
        }
      else
        {
          size_t
            number_colors;

          /*
            Read PSD raster colormap.
          */
          number_colors=(size_t) length/3;
          if (number_colors > 65536)
            ThrowReaderException(CorruptImageError,"ImproperImageHeader");
          if (AcquireImageColormap(image,number_colors,exception) == MagickFalse)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          for (i=0; i < (ssize_t) image->colors; i++)
            image->colormap[i].red=(MagickRealType) ScaleCharToQuantum(
              (unsigned char) ReadBlobByte(image));
          for (i=0; i < (ssize_t) image->colors; i++)
            image->colormap[i].green=(MagickRealType) ScaleCharToQuantum(
              (unsigned char) ReadBlobByte(image));
          for (i=0; i < (ssize_t) image->colors; i++)
            image->colormap[i].blue=(MagickRealType) ScaleCharToQuantum(
              (unsigned char) ReadBlobByte(image));
          image->alpha_trait=UndefinedPixelTrait;
        }
    }
  if ((image->depth == 1) && (image->storage_class != PseudoClass))
    ThrowReaderException(CorruptImageError, "ImproperImageHeader");
  psd_info.has_merged_image=MagickTrue;
  profile=(StringInfo *) NULL;
  length=ReadBlobMSBLong(image);
  if (length != 0)
    {
      unsigned char
        *blocks;

      /*
        Image resources block.
      */
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  reading image resource blocks - %.20g bytes",(double)
          ((MagickOffsetType) length));
      if (length > GetBlobSize(image))
        ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
      blocks=(unsigned char *) AcquireQuantumMemory((size_t) length,
        sizeof(*blocks));
      if (blocks == (unsigned char *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      count=ReadBlob(image,(size_t) length,blocks);
      if ((count != (ssize_t) length) || (length < 4) ||
          (LocaleNCompare((char *) blocks,"8BIM",4) != 0))
        {
          blocks=(unsigned char *) RelinquishMagickMemory(blocks);
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        }
      profile=ParseImageResourceBlocks(&psd_info,image,blocks,(size_t) length);
      blocks=(unsigned char *) RelinquishMagickMemory(blocks);
    }
  /*
    Layer and mask block.
  */
  length=GetPSDSize(&psd_info,image);
  if (length == 8)
    {
      length=ReadBlobMSBLong(image);
      length=ReadBlobMSBLong(image);
    }
  offset=TellBlob(image);
  skip_layers=MagickFalse;
  if ((image_info->number_scenes == 1) && (image_info->scene == 0) &&
      (psd_info.has_merged_image != MagickFalse))
    {
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  read composite only");
      skip_layers=MagickTrue;
    }
  if (length == 0)
    {
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  image has no layers");
    }
  else
    {
      if (ReadPSDLayersInternal(image,image_info,&psd_info,skip_layers,
            exception) != MagickTrue)
        {
          if (profile != (StringInfo *) NULL)
            profile=DestroyStringInfo(profile);
          (void) CloseBlob(image);
          image=DestroyImageList(image);
          return((Image *) NULL);
        }

      /*
         Skip the rest of the layer and mask information.
      */
      (void) SeekBlob(image,offset+length,SEEK_SET);
    }
  /*
    If we are only "pinging" the image, then we're done - so return.
  */
  if (EOFBlob(image) != MagickFalse)
    {
      if (profile != (StringInfo *) NULL)
        profile=DestroyStringInfo(profile);
      ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
    }
  if (image_info->ping != MagickFalse)
    {
      if (profile != (StringInfo *) NULL)
        profile=DestroyStringInfo(profile);
      (void) CloseBlob(image);
      return(GetFirstImageInList(image));
    }
  /*
    Read the precombined layer, present for PSD < 4 compatibility.
  */
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  reading the precombined layer");
  image_list_length=GetImageListLength(image);
  if ((psd_info.has_merged_image != MagickFalse) || (image_list_length == 1))
    psd_info.has_merged_image=(MagickBooleanType) ReadPSDMergedImage(
      image_info,image,&psd_info,exception);
  if ((psd_info.has_merged_image == MagickFalse) && (image_list_length == 1) &&
      (length != 0))
    {
      (void) SeekBlob(image,offset,SEEK_SET);
      status=ReadPSDLayersInternal(image,image_info,&psd_info,MagickFalse,
        exception);
      if (status != MagickTrue)
        {
          if (profile != (StringInfo *) NULL)
            profile=DestroyStringInfo(profile);
          (void) CloseBlob(image);
          image=DestroyImageList(image);
          return((Image *) NULL);
        }
      image_list_length=GetImageListLength(image);
    }
  if (psd_info.has_merged_image == MagickFalse)
    {
      Image
        *merged;

      if (image_list_length == 1)
        {
          if (profile != (StringInfo *) NULL)
            profile=DestroyStringInfo(profile);
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        }
      image->background_color.alpha=(MagickRealType) TransparentAlpha;
      image->background_color.alpha_trait=BlendPixelTrait;
      (void) SetImageBackgroundColor(image,exception);
      merged=MergeImageLayers(image,FlattenLayer,exception);
      if (merged == (Image *) NULL)
        {
          (void) CloseBlob(image);
          image=DestroyImageList(image);
          return((Image *) NULL);
        }
      ReplaceImageInList(&image,merged);
    }
  if (profile != (StringInfo *) NULL)
    {
      const char
        *option;

      Image
        *next;

      MagickBooleanType
        replicate_profile;

      option=GetImageOption(image_info,"psd:replicate-profile");
      replicate_profile=IsStringTrue(option);
      i=0;
      next=image;
      while (next != (Image *) NULL)
      {
        if (PSDSkipImage(&psd_info,image_info,i++) == MagickFalse)
          {
            (void) SetImageProfile(next,GetStringInfoName(profile),profile,
              exception);
            if (replicate_profile == MagickFalse)
              break;
          }
        next=next->next;
      }
      profile=DestroyStringInfo(profile);
    }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}