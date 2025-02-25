static Image *ReadOnePNGImage(MngInfo *mng_info,
    const ImageInfo *image_info, ExceptionInfo *exception)
{
  /* Read one PNG image */

  /* To do: Read the tEXt/Creation Time chunk into the date:create property */

  Image
    *image;

  char
    im_vers[32],
    libpng_runv[32],
    libpng_vers[32],
    zlib_runv[32],
    zlib_vers[32];

  int
    intent, /* "PNG Rendering intent", which is ICC intent + 1 */
    num_raw_profiles,
    num_text,
    num_text_total,
    num_passes,
    number_colors,
    pass,
    ping_bit_depth,
    ping_color_type,
    ping_file_depth,
    ping_interlace_method,
    ping_compression_method,
    ping_filter_method,
    ping_num_trans,
    unit_type;

  double
    file_gamma;

  MagickBooleanType
    logging,
    ping_found_cHRM,
    ping_found_gAMA,
    ping_found_iCCP,
    ping_found_sRGB,
    ping_found_sRGB_cHRM,
    ping_preserve_iCCP,
    status;

  MemoryInfo
    *volatile pixel_info;

  PixelInfo
    transparent_color;

  PNGErrorInfo
    error_info;

  png_bytep
     ping_trans_alpha;

  png_color_16p
     ping_background,
     ping_trans_color;

  png_info
    *end_info,
    *ping_info;

  png_struct
    *ping;

  png_textp
    text;

  png_uint_32
    ping_height,
    ping_width,
    x_resolution,
    y_resolution;

  QuantumInfo
    *quantum_info;

  ssize_t
    ping_rowbytes,
    y;

  register unsigned char
    *p;

  register ssize_t
    i,
    x;

  register Quantum
    *q;

  size_t
    length,
    row_offset;

  ssize_t
    j;

  unsigned char
    *ping_pixels;

#ifdef PNG_UNKNOWN_CHUNKS_SUPPORTED
  png_byte unused_chunks[]=
  {
    104,  73,  83,  84, (png_byte) '\0',   /* hIST */
    105,  84,  88, 116, (png_byte) '\0',   /* iTXt */
    112,  67,  65,  76, (png_byte) '\0',   /* pCAL */
    115,  67,  65,  76, (png_byte) '\0',   /* sCAL */
    115,  80,  76,  84, (png_byte) '\0',   /* sPLT */
#if !defined(PNG_tIME_SUPPORTED)
    116,  73,  77,  69, (png_byte) '\0',   /* tIME */
#endif
#ifdef PNG_APNG_SUPPORTED /* libpng was built with APNG patch; */
                          /* ignore the APNG chunks */
     97,  99,  84,  76, (png_byte) '\0',   /* acTL */
    102,  99,  84,  76, (png_byte) '\0',   /* fcTL */
    102, 100,  65,  84, (png_byte) '\0',   /* fdAT */
#endif
  };
#endif

  /* Define these outside of the following "if logging()" block so they will
   * show in debuggers.
   */
  *im_vers='\0';
  (void) ConcatenateMagickString(im_vers,
         MagickLibVersionText,32);
  (void) ConcatenateMagickString(im_vers,
         MagickLibAddendum,32);

  *libpng_vers='\0';
  (void) ConcatenateMagickString(libpng_vers,
         PNG_LIBPNG_VER_STRING,32);
  *libpng_runv='\0';
  (void) ConcatenateMagickString(libpng_runv,
         png_get_libpng_ver(NULL),32);

  *zlib_vers='\0';
  (void) ConcatenateMagickString(zlib_vers,
         ZLIB_VERSION,32);
  *zlib_runv='\0';
  (void) ConcatenateMagickString(zlib_runv,
         zlib_version,32);

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
       "  Enter ReadOnePNGImage()\n"
       "    IM version     = %s\n"
       "    Libpng version = %s",
       im_vers, libpng_vers);

  if (logging != MagickFalse)
  {
    if (LocaleCompare(libpng_vers,libpng_runv) != 0)
    {
   (void) LogMagickEvent(CoderEvent,GetMagickModule(),"      running with   %s",
        libpng_runv);
    }
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"    Zlib version   = %s",
        zlib_vers);
    if (LocaleCompare(zlib_vers,zlib_runv) != 0)
    {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"      running with   %s",
        zlib_runv);
    }
  }

#if (PNG_LIBPNG_VER < 10200)
  if (image_info->verbose)
    printf("Your PNG library (libpng-%s) is rather old.\n",
       PNG_LIBPNG_VER_STRING);
#endif

#if (PNG_LIBPNG_VER >= 10400)
#  ifndef  PNG_TRANSFORM_GRAY_TO_RGB    /* Added at libpng-1.4.0beta67 */
  if (image_info->verbose)
    {
      printf("Your PNG library (libpng-%s) is an old beta version.\n",
           PNG_LIBPNG_VER_STRING);
      printf("Please update it.\n");
    }
#  endif
#endif


  quantum_info = (QuantumInfo *) NULL;
  image=mng_info->image;

  if (logging != MagickFalse)
  {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "    Before reading:\n"
       "      image->alpha_trait=%d"
       "      image->rendering_intent=%d\n"
       "      image->colorspace=%d\n"
       "      image->gamma=%f",
       (int) image->alpha_trait, (int) image->rendering_intent,
       (int) image->colorspace, image->gamma);
  }
  intent=
    Magick_RenderingIntent_to_PNG_RenderingIntent(image->rendering_intent);

  /* Set to an out-of-range color unless tRNS chunk is present */
  transparent_color.red=65537;
  transparent_color.green=65537;
  transparent_color.blue=65537;
  transparent_color.alpha=65537;

  number_colors=0;
  num_text = 0;
  num_text_total = 0;
  num_raw_profiles = 0;

  ping_found_cHRM = MagickFalse;
  ping_found_gAMA = MagickFalse;
  ping_found_iCCP = MagickFalse;
  ping_found_sRGB = MagickFalse;
  ping_found_sRGB_cHRM = MagickFalse;
  ping_preserve_iCCP = MagickFalse;


  /*
    Allocate the PNG structures
  */
#ifdef PNG_USER_MEM_SUPPORTED
 error_info.image=image;
 error_info.exception=exception;
 ping=png_create_read_struct_2(PNG_LIBPNG_VER_STRING,&error_info,
   MagickPNGErrorHandler,MagickPNGWarningHandler, NULL,
   (png_malloc_ptr) Magick_png_malloc,(png_free_ptr) Magick_png_free);
#else
  ping=png_create_read_struct(PNG_LIBPNG_VER_STRING,&error_info,
    MagickPNGErrorHandler,MagickPNGWarningHandler);
