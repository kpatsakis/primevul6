static MagickBooleanType WriteJP2Image(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  const char
    *option,
    *property;

  int
    jp2_status;

  MagickBooleanType
    status;

  opj_codec_t
    *jp2_codec;

  OPJ_COLOR_SPACE
    jp2_colorspace;

  opj_cparameters_t
    parameters;

  opj_image_cmptparm_t
    jp2_info[5];

  opj_image_t
    *jp2_image;

  opj_stream_t
    *jp2_stream;

  register ssize_t
    i;

  ssize_t
    y;

  unsigned int
    channels;

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
  /*
    Initialize JPEG 2000 API.
  */
  opj_set_default_encoder_parameters(&parameters);
  for (i=1; i < 6; i++)
    if (((size_t) (1UL << (i+2)) > image->columns) &&
        ((size_t) (1UL << (i+2)) > image->rows))
      break;
  parameters.numresolution=i;
  option=GetImageOption(image_info,"jp2:number-resolutions");
  if (option != (const char *) NULL)
    parameters.numresolution=StringToInteger(option);
  parameters.tcp_numlayers=1;
  parameters.tcp_rates[0]=0;  /* lossless */
  parameters.cp_disto_alloc=1;
  if ((image_info->quality != 0) && (image_info->quality != 100))
    {
      parameters.tcp_distoratio[0]=(double) image_info->quality;
      parameters.cp_fixed_quality=OPJ_TRUE;
    }
  if (image_info->extract != (char *) NULL)
    {
      RectangleInfo
        geometry;

      int
        flags;

      /*
        Set tile size.
      */
      flags=ParseAbsoluteGeometry(image_info->extract,&geometry);
      parameters.cp_tdx=(int) geometry.width;
      parameters.cp_tdy=(int) geometry.width;
      if ((flags & HeightValue) != 0)
        parameters.cp_tdy=(int) geometry.height;
      if ((flags & XValue) != 0)
        parameters.cp_tx0=geometry.x;
      if ((flags & YValue) != 0)
        parameters.cp_ty0=geometry.y;
      parameters.tile_size_on=OPJ_TRUE;
    }
  option=GetImageOption(image_info,"jp2:quality");
  if (option != (const char *) NULL)
    {
      register const char
        *p;

      /*
        Set quality PSNR.
      */
      p=option;
      for (i=0; sscanf(p,"%f",&parameters.tcp_distoratio[i]) == 1; i++)
      {
        if (i > 100)
          break;
        while ((*p != '\0') && (*p != ','))
          p++;
        if (*p == '\0')
          break;
        p++;
      }
      parameters.tcp_numlayers=i+1;
      parameters.cp_fixed_quality=OPJ_TRUE;
    }
  option=GetImageOption(image_info,"jp2:progression-order");
  if (option != (const char *) NULL)
    {
      if (LocaleCompare(option,"LRCP") == 0)
        parameters.prog_order=OPJ_LRCP;
      if (LocaleCompare(option,"RLCP") == 0)
        parameters.prog_order=OPJ_RLCP;
      if (LocaleCompare(option,"RPCL") == 0)
        parameters.prog_order=OPJ_RPCL;
      if (LocaleCompare(option,"PCRL") == 0)
        parameters.prog_order=OPJ_PCRL;
      if (LocaleCompare(option,"CPRL") == 0)
        parameters.prog_order=OPJ_CPRL;
    }
  option=GetImageOption(image_info,"jp2:rate");
  if (option != (const char *) NULL)
    {
      register const char
        *p;

      /*
        Set compression rate.
      */
      p=option;
      for (i=0; sscanf(p,"%f",&parameters.tcp_rates[i]) == 1; i++)
      {
        if (i >= 100)
          break;
        while ((*p != '\0') && (*p != ','))
          p++;
        if (*p == '\0')
          break;
        p++;
      }
      parameters.tcp_numlayers=i+1;
      parameters.cp_disto_alloc=OPJ_TRUE;
    }
  if (image_info->sampling_factor != (const char *) NULL)
    (void) sscanf(image_info->sampling_factor,"%d,%d",
      &parameters.subsampling_dx,&parameters.subsampling_dy);
  property=GetImageProperty(image,"comment",exception);
  if (property != (const char *) NULL)
    parameters.cp_comment=ConstantString(property);
  channels=3;
  jp2_colorspace=OPJ_CLRSPC_SRGB;
  if (image->colorspace == YUVColorspace)
    {
      jp2_colorspace=OPJ_CLRSPC_SYCC;
      parameters.subsampling_dx=2;
    }
  else
    {
      if (IsGrayColorspace(image->colorspace) != MagickFalse)
        {
          channels=1;
          jp2_colorspace=OPJ_CLRSPC_GRAY;
        }
      else
        (void) TransformImageColorspace(image,sRGBColorspace,exception);
      if (image->alpha_trait != UndefinedPixelTrait)
        channels++;
    }
  parameters.tcp_mct=channels == 3 ? 1 : 0;
  ResetMagickMemory(jp2_info,0,sizeof(jp2_info));
  for (i=0; i < (ssize_t) channels; i++)
  {
    jp2_info[i].prec=(OPJ_UINT32) image->depth;
    jp2_info[i].bpp=(OPJ_UINT32) image->depth;
    if ((image->depth == 1) &&
        ((LocaleCompare(image_info->magick,"JPT") == 0) ||
         (LocaleCompare(image_info->magick,"JP2") == 0)))
      {
        jp2_info[i].prec++;  /* OpenJPEG returns exception for depth @ 1 */
        jp2_info[i].bpp++;
      }
    jp2_info[i].sgnd=0;
    jp2_info[i].dx=parameters.subsampling_dx;
    jp2_info[i].dy=parameters.subsampling_dy;
    jp2_info[i].w=(OPJ_UINT32) image->columns;
    jp2_info[i].h=(OPJ_UINT32) image->rows;
  }
  jp2_image=opj_image_create((OPJ_UINT32) channels,jp2_info,jp2_colorspace);
  if (jp2_image == (opj_image_t *) NULL)
    ThrowWriterException(DelegateError,"UnableToEncodeImageFile");
  jp2_image->x0=parameters.image_offset_x0;
  jp2_image->y0=parameters.image_offset_y0;
  jp2_image->x1=(unsigned int) (2*parameters.image_offset_x0+(image->columns-1)*
    parameters.subsampling_dx+1);
  jp2_image->y1=(unsigned int) (2*parameters.image_offset_y0+(image->rows-1)*
    parameters.subsampling_dx+1);
  if ((image->depth == 12) &&
      ((image->columns == 2048) || (image->rows == 1080) ||
       (image->columns == 4096) || (image->rows == 2160)))
    CinemaProfileCompliance(jp2_image,&parameters);
  if (channels == 4)
    jp2_image->comps[3].alpha=1;
  else
   if ((channels == 2) && (jp2_colorspace == OPJ_CLRSPC_GRAY))
     jp2_image->comps[1].alpha=1;
  /*
    Convert to JP2 pixels.
  */
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *p;

    ssize_t
      x;

    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      for (i=0; i < (ssize_t) channels; i++)
      {
        double
          scale;

        register int
          *q;

        scale=(double) ((1UL << jp2_image->comps[i].prec)-1)/QuantumRange;
        q=jp2_image->comps[i].data+(y/jp2_image->comps[i].dy*
          image->columns/jp2_image->comps[i].dx+x/jp2_image->comps[i].dx);
        switch (i)
        {
          case 0:
          {
            if (jp2_colorspace == OPJ_CLRSPC_GRAY)
              {
                *q=(int) (scale*GetPixelLuma(image,p));
                break;
              }
            *q=(int) (scale*GetPixelRed(image,p));
            break;
          }
          case 1:
          {
            if (jp2_colorspace == OPJ_CLRSPC_GRAY)
              {
                *q=(int) (scale*GetPixelAlpha(image,p));
                break;
              }
            *q=(int) (scale*GetPixelGreen(image,p));
            break;
          }
          case 2:
          {
            *q=(int) (scale*GetPixelBlue(image,p));
            break;
          }
          case 3:
          {
            *q=(int) (scale*GetPixelAlpha(image,p));
            break;
          }
        }
      }
      p+=GetPixelChannels(image);
    }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  if (LocaleCompare(image_info->magick,"JPT") == 0)
    jp2_codec=opj_create_compress(OPJ_CODEC_JPT);
  else
    if (LocaleCompare(image_info->magick,"J2K") == 0)
      jp2_codec=opj_create_compress(OPJ_CODEC_J2K);
    else
      jp2_codec=opj_create_compress(OPJ_CODEC_JP2);
  opj_set_warning_handler(jp2_codec,JP2WarningHandler,exception);
  opj_set_error_handler(jp2_codec,JP2ErrorHandler,exception);
  opj_setup_encoder(jp2_codec,&parameters,jp2_image);
  jp2_stream=opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE,OPJ_FALSE);
  opj_stream_set_read_function(jp2_stream,JP2ReadHandler);
  opj_stream_set_write_function(jp2_stream,JP2WriteHandler);
  opj_stream_set_seek_function(jp2_stream,JP2SeekHandler);
  opj_stream_set_skip_function(jp2_stream,JP2SkipHandler);
  opj_stream_set_user_data(jp2_stream,image,NULL);
  if (jp2_stream == (opj_stream_t *) NULL)
    ThrowWriterException(DelegateError,"UnableToEncodeImageFile");
  jp2_status=opj_start_compress(jp2_codec,jp2_image,jp2_stream);
  if (jp2_status == 0)
    ThrowWriterException(DelegateError,"UnableToEncodeImageFile");
  if ((opj_encode(jp2_codec,jp2_stream) == 0) ||
      (opj_end_compress(jp2_codec,jp2_stream) == 0))
    {
      opj_stream_destroy(jp2_stream);
      opj_destroy_codec(jp2_codec);
      opj_image_destroy(jp2_image);
      ThrowWriterException(DelegateError,"UnableToEncodeImageFile");
    }
  /*
    Free resources.
  */
  opj_stream_destroy(jp2_stream);
  opj_destroy_codec(jp2_codec);
  opj_image_destroy(jp2_image);
  (void) CloseBlob(image);
  return(MagickTrue);
}