#endif
  if (ping == (png_struct *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

  ping_info=png_create_info_struct(ping);

  if (ping_info == (png_info *) NULL)
    {
      png_destroy_read_struct(&ping,(png_info **) NULL,(png_info **) NULL);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }

  end_info=png_create_info_struct(ping);

  if (end_info == (png_info *) NULL)
    {
      png_destroy_read_struct(&ping,&ping_info,(png_info **) NULL);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }

  pixel_info=(MemoryInfo *) NULL;

  if (setjmp(png_jmpbuf(ping)))
    {
      /*
        PNG image is corrupt.
      */
      png_destroy_read_struct(&ping,&ping_info,&end_info);

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
      UnlockSemaphoreInfo(ping_semaphore);
#endif

      if (pixel_info != (MemoryInfo *) NULL)
        pixel_info=RelinquishVirtualMemory(pixel_info);

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  exit ReadOnePNGImage() with error.");

      return(GetFirstImageInList(image));
    }

  /* {  For navigation to end of SETJMP-protected block.  Within this
   *    block, use png_error() instead of Throwing an Exception, to ensure
   *    that libpng is able to clean up, and that the semaphore is unlocked.
   */

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
  LockSemaphoreInfo(ping_semaphore);
#endif

#ifdef PNG_BENIGN_ERRORS_SUPPORTED
  /* Allow benign errors */
  png_set_benign_errors(ping, 1);
#endif

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
  /* Reject images with too many rows or columns */
  png_set_user_limits(ping,
    (png_uint_32) MagickMin(0x7fffffffL,
        GetMagickResourceLimit(WidthResource)),
    (png_uint_32) MagickMin(0x7fffffffL,
        GetMagickResourceLimit(HeightResource)));
#endif /* PNG_SET_USER_LIMITS_SUPPORTED */

  /*
    Prepare PNG for reading.
  */

  mng_info->image_found++;
  png_set_sig_bytes(ping,8);

  if (LocaleCompare(image_info->magick,"MNG") == 0)
    {
#if defined(PNG_MNG_FEATURES_SUPPORTED)
      (void) png_permit_mng_features(ping,PNG_ALL_MNG_FEATURES);
      png_set_read_fn(ping,image,png_get_data);
#else
#if defined(PNG_READ_EMPTY_PLTE_SUPPORTED)
      png_permit_empty_plte(ping,MagickTrue);
      png_set_read_fn(ping,image,png_get_data);
#else
      mng_info->image=image;
      mng_info->bytes_in_read_buffer=0;
      mng_info->found_empty_plte=MagickFalse;
      mng_info->have_saved_bkgd_index=MagickFalse;
      png_set_read_fn(ping,mng_info,mng_get_data);
#endif
#endif
    }

  else
    png_set_read_fn(ping,image,png_get_data);

  {
    const char
      *value;

    value=GetImageOption(image_info,"profile:skip");

    if (IsOptionMember("ICC",value) == MagickFalse)
    {

       value=GetImageOption(image_info,"png:preserve-iCCP");

       if (value == NULL)
          value=GetImageArtifact(image,"png:preserve-iCCP");

       if (value != NULL)
          ping_preserve_iCCP=MagickTrue;

#if defined(PNG_SKIP_sRGB_CHECK_PROFILE) && defined(PNG_SET_OPTION_SUPPORTED)
       /* Don't let libpng check for ICC/sRGB profile because we're going
        * to do that anyway.  This feature was added at libpng-1.6.12.
        * If logging, go ahead and check and issue a warning as appropriate.
        */
       if (logging == MagickFalse)
          png_set_option(ping, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);
#endif
    }
#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
    else
    {
       png_set_keep_unknown_chunks(ping, 1, (png_bytep) mng_iCCP, 1);
    }
#endif
  }
#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
  /* Ignore unused chunks and all unknown chunks except for eXIf,
     zxIf, caNv, and vpAg */
# if PNG_LIBPNG_VER < 10700 /* Avoid libpng16 warning */
  png_set_keep_unknown_chunks(ping, 2, NULL, 0);
# else
  png_set_keep_unknown_chunks(ping, 1, NULL, 0);
# endif
#if defined(zxIf_SUPPORTED)
  png_set_keep_unknown_chunks(ping, 2, (png_bytep) mng_uxIf, 1);
  png_set_keep_unknown_chunks(ping, 2, (png_bytep) mng_zxIf, 1);
#endif
  png_set_keep_unknown_chunks(ping, 2, (png_bytep) mng_eXIf, 1);
  png_set_keep_unknown_chunks(ping, 2, (png_bytep) mng_caNv, 1);
  png_set_keep_unknown_chunks(ping, 2, (png_bytep) mng_vpAg, 1);
  png_set_keep_unknown_chunks(ping, 1, unused_chunks,
     (int)sizeof(unused_chunks)/5);
  /* Callback for other unknown chunks */
  png_set_read_user_chunk_fn(ping, image, read_user_chunk_callback);
#endif

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
#  if (PNG_LIBPNG_VER >= 10400)
    /* Limit the size of the chunk storage cache used for sPLT, text,
     * and unknown chunks.
     */
    png_set_chunk_cache_max(ping, 32767);
#  endif
#endif

#ifdef PNG_READ_CHECK_FOR_INVALID_INDEX_SUPPORTED
    /* Disable new libpng-1.5.10 feature */
    png_set_check_for_invalid_index (ping, 0);
#endif

#if (PNG_LIBPNG_VER < 10400)
#  if defined(PNG_USE_PNGGCCRD) && defined(PNG_ASSEMBLER_CODE_SUPPORTED) && \
   (PNG_LIBPNG_VER >= 10200) && (PNG_LIBPNG_VER < 10220) && defined(__i386__)
  /* Disable thread-unsafe features of pnggccrd */
  if (png_access_version_number() >= 10200)
  {
    png_uint_32 mmx_disable_mask=0;
    png_uint_32 asm_flags;

    mmx_disable_mask |= ( PNG_ASM_FLAG_MMX_READ_COMBINE_ROW  \
                        | PNG_ASM_FLAG_MMX_READ_FILTER_SUB   \
                        | PNG_ASM_FLAG_MMX_READ_FILTER_AVG   \
                        | PNG_ASM_FLAG_MMX_READ_FILTER_PAETH );
    asm_flags=png_get_asm_flags(ping);
    png_set_asm_flags(ping, asm_flags & ~mmx_disable_mask);
  }
#  endif
#endif

  png_read_info(ping,ping_info);

  png_get_IHDR(ping,ping_info,&ping_width,&ping_height,
               &ping_bit_depth,&ping_color_type,
               &ping_interlace_method,&ping_compression_method,
               &ping_filter_method);

  ping_file_depth = ping_bit_depth;

  /* Swap bytes if requested */
  if (ping_file_depth == 16)
  {
     const char
       *value;

     value=GetImageOption(image_info,"png:swap-bytes");

     if (value == NULL)
        value=GetImageArtifact(image,"png:swap-bytes");

     if (value != NULL)
        png_set_swap(ping);
  }

  /* Save bit-depth and color-type in case we later want to write a PNG00 */
  {
      char
        msg[MagickPathExtent];

      (void) FormatLocaleString(msg,MagickPathExtent,"%d",
         (int) ping_color_type);
      (void) SetImageProperty(image,"png:IHDR.color-type-orig",msg,exception);

      (void) FormatLocaleString(msg,MagickPathExtent,"%d",
         (int) ping_bit_depth);
      (void) SetImageProperty(image,"png:IHDR.bit-depth-orig",msg,exception);
  }

  (void) png_get_tRNS(ping, ping_info, &ping_trans_alpha, &ping_num_trans,
                      &ping_trans_color);

  (void) png_get_bKGD(ping, ping_info, &ping_background);

  if (ping_bit_depth < 8)
    {
       png_set_packing(ping);
       ping_bit_depth = 8;
    }

  image->depth=ping_bit_depth;
  image->depth=GetImageQuantumDepth(image,MagickFalse);
  image->interlace=ping_interlace_method != 0 ? PNGInterlace : NoInterlace;

  if (((int) ping_color_type == PNG_COLOR_TYPE_GRAY) ||
      ((int) ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
    {
      image->rendering_intent=UndefinedIntent;
      intent=Magick_RenderingIntent_to_PNG_RenderingIntent(UndefinedIntent);
      (void) ResetMagickMemory(&image->chromaticity,0,
        sizeof(image->chromaticity));
    }

  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    PNG width: %.20g, height: %.20g\n"
        "    PNG color_type: %d, bit_depth: %d\n"
        "    PNG compression_method: %d\n"
        "    PNG interlace_method: %d, filter_method: %d",
        (double) ping_width, (double) ping_height,
        ping_color_type, ping_bit_depth,
        ping_compression_method,
        ping_interlace_method,ping_filter_method);

    }

  if (png_get_valid(ping,ping_info, PNG_INFO_iCCP))
    {
      ping_found_iCCP=MagickTrue;
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Found PNG iCCP chunk.");
    }

  if (png_get_valid(ping,ping_info,PNG_INFO_gAMA))
    {
      ping_found_gAMA=MagickTrue;
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Found PNG gAMA chunk.");
    }

  if (png_get_valid(ping,ping_info,PNG_INFO_cHRM))
    {
      ping_found_cHRM=MagickTrue;
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Found PNG cHRM chunk.");
    }

  if (ping_found_iCCP != MagickTrue && png_get_valid(ping,ping_info,
      PNG_INFO_sRGB))
    {
      ping_found_sRGB=MagickTrue;
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Found PNG sRGB chunk.");
    }

#ifdef PNG_READ_iCCP_SUPPORTED
    if (ping_found_iCCP !=MagickTrue &&
      ping_found_sRGB != MagickTrue &&
      png_get_valid(ping,ping_info, PNG_INFO_iCCP))
    {
      ping_found_iCCP=MagickTrue;
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Found PNG iCCP chunk.");
    }

  if (png_get_valid(ping,ping_info,PNG_INFO_iCCP))
    {
      int
        compression;

#if (PNG_LIBPNG_VER < 10500)
      png_charp
        info;
#else
      png_bytep
        info;
#endif

      png_charp
        name;

      png_uint_32
        profile_length;

      (void) png_get_iCCP(ping,ping_info,&name,(int *) &compression,&info,
        &profile_length);

      if (profile_length != 0)
        {
          StringInfo
            *profile;

          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Reading PNG iCCP chunk.");

          profile=BlobToStringInfo(info,profile_length);

          if (profile == (StringInfo *) NULL)
          {
            png_warning(ping, "ICC profile is NULL");
            profile=DestroyStringInfo(profile);
          }
          else
          {
            if (ping_preserve_iCCP == MagickFalse)
            {
                 int
                   icheck,
                   got_crc=0;


                 png_uint_32
                   length,
                   profile_crc=0;

                 unsigned char
                   *data;

                 length=(png_uint_32) GetStringInfoLength(profile);

                 for (icheck=0; sRGB_info[icheck].len > 0; icheck++)
                 {
                   if (length == sRGB_info[icheck].len)
                   {
                     if (got_crc == 0)
                     {
                       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                         "    Got a %lu-byte ICC profile (potentially sRGB)",
                         (unsigned long) length);

                       data=GetStringInfoDatum(profile);
                       profile_crc=crc32(0,data,length);

                       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                           "      with crc=%8x",(unsigned int) profile_crc);
                       got_crc++;
                     }

                     if (profile_crc == sRGB_info[icheck].crc)
                     {
                        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "      It is sRGB with rendering intent = %s",
                        Magick_RenderingIntentString_from_PNG_RenderingIntent(
                             sRGB_info[icheck].intent));
                        if (image->rendering_intent==UndefinedIntent)
                        {
                          image->rendering_intent=
                          Magick_RenderingIntent_from_PNG_RenderingIntent(
                             sRGB_info[icheck].intent);
                        }
                        break;
                     }
                   }
                 }
                 if (sRGB_info[icheck].len == 0)
                 {
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "    Got %lu-byte ICC profile not recognized as sRGB",
                        (unsigned long) length);
                    (void) SetImageProfile(image,"icc",profile,exception);
                 }
            }
            else /* Preserve-iCCP */
            {
                    (void) SetImageProfile(image,"icc",profile,exception);
            }

            profile=DestroyStringInfo(profile);
          }
      }
    }
#endif

#if defined(PNG_READ_sRGB_SUPPORTED)
  {
    if (ping_found_iCCP==MagickFalse && png_get_valid(ping,ping_info,
        PNG_INFO_sRGB))
    {
      if (png_get_sRGB(ping,ping_info,&intent))
      {
        if (image->rendering_intent == UndefinedIntent)
          image->rendering_intent=
             Magick_RenderingIntent_from_PNG_RenderingIntent (intent);

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Reading PNG sRGB chunk: rendering_intent: %d",intent);
      }
    }

    else if (mng_info->have_global_srgb)
      {
        if (image->rendering_intent == UndefinedIntent)
          image->rendering_intent=
            Magick_RenderingIntent_from_PNG_RenderingIntent
            (mng_info->global_srgb_intent);
      }
  }
#endif


  {
     if (!png_get_gAMA(ping,ping_info,&file_gamma))
       if (mng_info->have_global_gama)
         png_set_gAMA(ping,ping_info,mng_info->global_gamma);

     if (png_get_gAMA(ping,ping_info,&file_gamma))
       {
         image->gamma=(float) file_gamma;
         if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "    Reading PNG gAMA chunk: gamma: %f",file_gamma);
       }
  }

  if (!png_get_valid(ping,ping_info,PNG_INFO_cHRM))
    {
      if (mng_info->have_global_chrm != MagickFalse)
        {
          (void) png_set_cHRM(ping,ping_info,
            mng_info->global_chrm.white_point.x,
            mng_info->global_chrm.white_point.y,
            mng_info->global_chrm.red_primary.x,
            mng_info->global_chrm.red_primary.y,
            mng_info->global_chrm.green_primary.x,
            mng_info->global_chrm.green_primary.y,
            mng_info->global_chrm.blue_primary.x,
            mng_info->global_chrm.blue_primary.y);
        }
    }

  if (png_get_valid(ping,ping_info,PNG_INFO_cHRM))
    {
      (void) png_get_cHRM(ping,ping_info,
        &image->chromaticity.white_point.x,
        &image->chromaticity.white_point.y,
        &image->chromaticity.red_primary.x,
        &image->chromaticity.red_primary.y,
        &image->chromaticity.green_primary.x,
        &image->chromaticity.green_primary.y,
        &image->chromaticity.blue_primary.x,
        &image->chromaticity.blue_primary.y);

       ping_found_cHRM=MagickTrue;

       if (image->chromaticity.red_primary.x>0.6399f &&
           image->chromaticity.red_primary.x<0.6401f &&
           image->chromaticity.red_primary.y>0.3299f &&
           image->chromaticity.red_primary.y<0.3301f &&
           image->chromaticity.green_primary.x>0.2999f &&
           image->chromaticity.green_primary.x<0.3001f &&
           image->chromaticity.green_primary.y>0.5999f &&
           image->chromaticity.green_primary.y<0.6001f &&
           image->chromaticity.blue_primary.x>0.1499f &&
           image->chromaticity.blue_primary.x<0.1501f &&
           image->chromaticity.blue_primary.y>0.0599f &&
           image->chromaticity.blue_primary.y<0.0601f &&
           image->chromaticity.white_point.x>0.3126f &&
           image->chromaticity.white_point.x<0.3128f &&
           image->chromaticity.white_point.y>0.3289f &&
           image->chromaticity.white_point.y<0.3291f)
          ping_found_sRGB_cHRM=MagickTrue;
    }

  if (image->rendering_intent != UndefinedIntent)
    {
      if (ping_found_sRGB != MagickTrue &&
          (ping_found_gAMA != MagickTrue ||
          (image->gamma > .45 && image->gamma < .46)) &&
          (ping_found_cHRM != MagickTrue ||
          ping_found_sRGB_cHRM != MagickFalse) &&
          ping_found_iCCP != MagickTrue)
      {
         png_set_sRGB(ping,ping_info,
            Magick_RenderingIntent_to_PNG_RenderingIntent
            (image->rendering_intent));
         file_gamma=1.000f/2.200f;
         ping_found_sRGB=MagickTrue;
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "    Setting sRGB as if in input");
      }
    }

#if defined(PNG_oFFs_SUPPORTED)
  if (png_get_valid(ping,ping_info,PNG_INFO_oFFs))
    {
      image->page.x=(ssize_t) png_get_x_offset_pixels(ping, ping_info);
      image->page.y=(ssize_t) png_get_y_offset_pixels(ping, ping_info);

      if (logging != MagickFalse)
        if (image->page.x || image->page.y)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Reading PNG oFFs chunk: x: %.20g, y: %.20g.",(double)
            image->page.x,(double) image->page.y);
    }
#endif
#if defined(PNG_pHYs_SUPPORTED)
  if (!png_get_valid(ping,ping_info,PNG_INFO_pHYs))
    {
      if (mng_info->have_global_phys)
        {
          png_set_pHYs(ping,ping_info,
                       mng_info->global_x_pixels_per_unit,
                       mng_info->global_y_pixels_per_unit,
                       mng_info->global_phys_unit_type);
        }
    }

  x_resolution=0;
  y_resolution=0;
  unit_type=0;
  if (png_get_valid(ping,ping_info,PNG_INFO_pHYs))
    {
      /*
        Set image resolution.
      */
      (void) png_get_pHYs(ping,ping_info,&x_resolution,&y_resolution,
        &unit_type);
      image->resolution.x=(double) x_resolution;
      image->resolution.y=(double) y_resolution;

      if (unit_type == PNG_RESOLUTION_METER)
        {
          image->units=PixelsPerCentimeterResolution;
          image->resolution.x=(double) x_resolution/100.0;
          image->resolution.y=(double) y_resolution/100.0;
        }

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Reading PNG pHYs chunk: xres: %.20g, yres: %.20g, units: %d.",
          (double) x_resolution,(double) y_resolution,unit_type);
    }
#endif

  if (png_get_valid(ping,ping_info,PNG_INFO_PLTE))
    {
      png_colorp
        palette;

      (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);

      if ((number_colors == 0) &&
          ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE))
        {
          if (mng_info->global_plte_length)
            {
              png_set_PLTE(ping,ping_info,mng_info->global_plte,
                (int) mng_info->global_plte_length);

              if (!png_get_valid(ping,ping_info,PNG_INFO_tRNS))
              {
                if (mng_info->global_trns_length)
                  {
                    png_warning(ping,
                      "global tRNS has more entries than global PLTE");
                  }
                else
                  {
                     png_set_tRNS(ping,ping_info,mng_info->global_trns,
                       (int) mng_info->global_trns_length,NULL);
                  }
               }
#ifdef PNG_READ_bKGD_SUPPORTED
              if (
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
                   mng_info->have_saved_bkgd_index ||
#endif
                   png_get_valid(ping,ping_info,PNG_INFO_bKGD))
                    {
                      png_color_16
                         background;

#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
                      if (mng_info->have_saved_bkgd_index)
                        background.index=mng_info->saved_bkgd_index;
#endif
                      if (png_get_valid(ping, ping_info, PNG_INFO_bKGD))
                        background.index=ping_background->index;

                      background.red=(png_uint_16)
                        mng_info->global_plte[background.index].red;

                      background.green=(png_uint_16)
                        mng_info->global_plte[background.index].green;

                      background.blue=(png_uint_16)
                        mng_info->global_plte[background.index].blue;

                      background.gray=(png_uint_16)
                        mng_info->global_plte[background.index].green;

                      png_set_bKGD(ping,ping_info,&background);
                    }
#endif
                }
              else
                png_error(ping,"No global PLTE in file");
            }
        }

#ifdef PNG_READ_bKGD_SUPPORTED
  if (mng_info->have_global_bkgd &&
          (!png_get_valid(ping,ping_info,PNG_INFO_bKGD)))
      image->background_color=mng_info->mng_global_bkgd;

  if (png_get_valid(ping,ping_info,PNG_INFO_bKGD))
    {
      unsigned int
        bkgd_scale;

      /* Set image background color.
       * Scale background components to 16-bit, then scale
       * to quantum depth
       */

        bkgd_scale = 1;

        if (ping_file_depth == 1)
           bkgd_scale = 255;

        else if (ping_file_depth == 2)
           bkgd_scale = 85;

        else if (ping_file_depth == 4)
           bkgd_scale = 17;

        if (ping_file_depth <= 8)
           bkgd_scale *= 257;

        ping_background->red *= bkgd_scale;
        ping_background->green *= bkgd_scale;
        ping_background->blue *= bkgd_scale;

        if (logging != MagickFalse)
          {
            if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "    Reading PNG bKGD chunk, raw ping_background=(%d,%d,%d)\n"
                 "    bkgd_scale=%d.  ping_background=(%d,%d,%d)",
                 ping_background->red,ping_background->green,
                 ping_background->blue,
                 bkgd_scale,ping_background->red,
                 ping_background->green,ping_background->blue);
          }

        image->background_color.red=
            ScaleShortToQuantum(ping_background->red);

        image->background_color.green=
            ScaleShortToQuantum(ping_background->green);

        image->background_color.blue=
          ScaleShortToQuantum(ping_background->blue);

        image->background_color.alpha=OpaqueAlpha;

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    image->background_color=(%.20g,%.20g,%.20g).",
            (double) image->background_color.red,
            (double) image->background_color.green,
            (double) image->background_color.blue);
    }
#endif /* PNG_READ_bKGD_SUPPORTED */

  if (png_get_valid(ping,ping_info,PNG_INFO_tRNS))
    {
      /*
        Image has a tRNS chunk.
      */
      int
        max_sample;

      size_t
        one=1;

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Reading PNG tRNS chunk.");

      max_sample = (int) ((one << ping_file_depth) - 1);

      if ((ping_color_type == PNG_COLOR_TYPE_GRAY &&
          (int)ping_trans_color->gray > max_sample) ||
          (ping_color_type == PNG_COLOR_TYPE_RGB &&
          ((int)ping_trans_color->red > max_sample ||
          (int)ping_trans_color->green > max_sample ||
          (int)ping_trans_color->blue > max_sample)))
        {
          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Ignoring PNG tRNS chunk with out-of-range sample.");
          png_free_data(ping, ping_info, PNG_FREE_TRNS, 0);
          png_set_invalid(ping,ping_info,PNG_INFO_tRNS);
          image->alpha_trait=UndefinedPixelTrait;
        }
      else
        {
          int
            scale_to_short;

          scale_to_short = 65535L/((1UL << ping_file_depth)-1);

          /* Scale transparent_color to short */
          transparent_color.red= scale_to_short*ping_trans_color->red;
          transparent_color.green= scale_to_short*ping_trans_color->green;
          transparent_color.blue= scale_to_short*ping_trans_color->blue;
          transparent_color.alpha= scale_to_short*ping_trans_color->gray;

          if (ping_color_type == PNG_COLOR_TYPE_GRAY)
            {
              if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    Raw tRNS graylevel = %d, scaled graylevel = %d.",
                  (int) ping_trans_color->gray,(int) transparent_color.alpha);

              }
              transparent_color.red=transparent_color.alpha;
              transparent_color.green=transparent_color.alpha;
              transparent_color.blue=transparent_color.alpha;
            }
        }
    }
#if defined(PNG_READ_sBIT_SUPPORTED)
  if (mng_info->have_global_sbit)
    {
      if (!png_get_valid(ping,ping_info,PNG_INFO_sBIT))
        png_set_sBIT(ping,ping_info,&mng_info->global_sbit);
    }
#endif
  num_passes=png_set_interlace_handling(ping);

  png_read_update_info(ping,ping_info);

  ping_rowbytes=png_get_rowbytes(ping,ping_info);

  /*
    Initialize image structure.
  */
  mng_info->image_box.left=0;
  mng_info->image_box.right=(ssize_t) ping_width;
  mng_info->image_box.top=0;
  mng_info->image_box.bottom=(ssize_t) ping_height;
  if (mng_info->mng_type == 0)
    {
      mng_info->mng_width=ping_width;
      mng_info->mng_height=ping_height;
      mng_info->frame=mng_info->image_box;
      mng_info->clip=mng_info->image_box;
    }

  else
    {
      image->page.y=mng_info->y_off[mng_info->object_id];
    }

  image->compression=ZipCompression;
  image->columns=ping_width;
  image->rows=ping_height;

  if (((int) ping_color_type == PNG_COLOR_TYPE_GRAY) ||
      ((int) ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
    {
      double
        image_gamma = image->gamma;

      (void)LogMagickEvent(CoderEvent,GetMagickModule(),
         "    image->gamma=%f",(float) image_gamma);

      if (image_gamma > 0.75)
        {
          /* Set image->rendering_intent to Undefined,
           * image->colorspace to GRAY, and reset image->chromaticity.
           */
          image->intensity = Rec709LuminancePixelIntensityMethod;
          SetImageColorspace(image,GRAYColorspace,exception);
        }
      else
        {
          RenderingIntent
            save_rendering_intent = image->rendering_intent;
          ChromaticityInfo
            save_chromaticity = image->chromaticity;

          SetImageColorspace(image,GRAYColorspace,exception);
          image->rendering_intent = save_rendering_intent;
          image->chromaticity = save_chromaticity;
        }

      image->gamma = image_gamma;
    }

  (void)LogMagickEvent(CoderEvent,GetMagickModule(),
      "    image->colorspace=%d",(int) image->colorspace);

  if (((int) ping_color_type == PNG_COLOR_TYPE_PALETTE) ||
      ((int) ping_bit_depth < 16 &&
      (int) ping_color_type == PNG_COLOR_TYPE_GRAY))
    {
      size_t
        one;

      image->storage_class=PseudoClass;
      one=1;
      image->colors=one << ping_file_depth;
#if (MAGICKCORE_QUANTUM_DEPTH == 8)
      if (image->colors > 256)
        image->colors=256;
#else
      if (image->colors > 65536L)
        image->colors=65536L;
#endif
      if ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE)
        {
          png_colorp
            palette;

          (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
          image->colors=(size_t) number_colors;

          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Reading PNG PLTE chunk: number_colors: %d.",number_colors);
        }
    }

  if (image->storage_class == PseudoClass)
    {
      /*
        Initialize image colormap.
      */
      if (AcquireImageColormap(image,image->colors,exception) == MagickFalse)
        png_error(ping,"Memory allocation failed");

      if ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE)
        {
          png_colorp
            palette;

          (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);

          for (i=0; i < (ssize_t) number_colors; i++)
          {
            image->colormap[i].red=ScaleCharToQuantum(palette[i].red);
            image->colormap[i].green=ScaleCharToQuantum(palette[i].green);
            image->colormap[i].blue=ScaleCharToQuantum(palette[i].blue);
          }

          for ( ; i < (ssize_t) image->colors; i++)
          {
            image->colormap[i].red=0;
            image->colormap[i].green=0;
            image->colormap[i].blue=0;
          }
        }

      else
        {
          Quantum
            scale;

          scale = (Quantum) (65535.0/((1UL << ping_file_depth)-1.0));

#if (MAGICKCORE_QUANTUM_DEPTH > 16)
          scale = ScaleShortToQuantum(scale);
#endif

          for (i=0; i < (ssize_t) image->colors; i++)
          {
            image->colormap[i].red=(Quantum) (i*scale);
            image->colormap[i].green=(Quantum) (i*scale);
            image->colormap[i].blue=(Quantum) (i*scale);
          }
       }
    }

   /* Set some properties for reporting by "identify" */
    {
      char
        msg[MagickPathExtent];

     /* encode ping_width, ping_height, ping_file_depth, ping_color_type,
        ping_interlace_method in value */

     (void) FormatLocaleString(msg,MagickPathExtent,
         "%d, %d",(int) ping_width, (int) ping_height);
     (void) SetImageProperty(image,"png:IHDR.width,height",msg,exception);

     (void) FormatLocaleString(msg,MagickPathExtent,"%d",
        (int) ping_file_depth);
     (void) SetImageProperty(image,"png:IHDR.bit_depth",msg,exception);

     (void) FormatLocaleString(msg,MagickPathExtent,"%d (%s)",
         (int) ping_color_type,
         Magick_ColorType_from_PNG_ColorType((int)ping_color_type));
     (void) SetImageProperty(image,"png:IHDR.color_type",msg,exception);

     if (ping_interlace_method == 0)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,"%d (Not interlaced)",
            (int) ping_interlace_method);
       }
     else if (ping_interlace_method == 1)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,"%d (Adam7 method)",
            (int) ping_interlace_method);
       }
     else
       {
         (void) FormatLocaleString(msg,MagickPathExtent,"%d (Unknown method)",
            (int) ping_interlace_method);
       }
       (void) SetImageProperty(image,"png:IHDR.interlace_method",
         msg,exception);

     if (number_colors != 0)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,"%d",
            (int) number_colors);
         (void) SetImageProperty(image,"png:PLTE.number_colors",msg,
            exception);
       }
   }
#if defined(PNG_tIME_SUPPORTED)
   read_tIME_chunk(image,ping,ping_info,exception);
#endif


  /*
    Read image scanlines.
  */
  if (image->delay != 0)
    mng_info->scenes_found++;

  if ((mng_info->mng_type == 0 && (image->ping != MagickFalse)) || (
      (image_info->number_scenes != 0) && (mng_info->scenes_found > (ssize_t)
      (image_info->first_scene+image_info->number_scenes))))
    {
      /* This happens later in non-ping decodes */
      if (png_get_valid(ping,ping_info,PNG_INFO_tRNS))
        image->storage_class=DirectClass;
      image->alpha_trait=
        (((int) ping_color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
         ((int) ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
         (png_get_valid(ping,ping_info,PNG_INFO_tRNS))) ?
        BlendPixelTrait : UndefinedPixelTrait;

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Skipping PNG image data for scene %.20g",(double)
          mng_info->scenes_found-1);
      png_destroy_read_struct(&ping,&ping_info,&end_info);

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
      UnlockSemaphoreInfo(ping_semaphore);
#endif

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  exit ReadOnePNGImage().");

      return(image);
    }

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    Reading PNG IDAT chunk(s)");

  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));

  if (num_passes > 1)
    pixel_info=AcquireVirtualMemory(image->rows,ping_rowbytes*
      sizeof(*ping_pixels));
  else
    pixel_info=AcquireVirtualMemory(ping_rowbytes,sizeof(*ping_pixels));

  if (pixel_info == (MemoryInfo *) NULL)
    png_error(ping,"Memory allocation failed");
  ping_pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    Converting PNG pixels to pixel packets");
  /*
    Convert PNG pixels to pixel packets.
  */
  quantum_info=AcquireQuantumInfo(image_info,image);

  if (quantum_info == (QuantumInfo *) NULL)
     png_error(ping,"Failed to allocate quantum_info");

  (void) SetQuantumEndian(image,quantum_info,MSBEndian);

  {

   MagickBooleanType
     found_transparent_pixel;

  found_transparent_pixel=MagickFalse;

  if (image->storage_class == DirectClass)
    {
      for (pass=0; pass < num_passes; pass++)
      {
        /*
          Convert image to DirectClass pixel packets.
        */
        image->alpha_trait=
            (((int) ping_color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
            ((int) ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
            (png_get_valid(ping,ping_info,PNG_INFO_tRNS))) ?
            BlendPixelTrait : UndefinedPixelTrait;

        for (y=0; y < (ssize_t) image->rows; y++)
        {
          if (num_passes > 1)
            row_offset=ping_rowbytes*y;

          else
            row_offset=0;

          png_read_row(ping,ping_pixels+row_offset,NULL);

          if (pass < num_passes-1)
            continue;

          q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

          if (q == (Quantum *) NULL)
            break;

          if ((int) ping_color_type == PNG_COLOR_TYPE_GRAY)
            (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
              GrayQuantum,ping_pixels+row_offset,exception);

          else if ((int) ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
              GrayAlphaQuantum,ping_pixels+row_offset,exception);

          else if ((int) ping_color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
              RGBAQuantum,ping_pixels+row_offset,exception);

          else if ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE)
            (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
              IndexQuantum,ping_pixels+row_offset,exception);

          else /* ping_color_type == PNG_COLOR_TYPE_RGB */
            (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
              RGBQuantum,ping_pixels+row_offset,exception);

          if (found_transparent_pixel == MagickFalse)
            {
              /* Is there a transparent pixel in the row? */
              if (y== 0 && logging != MagickFalse)
                 (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    Looking for cheap transparent pixel");

              for (x=(ssize_t) image->columns-1; x >= 0; x--)
              {
                if ((ping_color_type == PNG_COLOR_TYPE_RGBA ||
                    ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
                   (GetPixelAlpha(image,q) != OpaqueAlpha))
                  {
                    if (logging != MagickFalse)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "    ...got one.");

                    found_transparent_pixel = MagickTrue;
                    break;
                  }
                if ((ping_color_type == PNG_COLOR_TYPE_RGB ||
                    ping_color_type == PNG_COLOR_TYPE_GRAY) &&
                    (ScaleQuantumToShort(GetPixelRed(image,q)) ==
                    transparent_color.red &&
                    ScaleQuantumToShort(GetPixelGreen(image,q)) ==
                    transparent_color.green &&
                    ScaleQuantumToShort(GetPixelBlue(image,q)) ==
                    transparent_color.blue))
                  {
                    if (logging != MagickFalse)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "    ...got one.");
                    found_transparent_pixel = MagickTrue;
                    break;
                  }
                q+=GetPixelChannels(image);
              }
            }

          if (num_passes == 1)
            {
              status=SetImageProgress(image,LoadImageTag,
                  (MagickOffsetType) y, image->rows);

              if (status == MagickFalse)
                break;
            }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
        }

        if (num_passes != 1)
          {
            status=SetImageProgress(image,LoadImageTag,pass,num_passes);
            if (status == MagickFalse)
              break;
          }
      }
    }

  else /* image->storage_class != DirectClass */

    for (pass=0; pass < num_passes; pass++)
    {
      Quantum
        *quantum_scanline;

      register Quantum
        *r;

      /*
        Convert grayscale image to PseudoClass pixel packets.
      */
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Converting grayscale pixels to pixel packets");

      image->alpha_trait=ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA ?
        BlendPixelTrait : UndefinedPixelTrait;

      quantum_scanline=(Quantum *) AcquireQuantumMemory(image->columns,
        (image->alpha_trait  == BlendPixelTrait?  2 : 1)*
        sizeof(*quantum_scanline));

      if (quantum_scanline == (Quantum *) NULL)
        png_error(ping,"Memory allocation failed");

      for (y=0; y < (ssize_t) image->rows; y++)
      {
        Quantum
           alpha;

        if (num_passes > 1)
          row_offset=ping_rowbytes*y;

        else
          row_offset=0;

        png_read_row(ping,ping_pixels+row_offset,NULL);

        if (pass < num_passes-1)
          continue;

        q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);

        if (q == (Quantum *) NULL)
          break;

        p=ping_pixels+row_offset;
        r=quantum_scanline;

        switch (ping_bit_depth)
        {
          case 8:
          {

            if (ping_color_type == 4)
              for (x=(ssize_t) image->columns-1; x >= 0; x--)
              {
                *r++=*p++;

                alpha=ScaleCharToQuantum((unsigned char)*p++);

                SetPixelAlpha(image,alpha,q);

                if (alpha != OpaqueAlpha)
                  found_transparent_pixel = MagickTrue;

                q+=GetPixelChannels(image);
              }

            else
              for (x=(ssize_t) image->columns-1; x >= 0; x--)
                *r++=*p++;

            break;
          }

          case 16:
          {
            for (x=(ssize_t) image->columns-1; x >= 0; x--)
            {
#if (MAGICKCORE_QUANTUM_DEPTH >= 16)
              unsigned short
                quantum;

              if (image->colors > 256)
                quantum=((*p++) << 8);

              else
                quantum=0;

              quantum|=(*p++);
              *r=ScaleShortToQuantum(quantum);
              r++;

              if (ping_color_type == 4)
                {
                  if (image->colors > 256)
                    quantum=((*p++) << 8);
                  else
                    quantum=0;

                  quantum|=(*p++);

                  alpha=ScaleShortToQuantum(quantum);
                  SetPixelAlpha(image,alpha,q);

                  if (alpha != OpaqueAlpha)
                    found_transparent_pixel = MagickTrue;

                  q+=GetPixelChannels(image);
                }

#else /* MAGICKCORE_QUANTUM_DEPTH == 8 */
              *r++=(*p++);
              p++; /* strip low byte */

              if (ping_color_type == 4)
                {
                  SetPixelAlpha(image,*p++,q);

                  if (GetPixelAlpha(image,q) != OpaqueAlpha)
                    found_transparent_pixel = MagickTrue;

                  p++;
                  q+=GetPixelChannels(image);
                }
#endif
            }

            break;
          }

          default:
            break;
        }

        /*
          Transfer image scanline.
        */
        r=quantum_scanline;

        q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

        if (q == (Quantum *) NULL)
          break;
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          SetPixelIndex(image,*r++,q);
          q+=GetPixelChannels(image);
        }

        if (SyncAuthenticPixels(image,exception) == MagickFalse)
          break;

        if (num_passes == 1)
          {
            status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
              image->rows);

            if (status == MagickFalse)
              break;
          }
      }

      if (num_passes != 1)
        {
          status=SetImageProgress(image,LoadImageTag,pass,num_passes);

          if (status == MagickFalse)
            break;
        }

      quantum_scanline=(Quantum *) RelinquishMagickMemory(quantum_scanline);
    }

    image->alpha_trait=found_transparent_pixel ? BlendPixelTrait :
      UndefinedPixelTrait;

    if (logging != MagickFalse)
      {
        if (found_transparent_pixel != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Found transparent pixel");
        else
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    No transparent pixel was found");

            ping_color_type&=0x03;
          }
      }
    }

  quantum_info=DestroyQuantumInfo(quantum_info);

  if (image->storage_class == PseudoClass)
    {
      PixelTrait
        alpha_trait;

      alpha_trait=image->alpha_trait;
      image->alpha_trait=UndefinedPixelTrait;
      (void) SyncImage(image,exception);
      image->alpha_trait=alpha_trait;
    }

  png_read_end(ping,end_info);

  if (logging != MagickFalse)
  {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "  image->storage_class=%d\n",(int) image->storage_class);
  }

  if (image_info->number_scenes != 0 && mng_info->scenes_found-1 <
      (ssize_t) image_info->first_scene && image->delay != 0)
    {
      png_destroy_read_struct(&ping,&ping_info,&end_info);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      image->colors=2;
      (void) SetImageBackgroundColor(image,exception);
#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
      UnlockSemaphoreInfo(ping_semaphore);
#endif
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  exit ReadOnePNGImage() early.");
      return(image);
    }

  if (png_get_valid(ping,ping_info,PNG_INFO_tRNS))
    {
      ClassType
        storage_class;

      /*
        Image has a transparent background.
      */
      storage_class=image->storage_class;
      image->alpha_trait=BlendPixelTrait;

/* Balfour fix from imagemagick discourse server, 5 Feb 2010 */

      if (storage_class == PseudoClass)
        {
          if ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE)
            {
              for (x=0; x < ping_num_trans; x++)
              {
                 image->colormap[x].alpha_trait=BlendPixelTrait;
                 image->colormap[x].alpha =
                   ScaleCharToQuantum((unsigned char)ping_trans_alpha[x]);
              }
            }

          else if (ping_color_type == PNG_COLOR_TYPE_GRAY)
            {
              for (x=0; x < (int) image->colors; x++)
              {
                 if (ScaleQuantumToShort(image->colormap[x].red) ==
                     transparent_color.alpha)
                 {
                    image->colormap[x].alpha_trait=BlendPixelTrait;
                    image->colormap[x].alpha = (Quantum) TransparentAlpha;
                 }
              }
            }
          (void) SyncImage(image,exception);
        }

#if 1 /* Should have already been done above, but glennrp problem P10
       * needs this.
       */
      else
        {
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            image->storage_class=storage_class;
            q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

            if (q == (Quantum *) NULL)
              break;


            /* Caution: on a Q8 build, this does not distinguish between
             * 16-bit colors that differ only in the low byte
             */
            for (x=(ssize_t) image->columns-1; x >= 0; x--)
            {
              if (ScaleQuantumToShort(GetPixelRed(image,q)) ==
                  transparent_color.red &&
                  ScaleQuantumToShort(GetPixelGreen(image,q)) ==
                  transparent_color.green &&
                  ScaleQuantumToShort(GetPixelBlue(image,q)) ==
                  transparent_color.blue)
                {
                  SetPixelAlpha(image,TransparentAlpha,q);
                }

#if 0 /* I have not found a case where this is needed. */
              else
                {
                  SetPixelAlpha(image,q)=(Quantum) OpaqueAlpha;
                }
#endif

              q+=GetPixelChannels(image);
            }

            if (SyncAuthenticPixels(image,exception) == MagickFalse)
               break;
          }
        }
#endif

      image->storage_class=DirectClass;
    }

  for (j = 0; j < 2; j++)
  {
    if (j == 0)
      status = png_get_text(ping,ping_info,&text,&num_text) != 0 ?
          MagickTrue : MagickFalse;
    else
      status = png_get_text(ping,end_info,&text,&num_text) != 0 ?
          MagickTrue : MagickFalse;

    if (status != MagickFalse)
      for (i=0; i < (ssize_t) num_text; i++)
      {
        /* Check for a profile */

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Reading PNG text chunk");

        if (strlen(text[i].key) > 16 &&
            memcmp(text[i].key, "Raw profile type ",17) == 0)
          {
            const char
              *value;

            value=GetImageOption(image_info,"profile:skip");

            if (IsOptionMember(text[i].key+17,value) == MagickFalse)
            {
               (void) Magick_png_read_raw_profile(ping,image,image_info,text,
                  (int) i,exception);
               num_raw_profiles++;
               if (logging != MagickFalse)
                 (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    Read raw profile %s",text[i].key+17);
            }
            else
            {
               if (logging != MagickFalse)
                 (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    Skipping raw profile %s",text[i].key+17);
            }
          }

        else
          {
            char
              *value;

            length=text[i].text_length;
            value=(char *) AcquireQuantumMemory(length+MagickPathExtent,
              sizeof(*value));
            if (value == (char *) NULL)
              {
                png_error(ping,"Memory allocation failed");
                break;
              }
            *value='\0';
            (void) ConcatenateMagickString(value,text[i].text,length+2);

            /* Don't save "density" or "units" property if we have a pHYs
             * chunk
             */
            if (!png_get_valid(ping,ping_info,PNG_INFO_pHYs) ||
                (LocaleCompare(text[i].key,"density") != 0 &&
                LocaleCompare(text[i].key,"units") != 0))
               (void) SetImageProperty(image,text[i].key,value,exception);

            if (logging != MagickFalse)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "      length: %lu\n"
                "      Keyword: %s",
                (unsigned long) length,
                text[i].key);
            }

            value=DestroyString(value);
          }
      }
    num_text_total += num_text;
  }

#ifdef MNG_OBJECT_BUFFERS
  /*
    Store the object if necessary.
  */
  if (object_id && !mng_info->frozen[object_id])
    {
      if (mng_info->ob[object_id] == (MngBuffer *) NULL)
        {
          /*
            create a new object buffer.
          */
          mng_info->ob[object_id]=(MngBuffer *)
            AcquireMagickMemory(sizeof(MngBuffer));

          if (mng_info->ob[object_id] != (MngBuffer *) NULL)
            {
              mng_info->ob[object_id]->image=(Image *) NULL;
              mng_info->ob[object_id]->reference_count=1;
            }
        }

      if ((mng_info->ob[object_id] == (MngBuffer *) NULL) ||
          mng_info->ob[object_id]->frozen)
        {
          if (mng_info->ob[object_id] == (MngBuffer *) NULL)
             png_error(ping,"Memory allocation failed");

          if (mng_info->ob[object_id]->frozen)
            png_error(ping,"Cannot overwrite frozen MNG object buffer");
        }

      else
        {

          if (mng_info->ob[object_id]->image != (Image *) NULL)
            mng_info->ob[object_id]->image=DestroyImage
                (mng_info->ob[object_id]->image);

          mng_info->ob[object_id]->image=CloneImage(image,0,0,MagickTrue,
            exception);

          if (mng_info->ob[object_id]->image != (Image *) NULL)
            mng_info->ob[object_id]->image->file=(FILE *) NULL;

          else
            png_error(ping, "Cloning image for object buffer failed");

          if (ping_width > 250000L || ping_height > 250000L)
             png_error(ping,"PNG Image dimensions are too large.");

          mng_info->ob[object_id]->width=ping_width;
          mng_info->ob[object_id]->height=ping_height;
          mng_info->ob[object_id]->color_type=ping_color_type;
          mng_info->ob[object_id]->sample_depth=ping_bit_depth;
          mng_info->ob[object_id]->interlace_method=ping_interlace_method;
          mng_info->ob[object_id]->compression_method=
             ping_compression_method;
          mng_info->ob[object_id]->filter_method=ping_filter_method;

          if (png_get_valid(ping,ping_info,PNG_INFO_PLTE))
            {
              png_colorp
                plte;

              /*
                Copy the PLTE to the object buffer.
              */
              png_get_PLTE(ping,ping_info,&plte,&number_colors);
              mng_info->ob[object_id]->plte_length=number_colors;

              for (i=0; i < number_colors; i++)
              {
                mng_info->ob[object_id]->plte[i]=plte[i];
              }
            }

          else
              mng_info->ob[object_id]->plte_length=0;
        }
    }
#endif

   /* Set image->alpha_trait to MagickTrue if the input colortype supports
    * alpha or if a valid tRNS chunk is present, no matter whether there
    * is actual transparency present.
    */
    image->alpha_trait=(((int) ping_color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
        ((int) ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
        (png_get_valid(ping,ping_info,PNG_INFO_tRNS))) ?
        BlendPixelTrait : UndefinedPixelTrait;

#if 0  /* I'm not sure what's wrong here but it does not work. */
    if (image->alpha_trait != UndefinedPixelTrait)
    {
      if (ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        (void) SetImageType(image,GrayscaleAlphaType,exception);

      else if (ping_color_type == PNG_COLOR_TYPE_PALETTE)
        (void) SetImageType(image,PaletteAlphaType,exception);

      else
        (void) SetImageType(image,TrueColorAlphaType,exception);
    }

    else
    {
      if (ping_color_type == PNG_COLOR_TYPE_GRAY)
        (void) SetImageType(image,GrayscaleType,exception);

      else if (ping_color_type == PNG_COLOR_TYPE_PALETTE)
        (void) SetImageType(image,PaletteType,exception);

      else
        (void) SetImageType(image,TrueColorType,exception);
    }
#endif

   /* Set more properties for identify to retrieve */
   {
     char
       msg[MagickPathExtent];

     if (num_text_total != 0)
       {
         /* libpng doesn't tell us whether they were tEXt, zTXt, or iTXt */
         (void) FormatLocaleString(msg,MagickPathExtent,
            "%d tEXt/zTXt/iTXt chunks were found", num_text_total);
         (void) SetImageProperty(image,"png:text",msg,
                exception);
       }

     if (num_raw_profiles != 0)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "%d were found", num_raw_profiles);
         (void) SetImageProperty(image,"png:text-encoded profiles",msg,
                exception);
       }

     /* cHRM chunk: */
     if (ping_found_cHRM != MagickFalse)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,"%s",
            "chunk was found (see Chromaticity, above)");
         (void) SetImageProperty(image,"png:cHRM",msg,
                exception);
       }

     /* bKGD chunk: */
     if (png_get_valid(ping,ping_info,PNG_INFO_bKGD))
       {
         (void) FormatLocaleString(msg,MagickPathExtent,"%s",
            "chunk was found (see Background color, above)");
         (void) SetImageProperty(image,"png:bKGD",msg,
                exception);
       }

     (void) FormatLocaleString(msg,MagickPathExtent,"%s",
        "chunk was found");

#if defined(PNG_iCCP_SUPPORTED)
     /* iCCP chunk: */
     if (ping_found_iCCP != MagickFalse)
        (void) SetImageProperty(image,"png:iCCP",msg,
                exception);
#endif

     if (png_get_valid(ping,ping_info,PNG_INFO_tRNS))
        (void) SetImageProperty(image,"png:tRNS",msg,
                exception);

#if defined(PNG_sRGB_SUPPORTED)
     /* sRGB chunk: */
     if (ping_found_sRGB != MagickFalse)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "intent=%d (%s)",
            (int) intent,
            Magick_RenderingIntentString_from_PNG_RenderingIntent(intent));
         (void) SetImageProperty(image,"png:sRGB",msg,
                 exception);
       }
#endif

     /* gAMA chunk: */
     if (ping_found_gAMA != MagickFalse)
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "gamma=%.8g (See Gamma, above)",
            file_gamma);
         (void) SetImageProperty(image,"png:gAMA",msg,
                exception);
       }

#if defined(PNG_pHYs_SUPPORTED)
     /* pHYs chunk: */
     if (png_get_valid(ping,ping_info,PNG_INFO_pHYs))
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "x_res=%.10g, y_res=%.10g, units=%d",
            (double) x_resolution,(double) y_resolution, unit_type);
         (void) SetImageProperty(image,"png:pHYs",msg,
                exception);
       }
#endif

#if defined(PNG_oFFs_SUPPORTED)
     /* oFFs chunk: */
     if (png_get_valid(ping,ping_info,PNG_INFO_oFFs))
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "x_off=%.20g, y_off=%.20g",
            (double) image->page.x,(double) image->page.y);
         (void) SetImageProperty(image,"png:oFFs",msg,
                exception);
       }
#endif

#if defined(PNG_tIME_SUPPORTED)
     read_tIME_chunk(image,ping,end_info,exception);
#endif

     /* caNv chunk: */
     if ((image->page.width != 0 && image->page.width != image->columns) ||
         (image->page.height != 0 && image->page.height != image->rows) ||
         (image->page.x != 0 || image->page.y != 0))
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "width=%.20g, height=%.20g, x_offset=%.20g, y_offset=%.20g",
            (double) image->page.width,(double) image->page.height,
            (double) image->page.x,(double) image->page.y);
         (void) SetImageProperty(image,"png:caNv",msg,
                exception);
       }

     /* vpAg chunk: */
     if ((image->page.width != 0 && image->page.width != image->columns) ||
         (image->page.height != 0 && image->page.height != image->rows))
       {
         (void) FormatLocaleString(msg,MagickPathExtent,
            "width=%.20g, height=%.20g",
            (double) image->page.width,(double) image->page.height);
         (void) SetImageProperty(image,"png:vpAg",msg,
                exception);
       }
   }

  /*
    Relinquish resources.
  */
  png_destroy_read_struct(&ping,&ping_info,&end_info);

  pixel_info=RelinquishVirtualMemory(pixel_info);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  exit ReadOnePNGImage()");

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
  UnlockSemaphoreInfo(ping_semaphore);
#endif

  /* }  for navigation to beginning of SETJMP-protected block, revert to
   *    Throwing an Exception when an error occurs.
   */

  return(image);

/* end of reading one PNG image */
}