static MagickBooleanType WriteOnePNGImage(MngInfo *mng_info,
  const ImageInfo *IMimage_info,Image *IMimage,ExceptionInfo *exception)
{
  char
    im_vers[32],
    libpng_runv[32],
    libpng_vers[32],
    zlib_runv[32],
    zlib_vers[32];

  Image
    *image;

  ImageInfo
    *image_info;

  char
    s[2];

  const char
    *name,
    *property,
    *value;

  const StringInfo
    *profile;

  int
    num_passes,
    pass,
    ping_wrote_caNv;

  png_byte
     ping_trans_alpha[256];

  png_color
     palette[257];

  png_color_16
    ping_background,
    ping_trans_color;

  png_info
    *ping_info;

  png_struct
    *ping;

  png_uint_32
    ping_height,
    ping_width;

  ssize_t
    y;

  MagickBooleanType
    image_matte,
    logging,
    matte,

    ping_have_blob,
    ping_have_cheap_transparency,
    ping_have_color,
    ping_have_non_bw,
    ping_have_PLTE,
    ping_have_bKGD,
    ping_have_eXIf,
    ping_have_iCCP,
    ping_have_pHYs,
    ping_have_sRGB,
    ping_have_tRNS,

    ping_exclude_bKGD,
    ping_exclude_cHRM,
    ping_exclude_date,
    /* ping_exclude_EXIF, */
    ping_exclude_eXIf,
    ping_exclude_gAMA,
    ping_exclude_iCCP,
    /* ping_exclude_iTXt, */
    ping_exclude_oFFs,
    ping_exclude_pHYs,
    ping_exclude_sRGB,
    ping_exclude_tEXt,
    ping_exclude_tIME,
    /* ping_exclude_tRNS, */
    ping_exclude_caNv,
    ping_exclude_zCCP, /* hex-encoded iCCP */
    ping_exclude_zTXt,

    ping_preserve_colormap,
    ping_preserve_iCCP,
    ping_need_colortype_warning,

    status,
    tried_332,
    tried_333,
    tried_444;

  MemoryInfo
    *volatile pixel_info;

  QuantumInfo
    *quantum_info;

  PNGErrorInfo
    error_info;

  register ssize_t
    i,
    x;

  unsigned char
    *ping_pixels;

  volatile int
    image_colors,
    ping_bit_depth,
    ping_color_type,
    ping_interlace_method,
    ping_compression_method,
    ping_filter_method,
    ping_num_trans;

  volatile size_t
    image_depth,
    old_bit_depth;

  size_t
    quality,
    rowbytes,
    save_image_depth;

  int
    j,
    number_colors,
    number_opaque,
    number_semitransparent,
    number_transparent,
    ping_pHYs_unit_type;

  png_uint_32
    ping_pHYs_x_resolution,
    ping_pHYs_y_resolution;

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
    "  Enter WriteOnePNGImage()");

  image = CloneImage(IMimage,0,0,MagickFalse,exception);
  if (image == (Image *) NULL)
    return(MagickFalse);
  image_info=(ImageInfo *) CloneImageInfo(IMimage_info);

  /* Define these outside of the following "if logging()" block so they will
   * show in debuggers.
   */
  *im_vers='\0';
  (void) ConcatenateMagickString(im_vers,
         MagickLibVersionText,MagickPathExtent);
  (void) ConcatenateMagickString(im_vers,
         MagickLibAddendum,MagickPathExtent);

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

  if (logging != MagickFalse)
    {
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    IM version     = %s", im_vers);
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Libpng version = %s", libpng_vers);
       if (LocaleCompare(libpng_vers,libpng_runv) != 0)
       {
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "      running with   %s", libpng_runv);
       }
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Zlib version   = %s", zlib_vers);
       if (LocaleCompare(zlib_vers,zlib_runv) != 0)
       {
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "      running with   %s", zlib_runv);
       }
    }

  /* Initialize some stuff */
  ping_bit_depth=0,
  ping_color_type=0,
  ping_interlace_method=0,
  ping_compression_method=0,
  ping_filter_method=0,
  ping_num_trans = 0;

  ping_background.red = 0;
  ping_background.green = 0;
  ping_background.blue = 0;
  ping_background.gray = 0;
  ping_background.index = 0;

  ping_trans_color.red=0;
  ping_trans_color.green=0;
  ping_trans_color.blue=0;
  ping_trans_color.gray=0;

  ping_pHYs_unit_type = 0;
  ping_pHYs_x_resolution = 0;
  ping_pHYs_y_resolution = 0;

  ping_have_blob=MagickFalse;
  ping_have_cheap_transparency=MagickFalse;
  ping_have_color=MagickTrue;
  ping_have_non_bw=MagickTrue;
  ping_have_PLTE=MagickFalse;
  ping_have_bKGD=MagickFalse;
  ping_have_eXIf=MagickTrue;
  ping_have_iCCP=MagickFalse;
  ping_have_pHYs=MagickFalse;
  ping_have_sRGB=MagickFalse;
  ping_have_tRNS=MagickFalse;

  ping_exclude_bKGD=mng_info->ping_exclude_bKGD;
  ping_exclude_caNv=mng_info->ping_exclude_caNv;
  ping_exclude_cHRM=mng_info->ping_exclude_cHRM;
  ping_exclude_date=mng_info->ping_exclude_date;
  ping_exclude_eXIf=mng_info->ping_exclude_eXIf;
  ping_exclude_gAMA=mng_info->ping_exclude_gAMA;
  ping_exclude_iCCP=mng_info->ping_exclude_iCCP;
  /* ping_exclude_iTXt=mng_info->ping_exclude_iTXt; */
  ping_exclude_oFFs=mng_info->ping_exclude_oFFs;
  ping_exclude_pHYs=mng_info->ping_exclude_pHYs;
  ping_exclude_sRGB=mng_info->ping_exclude_sRGB;
  ping_exclude_tEXt=mng_info->ping_exclude_tEXt;
  ping_exclude_tIME=mng_info->ping_exclude_tIME;
  /* ping_exclude_tRNS=mng_info->ping_exclude_tRNS; */
  ping_exclude_zCCP=mng_info->ping_exclude_zCCP; /* hex-encoded iCCP in zTXt */
  ping_exclude_zTXt=mng_info->ping_exclude_zTXt;

  ping_preserve_colormap = mng_info->ping_preserve_colormap;
  ping_preserve_iCCP = mng_info->ping_preserve_iCCP;
  ping_need_colortype_warning = MagickFalse;

  /* Recognize the ICC sRGB profile and convert it to the sRGB chunk,
   * i.e., eliminate the ICC profile and set image->rendering_intent.
   * Note that this will not involve any changes to the actual pixels
   * but merely passes information to applications that read the resulting
   * PNG image.
   *
   * To do: recognize other variants of the sRGB profile, using the CRC to
   * verify all recognized variants including the 7 already known.
   *
   * Work around libpng16+ rejecting some "known invalid sRGB profiles".
   *
   * Use something other than image->rendering_intent to record the fact
   * that the sRGB profile was found.
   *
   * Record the ICC version (currently v2 or v4) of the incoming sRGB ICC
   * profile.  Record the Blackpoint Compensation, if any.
   */
   if (ping_exclude_sRGB == MagickFalse && ping_preserve_iCCP == MagickFalse)
   {
      char
        *name;

      const StringInfo
        *profile;

      ResetImageProfileIterator(image);
      for (name=GetNextImageProfile(image); name != (const char *) NULL; )
      {
        profile=GetImageProfile(image,name);

        if (profile != (StringInfo *) NULL)
          {
            if ((LocaleCompare(name,"ICC") == 0) ||
                (LocaleCompare(name,"ICM") == 0))

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
                        ping_exclude_iCCP = MagickTrue;
                        ping_exclude_zCCP = MagickTrue;
                        ping_have_sRGB = MagickTrue;
                        break;
                     }
                   }
                 }
                 if (sRGB_info[icheck].len == 0)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "    Got %lu-byte ICC profile not recognized as sRGB",
                        (unsigned long) length);
              }
          }
        name=GetNextImageProfile(image);
      }
  }

  number_opaque = 0;
  number_semitransparent = 0;
  number_transparent = 0;

  if (logging != MagickFalse)
    {
      if (image->storage_class == UndefinedClass)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    image->storage_class=UndefinedClass");
      if (image->storage_class == DirectClass)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    image->storage_class=DirectClass");
      if (image->storage_class == PseudoClass)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    image->storage_class=PseudoClass");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(), image->taint ?
          "    image->taint=MagickTrue":
          "    image->taint=MagickFalse");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    image->gamma=%g", image->gamma);
    }

  if (image->storage_class == PseudoClass &&
     (mng_info->write_png8 || mng_info->write_png24 || mng_info->write_png32 ||
     mng_info->write_png48 || mng_info->write_png64 ||
     (mng_info->write_png_colortype != 1 &&
     mng_info->write_png_colortype != 5)))
    {
      (void) SyncImage(image,exception);
      image->storage_class = DirectClass;
    }

  if (ping_preserve_colormap == MagickFalse)
    {
      if (image->storage_class != PseudoClass && image->colormap != NULL)
        {
          /* Free the bogus colormap; it can cause trouble later */
           if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Freeing bogus colormap");
           (void) RelinquishMagickMemory(image->colormap);
           image->colormap=NULL;
        }
    }

  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    (void) TransformImageColorspace(image,sRGBColorspace,exception);

  /*
    Sometimes we get PseudoClass images whose RGB values don't match
    the colors in the colormap.  This code syncs the RGB values.
  */
  if (image->depth <= 8 && image->taint && image->storage_class == PseudoClass)
     (void) SyncImage(image,exception);

#if (MAGICKCORE_QUANTUM_DEPTH == 8)
  if (image->depth > 8)
    {
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Reducing PNG bit depth to 8 since this is a Q8 build.");

      image->depth=8;
    }
#endif

  /* Respect the -depth option */
  if (image->depth < 4)
    {
       register Quantum
         *r;

       if (image->depth > 2)
         {
           /* Scale to 4-bit */
           LBR04PacketRGBO(image->background_color);

           for (y=0; y < (ssize_t) image->rows; y++)
           {
             r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

             if (r == (Quantum *) NULL)
               break;

             for (x=0; x < (ssize_t) image->columns; x++)
             {
                LBR04PixelRGBA(r);
                r+=GetPixelChannels(image);
             }

             if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }

           if (image->storage_class == PseudoClass && image->colormap != NULL)
           {
             for (i=0; i < (ssize_t) image->colors; i++)
             {
               LBR04PacketRGBO(image->colormap[i]);
             }
           }
         }
       else if (image->depth > 1)
         {
           /* Scale to 2-bit */
           LBR02PacketRGBO(image->background_color);

           for (y=0; y < (ssize_t) image->rows; y++)
           {
             r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

             if (r == (Quantum *) NULL)
               break;

             for (x=0; x < (ssize_t) image->columns; x++)
             {
                LBR02PixelRGBA(r);
                r+=GetPixelChannels(image);
             }

             if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }

           if (image->storage_class == PseudoClass && image->colormap != NULL)
           {
             for (i=0; i < (ssize_t) image->colors; i++)
             {
               LBR02PacketRGBO(image->colormap[i]);
             }
           }
         }
       else
         {
           /* Scale to 1-bit */
           LBR01PacketRGBO(image->background_color);

           for (y=0; y < (ssize_t) image->rows; y++)
           {
             r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

             if (r == (Quantum *) NULL)
               break;

             for (x=0; x < (ssize_t) image->columns; x++)
             {
                LBR01PixelRGBA(r);
                r+=GetPixelChannels(image);
             }

             if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
           }

           if (image->storage_class == PseudoClass && image->colormap != NULL)
           {
             for (i=0; i < (ssize_t) image->colors; i++)
             {
               LBR01PacketRGBO(image->colormap[i]);
             }
           }
         }
    }

  /* To do: set to next higher multiple of 8 */
  if (image->depth < 8)
     image->depth=8;

#if (MAGICKCORE_QUANTUM_DEPTH > 16)
  /* PNG does not handle depths greater than 16 so reduce it even
   * if lossy
   */
  if (image->depth > 8)
      image->depth=16;
#endif

#if (MAGICKCORE_QUANTUM_DEPTH > 8)
  if (image->depth > 8)
    {
      /* To do: fill low byte properly */
      image->depth=16;
    }

  if (image->depth == 16 && mng_info->write_png_depth != 16)
    if (mng_info->write_png8 ||
        LosslessReduceDepthOK(image,exception) != MagickFalse)
      image->depth = 8;
#endif

  image_colors = (int) image->colors;
  number_opaque = (int) image->colors;
  number_transparent = 0;
  number_semitransparent = 0;

  if (mng_info->write_png_colortype &&
     (mng_info->write_png_colortype > 4 || (mng_info->write_png_depth >= 8 &&
     mng_info->write_png_colortype < 4 &&
     image->alpha_trait == UndefinedPixelTrait)))
  {
     /* Avoid the expensive BUILD_PALETTE operation if we're sure that we
      * are not going to need the result.
      */
     if (mng_info->write_png_colortype == 1 ||
        mng_info->write_png_colortype == 5)
       ping_have_color=MagickFalse;

     if (image->alpha_trait != UndefinedPixelTrait)
       {
         number_transparent = 2;
         number_semitransparent = 1;
       }
  }

  if (mng_info->write_png_colortype < 7)
  {
  /* BUILD_PALETTE
   *
   * Normally we run this just once, but in the case of writing PNG8
   * we reduce the transparency to binary and run again, then if there
   * are still too many colors we reduce to a simple 4-4-4-1, then 3-3-3-1
   * RGBA palette and run again, and then to a simple 3-3-2-1 RGBA
   * palette.  Then (To do) we take care of a final reduction that is only
   * needed if there are still 256 colors present and one of them has both
   * transparent and opaque instances.
   */

  tried_332 = MagickFalse;
  tried_333 = MagickFalse;
  tried_444 = MagickFalse;

  for (j=0; j<6; j++)
  {
    /*
     * Sometimes we get DirectClass images that have 256 colors or fewer.
     * This code will build a colormap.
     *
     * Also, sometimes we get PseudoClass images with an out-of-date
     * colormap.  This code will replace the colormap with a new one.
     * Sometimes we get PseudoClass images that have more than 256 colors.
     * This code will delete the colormap and change the image to
     * DirectClass.
     *
     * If image->alpha_trait is MagickFalse, we ignore the alpha channel
     * even though it sometimes contains left-over non-opaque values.
     *
     * Also we gather some information (number of opaque, transparent,
     * and semitransparent pixels, and whether the image has any non-gray
     * pixels or only black-and-white pixels) that we might need later.
     *
     * Even if the user wants to force GrayAlpha or RGBA (colortype 4 or 6)
     * we need to check for bogus non-opaque values, at least.
     */

   int
     n;

   PixelInfo
     opaque[260],
     semitransparent[260],
     transparent[260];

   register const Quantum
     *s;

   register Quantum
     *q,
     *r;

   if (logging != MagickFalse)
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
         "    Enter BUILD_PALETTE:");

   if (logging != MagickFalse)
     {
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      image->columns=%.20g",(double) image->columns);
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      image->rows=%.20g",(double) image->rows);
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      image->alpha_trait=%.20g",(double) image->alpha_trait);
       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      image->depth=%.20g",(double) image->depth);

       if (image->storage_class == PseudoClass && image->colormap != NULL)
       {
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      Original colormap:");
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "        i    (red,green,blue,alpha)");

         for (i=0; i < 256; i++)
         {
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "        %d    (%d,%d,%d,%d)",
                    (int) i,
                    (int) image->colormap[i].red,
                    (int) image->colormap[i].green,
                    (int) image->colormap[i].blue,
                    (int) image->colormap[i].alpha);
         }

         for (i=image->colors - 10; i < (ssize_t) image->colors; i++)
         {
           if (i > 255)
             {
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "        %d    (%d,%d,%d,%d)",
                    (int) i,
                    (int) image->colormap[i].red,
                    (int) image->colormap[i].green,
                    (int) image->colormap[i].blue,
                    (int) image->colormap[i].alpha);
             }
         }
       }

       (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "      image->colors=%d",(int) image->colors);

       if (image->colors == 0)
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "        (zero means unknown)");

       if (ping_preserve_colormap == MagickFalse)
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "      Regenerate the colormap");
     }

     image_colors=0;
     number_opaque = 0;
     number_semitransparent = 0;
     number_transparent = 0;

     for (y=0; y < (ssize_t) image->rows; y++)
     {
       q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

       if (q == (Quantum *) NULL)
         break;

       for (x=0; x < (ssize_t) image->columns; x++)
       {
           if (image->alpha_trait == UndefinedPixelTrait ||
              GetPixelAlpha(image,q) == OpaqueAlpha)
             {
               if (number_opaque < 259)
                 {
                   if (number_opaque == 0)
                     {
                       GetPixelInfoPixel(image, q, opaque);
                       opaque[0].alpha=OpaqueAlpha;
                       number_opaque=1;
                     }

                   for (i=0; i< (ssize_t) number_opaque; i++)
                     {
                       if (Magick_png_color_equal(image,q,opaque+i))
                         break;
                     }

                   if (i ==  (ssize_t) number_opaque && number_opaque < 259)
                     {
                       number_opaque++;
                       GetPixelInfoPixel(image, q, opaque+i);
                       opaque[i].alpha=OpaqueAlpha;
                     }
                 }
             }
           else if (GetPixelAlpha(image,q) == TransparentAlpha)
             {
               if (number_transparent < 259)
                 {
                   if (number_transparent == 0)
                     {
                       GetPixelInfoPixel(image, q, transparent);
                       ping_trans_color.red=(unsigned short)
                         GetPixelRed(image,q);
                       ping_trans_color.green=(unsigned short)
                         GetPixelGreen(image,q);
                       ping_trans_color.blue=(unsigned short)
                         GetPixelBlue(image,q);
                       ping_trans_color.gray=(unsigned short)
                         GetPixelGray(image,q);
                       number_transparent = 1;
                     }

                   for (i=0; i< (ssize_t) number_transparent; i++)
                     {
                       if (Magick_png_color_equal(image,q,transparent+i))
                         break;
                     }

                   if (i ==  (ssize_t) number_transparent &&
                       number_transparent < 259)
                     {
                       number_transparent++;
                       GetPixelInfoPixel(image,q,transparent+i);
                     }
                 }
             }
           else
             {
               if (number_semitransparent < 259)
                 {
                   if (number_semitransparent == 0)
                     {
                       GetPixelInfoPixel(image,q,semitransparent);
                       number_semitransparent = 1;
                     }

                   for (i=0; i< (ssize_t) number_semitransparent; i++)
                     {
                       if (Magick_png_color_equal(image,q,semitransparent+i)
                           && GetPixelAlpha(image,q) ==
                           semitransparent[i].alpha)
                         break;
                     }

                   if (i ==  (ssize_t) number_semitransparent &&
                       number_semitransparent < 259)
                     {
                       number_semitransparent++;
                       GetPixelInfoPixel(image, q, semitransparent+i);
                     }
                 }
             }
           q+=GetPixelChannels(image);
        }
     }

     if (mng_info->write_png8 == MagickFalse &&
         ping_exclude_bKGD == MagickFalse)
       {
         /* Add the background color to the palette, if it
          * isn't already there.
          */
          if (logging != MagickFalse)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      Check colormap for background (%d,%d,%d)",
                  (int) image->background_color.red,
                  (int) image->background_color.green,
                  (int) image->background_color.blue);
            }
          for (i=0; i<number_opaque; i++)
          {
             if (opaque[i].red == image->background_color.red &&
                 opaque[i].green == image->background_color.green &&
                 opaque[i].blue == image->background_color.blue)
               break;
          }
          if (number_opaque < 259 && i == number_opaque)
            {
               opaque[i] = image->background_color;
               ping_background.index = i;
               number_opaque++;
               if (logging != MagickFalse)
                 {
                   (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "      background_color index is %d",(int) i);
                 }

            }
          else if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      No room in the colormap to add background color");
       }

     image_colors=number_opaque+number_transparent+number_semitransparent;

     if (logging != MagickFalse)
       {
         if (image_colors > 256)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      image has more than 256 colors");

         else
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      image has %d colors",image_colors);
       }

     if (ping_preserve_colormap != MagickFalse)
       break;

     if (mng_info->write_png_colortype != 7) /* We won't need this info */
       {
         ping_have_color=MagickFalse;
         ping_have_non_bw=MagickFalse;

         if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
         {
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "incompatible colorspace");
           ping_have_color=MagickTrue;
           ping_have_non_bw=MagickTrue;
         }

         if(image_colors > 256)
           {
             for (y=0; y < (ssize_t) image->rows; y++)
             {
               q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

               if (q == (Quantum *) NULL)
                 break;

               s=q;
               for (x=0; x < (ssize_t) image->columns; x++)
               {
                 if (GetPixelRed(image,s) != GetPixelGreen(image,s) ||
                     GetPixelRed(image,s) != GetPixelBlue(image,s))
                   {
                      ping_have_color=MagickTrue;
                      ping_have_non_bw=MagickTrue;
                      break;
                   }
                 s+=GetPixelChannels(image);
               }

               if (ping_have_color != MagickFalse)
                 break;

               /* Worst case is black-and-white; we are looking at every
                * pixel twice.
                */

               if (ping_have_non_bw == MagickFalse)
                 {
                   s=q;
                   for (x=0; x < (ssize_t) image->columns; x++)
                   {
                     if (GetPixelRed(image,s) != 0 &&
                         GetPixelRed(image,s) != QuantumRange)
                       {
                         ping_have_non_bw=MagickTrue;
                         break;
                       }
                     s+=GetPixelChannels(image);
                   }
               }
             }
           }
       }

     if (image_colors < 257)
       {
         PixelInfo
           colormap[260];

         /*
          * Initialize image colormap.
          */

         if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      Sort the new colormap");

        /* Sort palette, transparent first */;

         n = 0;

         for (i=0; i<number_transparent; i++)
            colormap[n++] = transparent[i];

         for (i=0; i<number_semitransparent; i++)
            colormap[n++] = semitransparent[i];

         for (i=0; i<number_opaque; i++)
            colormap[n++] = opaque[i];

         ping_background.index +=
           (number_transparent + number_semitransparent);

         /* image_colors < 257; search the colormap instead of the pixels
          * to get ping_have_color and ping_have_non_bw
          */
         for (i=0; i<n; i++)
         {
           if (ping_have_color == MagickFalse)
             {
                if (colormap[i].red != colormap[i].green ||
                    colormap[i].red != colormap[i].blue)
                  {
                     ping_have_color=MagickTrue;
                     ping_have_non_bw=MagickTrue;
                     break;
                  }
              }

           if (ping_have_non_bw == MagickFalse)
             {
               if (colormap[i].red != 0 && colormap[i].red != QuantumRange)
                   ping_have_non_bw=MagickTrue;
             }
          }

        if ((mng_info->ping_exclude_tRNS == MagickFalse ||
            (number_transparent == 0 && number_semitransparent == 0)) &&
            (((mng_info->write_png_colortype-1) ==
            PNG_COLOR_TYPE_PALETTE) ||
            (mng_info->write_png_colortype == 0)))
          {
            if (logging != MagickFalse)
              {
                if (n !=  (ssize_t) image_colors)
                   (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "   image_colors (%d) and n (%d)  don't match",
                   image_colors, n);

                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      AcquireImageColormap");
              }

            image->colors = image_colors;

            if (AcquireImageColormap(image,image_colors,exception) ==
                MagickFalse)
               ThrowWriterException(ResourceLimitError,
                   "MemoryAllocationFailed");

            for (i=0; i< (ssize_t) image_colors; i++)
               image->colormap[i] = colormap[i];

            if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                      "      image->colors=%d (%d)",
                      (int) image->colors, image_colors);

                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                      "      Update the pixel indexes");
              }

            /* Sync the pixel indices with the new colormap */

            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

              if (q == (Quantum *) NULL)
                break;

              for (x=0; x < (ssize_t) image->columns; x++)
              {
                for (i=0; i< (ssize_t) image_colors; i++)
                {
                  if ((image->alpha_trait == UndefinedPixelTrait ||
                      image->colormap[i].alpha == GetPixelAlpha(image,q)) &&
                      image->colormap[i].red == GetPixelRed(image,q) &&
                      image->colormap[i].green == GetPixelGreen(image,q) &&
                      image->colormap[i].blue == GetPixelBlue(image,q))
                  {
                    SetPixelIndex(image,i,q);
                    break;
                  }
                }
                q+=GetPixelChannels(image);
              }

              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                 break;
            }
          }
       }

     if (logging != MagickFalse)
       {
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      image->colors=%d", (int) image->colors);

         if (image->colormap != NULL)
           {
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "       i     (red,green,blue,alpha)");

             for (i=0; i < (ssize_t) image->colors; i++)
             {
               if (i < 300 || i >= (ssize_t) image->colors - 10)
                 {
                   (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "       %d     (%d,%d,%d,%d)",
                        (int) i,
                        (int) image->colormap[i].red,
                        (int) image->colormap[i].green,
                        (int) image->colormap[i].blue,
                        (int) image->colormap[i].alpha);
                 }
             }
           }

           if (number_transparent < 257)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      number_transparent     = %d",
                   number_transparent);
           else

             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      number_transparent     > 256");

           if (number_opaque < 257)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      number_opaque          = %d",
                   number_opaque);

           else
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      number_opaque          > 256");

           if (number_semitransparent < 257)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      number_semitransparent = %d",
                   number_semitransparent);

           else
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      number_semitransparent > 256");

           if (ping_have_non_bw == MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "      All pixels and the background are black or white");

           else if (ping_have_color == MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "      All pixels and the background are gray");

           else
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "      At least one pixel or the background is non-gray");

           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "    Exit BUILD_PALETTE:");
       }

   if (mng_info->write_png8 == MagickFalse)
      break;

   /* Make any reductions necessary for the PNG8 format */
    if (image_colors <= 256 &&
        image_colors != 0 && image->colormap != NULL &&
        number_semitransparent == 0 &&
        number_transparent <= 1)
      break;

    /* PNG8 can't have semitransparent colors so we threshold the
     * opacity to 0 or OpaqueOpacity, and PNG8 can only have one
     * transparent color so if more than one is transparent we merge
     * them into image->background_color.
     */
    if (number_semitransparent != 0 || number_transparent > 1)
      {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Thresholding the alpha channel to binary");

        for (y=0; y < (ssize_t) image->rows; y++)
        {
          r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

          if (r == (Quantum *) NULL)
            break;

          for (x=0; x < (ssize_t) image->columns; x++)
          {
              if (GetPixelAlpha(image,r) < OpaqueAlpha/2)
                {
                  SetPixelViaPixelInfo(image,&image->background_color,r);
                  SetPixelAlpha(image,TransparentAlpha,r);
                }
              else
                  SetPixelAlpha(image,OpaqueAlpha,r);
              r+=GetPixelChannels(image);
          }

          if (SyncAuthenticPixels(image,exception) == MagickFalse)
             break;

          if (image_colors != 0 && image_colors <= 256 &&
             image->colormap != NULL)
            for (i=0; i<image_colors; i++)
                image->colormap[i].alpha =
                    (image->colormap[i].alpha > TransparentAlpha/2 ?
                    TransparentAlpha : OpaqueAlpha);
        }
      continue;
    }

    /* PNG8 can't have more than 256 colors so we quantize the pixels and
     * background color to the 4-4-4-1, 3-3-3-1 or 3-3-2-1 palette.  If the
     * image is mostly gray, the 4-4-4-1 palette is likely to end up with 256
     * colors or less.
     */
    if (tried_444 == MagickFalse && (image_colors == 0 || image_colors > 256))
      {
        if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "    Quantizing the background color to 4-4-4");

        tried_444 = MagickTrue;

        LBR04PacketRGB(image->background_color);

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Quantizing the pixel colors to 4-4-4");

        if (image->colormap == NULL)
        {
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

            if (r == (Quantum *) NULL)
              break;

            for (x=0; x < (ssize_t) image->columns; x++)
            {
              if (GetPixelAlpha(image,r) == OpaqueAlpha)
                  LBR04PixelRGB(r);
              r+=GetPixelChannels(image);
            }

            if (SyncAuthenticPixels(image,exception) == MagickFalse)
               break;
          }
        }

        else /* Should not reach this; colormap already exists and
                must be <= 256 */
        {
          if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Quantizing the colormap to 4-4-4");

          for (i=0; i<image_colors; i++)
          {
            LBR04PacketRGB(image->colormap[i]);
          }
        }
        continue;
      }

    if (tried_333 == MagickFalse && (image_colors == 0 || image_colors > 256))
      {
        if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "    Quantizing the background color to 3-3-3");

        tried_333 = MagickTrue;

        LBR03PacketRGB(image->background_color);

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Quantizing the pixel colors to 3-3-3-1");

        if (image->colormap == NULL)
        {
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

            if (r == (Quantum *) NULL)
              break;

            for (x=0; x < (ssize_t) image->columns; x++)
            {
              if (GetPixelAlpha(image,r) == OpaqueAlpha)
                  LBR03RGB(r);
              r+=GetPixelChannels(image);
            }

            if (SyncAuthenticPixels(image,exception) == MagickFalse)
               break;
          }
        }

        else /* Should not reach this; colormap already exists and
                must be <= 256 */
        {
          if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Quantizing the colormap to 3-3-3-1");
          for (i=0; i<image_colors; i++)
          {
              LBR03PacketRGB(image->colormap[i]);
          }
        }
        continue;
      }

    if (tried_332 == MagickFalse && (image_colors == 0 || image_colors > 256))
      {
        if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "    Quantizing the background color to 3-3-2");

        tried_332 = MagickTrue;

        /* Red and green were already done so we only quantize the blue
         * channel
         */

        LBR02PacketBlue(image->background_color);

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Quantizing the pixel colors to 3-3-2-1");

        if (image->colormap == NULL)
        {
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

            if (r == (Quantum *) NULL)
              break;

            for (x=0; x < (ssize_t) image->columns; x++)
            {
              if (GetPixelAlpha(image,r) == OpaqueAlpha)
                  LBR02PixelBlue(r);
              r+=GetPixelChannels(image);
            }

            if (SyncAuthenticPixels(image,exception) == MagickFalse)
               break;
          }
        }

        else /* Should not reach this; colormap already exists and
                must be <= 256 */
        {
          if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    Quantizing the colormap to 3-3-2-1");
          for (i=0; i<image_colors; i++)
          {
              LBR02PacketBlue(image->colormap[i]);
          }
      }
      continue;
    }

    if (image_colors == 0 || image_colors > 256)
    {
      /* Take care of special case with 256 opaque colors + 1 transparent
       * color.  We don't need to quantize to 2-3-2-1; we only need to
       * eliminate one color, so we'll merge the two darkest red
       * colors (0x49, 0, 0) -> (0x24, 0, 0).
       */
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Merging two dark red background colors to 3-3-2-1");

      if (ScaleQuantumToChar(image->background_color.red) == 0x49 &&
          ScaleQuantumToChar(image->background_color.green) == 0x00 &&
          ScaleQuantumToChar(image->background_color.blue) == 0x00)
      {
         image->background_color.red=ScaleCharToQuantum(0x24);
      }

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Merging two dark red pixel colors to 3-3-2-1");

      if (image->colormap == NULL)
      {
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          r=GetAuthenticPixels(image,0,y,image->columns,1,exception);

          if (r == (Quantum *) NULL)
            break;

          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (ScaleQuantumToChar(GetPixelRed(image,r)) == 0x49 &&
                ScaleQuantumToChar(GetPixelGreen(image,r)) == 0x00 &&
                ScaleQuantumToChar(GetPixelBlue(image,r)) == 0x00 &&
                GetPixelAlpha(image,r) == OpaqueAlpha)
              {
                SetPixelRed(image,ScaleCharToQuantum(0x24),r);
              }
            r+=GetPixelChannels(image);
          }

          if (SyncAuthenticPixels(image,exception) == MagickFalse)
             break;

        }
      }

      else
      {
         for (i=0; i<image_colors; i++)
         {
            if (ScaleQuantumToChar(image->colormap[i].red) == 0x49 &&
                ScaleQuantumToChar(image->colormap[i].green) == 0x00 &&
                ScaleQuantumToChar(image->colormap[i].blue) == 0x00)
            {
               image->colormap[i].red=ScaleCharToQuantum(0x24);
            }
         }
      }
    }
  }
  }
  /* END OF BUILD_PALETTE */

  /* If we are excluding the tRNS chunk and there is transparency,
   * then we must write a Gray-Alpha (color-type 4) or RGBA (color-type 6)
   * PNG.
   */
  if (mng_info->ping_exclude_tRNS != MagickFalse &&
     (number_transparent != 0 || number_semitransparent != 0))
    {
      unsigned int colortype=mng_info->write_png_colortype;

      if (ping_have_color == MagickFalse)
        mng_info->write_png_colortype = 5;

      else
        mng_info->write_png_colortype = 7;

      if (colortype != 0 &&
         mng_info->write_png_colortype != colortype)
        ping_need_colortype_warning=MagickTrue;

    }

  /* See if cheap transparency is possible.  It is only possible
   * when there is a single transparent color, no semitransparent
   * color, and no opaque color that has the same RGB components
   * as the transparent color.  We only need this information if
   * we are writing a PNG with colortype 0 or 2, and we have not
   * excluded the tRNS chunk.
   */
  if (number_transparent == 1 &&
      mng_info->write_png_colortype < 4)
    {
       ping_have_cheap_transparency = MagickTrue;

       if (number_semitransparent != 0)
         ping_have_cheap_transparency = MagickFalse;

       else if (image_colors == 0 || image_colors > 256 ||
           image->colormap == NULL)
         {
           register const Quantum
             *q;

           for (y=0; y < (ssize_t) image->rows; y++)
           {
             q=GetVirtualPixels(image,0,y,image->columns,1, exception);

             if (q == (Quantum *) NULL)
               break;

             for (x=0; x < (ssize_t) image->columns; x++)
             {
                 if (GetPixelAlpha(image,q) != TransparentAlpha &&
                     (unsigned short) GetPixelRed(image,q) ==
                                     ping_trans_color.red &&
                     (unsigned short) GetPixelGreen(image,q) ==
                                     ping_trans_color.green &&
                     (unsigned short) GetPixelBlue(image,q) ==
                                     ping_trans_color.blue)
                   {
                     ping_have_cheap_transparency = MagickFalse;
                     break;
                   }

                 q+=GetPixelChannels(image);
             }

             if (ping_have_cheap_transparency == MagickFalse)
                break;
           }
         }
       else
         {
            /* Assuming that image->colormap[0] is the one transparent color
             * and that all others are opaque.
             */
            if (image_colors > 1)
              for (i=1; i<image_colors; i++)
                if (image->colormap[i].red == image->colormap[0].red &&
                    image->colormap[i].green == image->colormap[0].green &&
                    image->colormap[i].blue == image->colormap[0].blue)
                  {
                     ping_have_cheap_transparency = MagickFalse;
                     break;
                  }
         }

       if (logging != MagickFalse)
         {
           if (ping_have_cheap_transparency == MagickFalse)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "   Cheap transparency is not possible.");

           else
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "   Cheap transparency is possible.");
         }
     }
  else
    ping_have_cheap_transparency = MagickFalse;

  image_depth=image->depth;

  quantum_info = (QuantumInfo *) NULL;
  number_colors=0;
  image_colors=(int) image->colors;
  image_matte=image->alpha_trait !=
        UndefinedPixelTrait ? MagickTrue : MagickFalse;

  if (mng_info->write_png_colortype < 5)
    mng_info->IsPalette=image->storage_class == PseudoClass &&
      image_colors <= 256 && image->colormap != NULL;
  else
    mng_info->IsPalette = MagickFalse;

  if ((mng_info->write_png_colortype == 4 || mng_info->write_png8) &&
     (image->colors == 0 || image->colormap == NULL))
    {
      image_info=DestroyImageInfo(image_info);
      image=DestroyImage(image);
      (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
          "Cannot write PNG8 or color-type 3; colormap is NULL",
          "`%s'",IMimage->filename);
      return(MagickFalse);
    }

  /*
    Allocate the PNG structures
  */
#ifdef PNG_USER_MEM_SUPPORTED
 error_info.image=image;
 error_info.exception=exception;
  ping=png_create_write_struct_2(PNG_LIBPNG_VER_STRING,&error_info,
    MagickPNGErrorHandler,MagickPNGWarningHandler,(void *) NULL,
    (png_malloc_ptr) Magick_png_malloc,(png_free_ptr) Magick_png_free);

#else
  ping=png_create_write_struct(PNG_LIBPNG_VER_STRING,&error_info,
    MagickPNGErrorHandler,MagickPNGWarningHandler);

#endif
  if (ping == (png_struct *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

  ping_info=png_create_info_struct(ping);

  if (ping_info == (png_info *) NULL)
    {
      png_destroy_write_struct(&ping,(png_info **) NULL);
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    }

  png_set_write_fn(ping,image,png_put_data,png_flush_data);
  pixel_info=(MemoryInfo *) NULL;

  if (setjmp(png_jmpbuf(ping)))
    {
      /*
        PNG write failed.
      */
#ifdef PNG_DEBUG
     if (image_info->verbose)
        (void) printf("PNG write has failed.\n");
#endif
      png_destroy_write_struct(&ping,&ping_info);
#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
      UnlockSemaphoreInfo(ping_semaphore);
#endif

      if (pixel_info != (MemoryInfo *) NULL)
        pixel_info=RelinquishVirtualMemory(pixel_info);

      if (quantum_info != (QuantumInfo *) NULL)
        quantum_info=DestroyQuantumInfo(quantum_info);

      if (ping_have_blob != MagickFalse)
          (void) CloseBlob(image);
      image_info=DestroyImageInfo(image_info);
      image=DestroyImage(image);
      return(MagickFalse);
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
    Prepare PNG for writing.
  */

#if defined(PNG_MNG_FEATURES_SUPPORTED)
  if (mng_info->write_mng)
  {
     (void) png_permit_mng_features(ping,PNG_ALL_MNG_FEATURES);
# ifdef PNG_WRITE_CHECK_FOR_INVALID_INDEX_SUPPORTED
     /* Disable new libpng-1.5.10 feature when writing a MNG because
      * zero-length PLTE is OK
      */
     png_set_check_for_invalid_index (ping, 0);
# endif
  }

#else
# ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
  if (mng_info->write_mng)
     png_permit_empty_plte(ping,MagickTrue);

# endif
#endif

  x=0;

  ping_width=(png_uint_32) image->columns;
  ping_height=(png_uint_32) image->rows;

  if (mng_info->write_png8 || mng_info->write_png24 || mng_info->write_png32)
     image_depth=8;

  if (mng_info->write_png48 || mng_info->write_png64)
     image_depth=16;

  if (mng_info->write_png_depth != 0)
     image_depth=mng_info->write_png_depth;

  /* Adjust requested depth to next higher valid depth if necessary */
  if (image_depth > 8)
     image_depth=16;

  if ((image_depth > 4) && (image_depth < 8))
     image_depth=8;

  if (image_depth == 3)
     image_depth=4;

  if (logging != MagickFalse)
    {
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    width=%.20g",(double) ping_width);
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    height=%.20g",(double) ping_height);
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    image_matte=%.20g",(double) image->alpha_trait);
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    image->depth=%.20g",(double) image->depth);
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    Tentative ping_bit_depth=%.20g",(double) image_depth);
    }

  save_image_depth=image_depth;
  ping_bit_depth=(png_byte) save_image_depth;


#if defined(PNG_pHYs_SUPPORTED)
  if (ping_exclude_pHYs == MagickFalse)
  {
  if ((image->resolution.x != 0) && (image->resolution.y != 0) &&
      (!mng_info->write_mng || !mng_info->equal_physs))
    {
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Setting up pHYs chunk");

      if (image->units == PixelsPerInchResolution)
        {
          ping_pHYs_unit_type=PNG_RESOLUTION_METER;
          ping_pHYs_x_resolution=
             (png_uint_32) ((100.0*image->resolution.x+0.5)/2.54);
          ping_pHYs_y_resolution=
             (png_uint_32) ((100.0*image->resolution.y+0.5)/2.54);
        }

      else if (image->units == PixelsPerCentimeterResolution)
        {
          ping_pHYs_unit_type=PNG_RESOLUTION_METER;
          ping_pHYs_x_resolution=(png_uint_32) (100.0*image->resolution.x+0.5);
          ping_pHYs_y_resolution=(png_uint_32) (100.0*image->resolution.y+0.5);
        }

      else
        {
          ping_pHYs_unit_type=PNG_RESOLUTION_UNKNOWN;
          ping_pHYs_x_resolution=(png_uint_32) image->resolution.x;
          ping_pHYs_y_resolution=(png_uint_32) image->resolution.y;
        }

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Set up PNG pHYs chunk: xres: %.20g, yres: %.20g, units: %d.",
          (double) ping_pHYs_x_resolution,(double) ping_pHYs_y_resolution,
          (int) ping_pHYs_unit_type);
       ping_have_pHYs = MagickTrue;
    }
  }
#endif

  if (ping_exclude_bKGD == MagickFalse)
  {
  if ((!mng_info->adjoin || !mng_info->equal_backgrounds))
    {
       unsigned int
         mask;

       mask=0xffff;
       if (ping_bit_depth == 8)
          mask=0x00ff;

       if (ping_bit_depth == 4)
          mask=0x000f;

       if (ping_bit_depth == 2)
          mask=0x0003;

       if (ping_bit_depth == 1)
          mask=0x0001;

       ping_background.red=(png_uint_16)
         (ScaleQuantumToShort(image->background_color.red) & mask);

       ping_background.green=(png_uint_16)
         (ScaleQuantumToShort(image->background_color.green) & mask);

       ping_background.blue=(png_uint_16)
         (ScaleQuantumToShort(image->background_color.blue) & mask);

       ping_background.gray=(png_uint_16) ping_background.green;
    }

  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Setting up bKGD chunk (1)");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "      background_color index is %d",
          (int) ping_background.index);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    ping_bit_depth=%d",ping_bit_depth);
    }

  ping_have_bKGD = MagickTrue;
  }

  /*
    Select the color type.
  */
  matte=image_matte;
  old_bit_depth=0;

  if (mng_info->IsPalette && mng_info->write_png8)
    {
      /* To do: make this a function cause it's used twice, except
         for reducing the sample depth from 8. */

      number_colors=image_colors;

      ping_have_tRNS=MagickFalse;

      /*
        Set image palette.
      */
      ping_color_type=(png_byte) PNG_COLOR_TYPE_PALETTE;

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  Setting up PLTE chunk with %d colors (%d)",
            number_colors, image_colors);

      for (i=0; i < (ssize_t) number_colors; i++)
      {
        palette[i].red=ScaleQuantumToChar(image->colormap[i].red);
        palette[i].green=ScaleQuantumToChar(image->colormap[i].green);
        palette[i].blue=ScaleQuantumToChar(image->colormap[i].blue);
        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
#if MAGICKCORE_QUANTUM_DEPTH == 8
            "    %3ld (%3d,%3d,%3d)",
#else
            "    %5ld (%5d,%5d,%5d)",
#endif
            (long) i,palette[i].red,palette[i].green,palette[i].blue);

      }

      ping_have_PLTE=MagickTrue;
      image_depth=ping_bit_depth;
      ping_num_trans=0;

      if (matte != MagickFalse)
      {
          /*
            Identify which colormap entry is transparent.
          */
          assert(number_colors <= 256);
          assert(image->colormap != NULL);

          for (i=0; i < (ssize_t) number_transparent; i++)
             ping_trans_alpha[i]=0;


          ping_num_trans=(unsigned short) (number_transparent +
             number_semitransparent);

          if (ping_num_trans == 0)
             ping_have_tRNS=MagickFalse;

          else
             ping_have_tRNS=MagickTrue;
      }

      if (ping_exclude_bKGD == MagickFalse)
      {
       /*
        * Identify which colormap entry is the background color.
        */

        for (i=0; i < (ssize_t) MagickMax(1L*number_colors-1L,1L); i++)
          if (IsPNGColorEqual(ping_background,image->colormap[i]))
            break;

        ping_background.index=(png_byte) i;

        if (logging != MagickFalse)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "      background_color index is %d",
                 (int) ping_background.index);
          }
      }
    } /* end of write_png8 */

  else if (mng_info->write_png_colortype == 1)
    {
      image_matte=MagickFalse;
      ping_color_type=(png_byte) PNG_COLOR_TYPE_GRAY;
    }

  else if (mng_info->write_png24 || mng_info->write_png48 ||
      mng_info->write_png_colortype == 3)
    {
      image_matte=MagickFalse;
      ping_color_type=(png_byte) PNG_COLOR_TYPE_RGB;
    }

  else if (mng_info->write_png32 || mng_info->write_png64 ||
      mng_info->write_png_colortype == 7)
    {
      image_matte=MagickTrue;
      ping_color_type=(png_byte) PNG_COLOR_TYPE_RGB_ALPHA;
    }

  else /* mng_info->write_pngNN not specified */
    {
      image_depth=ping_bit_depth;

      if (mng_info->write_png_colortype != 0)
        {
          ping_color_type=(png_byte) mng_info->write_png_colortype-1;

          if (ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
              ping_color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            image_matte=MagickTrue;

          else
            image_matte=MagickFalse;

          if (logging != MagickFalse)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "   PNG colortype %d was specified:",(int) ping_color_type);
        }

      else /* write_png_colortype not specified */
        {
          if (logging != MagickFalse)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "  Selecting PNG colortype:");

          ping_color_type=(png_byte) ((matte != MagickFalse)?
            PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB);

          if (image_info->type == TrueColorType)
            {
              ping_color_type=(png_byte) PNG_COLOR_TYPE_RGB;
              image_matte=MagickFalse;
            }

          if (image_info->type == TrueColorAlphaType)
            {
              ping_color_type=(png_byte) PNG_COLOR_TYPE_RGB_ALPHA;
              image_matte=MagickTrue;
            }

          if (image_info->type == PaletteType ||
              image_info->type == PaletteAlphaType)
            ping_color_type=(png_byte) PNG_COLOR_TYPE_PALETTE;

          if (mng_info->write_png_colortype == 0 &&
             image_info->type == UndefinedType)
            {
              if (ping_have_color == MagickFalse)
                {
                  if (image_matte == MagickFalse)
                    {
                      ping_color_type=(png_byte) PNG_COLOR_TYPE_GRAY;
                      image_matte=MagickFalse;
                    }

                  else
                    {
                      ping_color_type=(png_byte) PNG_COLOR_TYPE_GRAY_ALPHA;
                      image_matte=MagickTrue;
                    }
                }
              else
                {
                  if (image_matte == MagickFalse)
                    {
                      ping_color_type=(png_byte) PNG_COLOR_TYPE_RGB;
                      image_matte=MagickFalse;
                    }

                  else
                    {
                      ping_color_type=(png_byte) PNG_COLOR_TYPE_RGBA;
                      image_matte=MagickTrue;
                    }
                 }
            }

        }

      if (logging != MagickFalse)
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
         "    Selected PNG colortype=%d",ping_color_type);

      if (ping_bit_depth < 8)
        {
          if (ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
              ping_color_type == PNG_COLOR_TYPE_RGB ||
              ping_color_type == PNG_COLOR_TYPE_RGB_ALPHA)
            ping_bit_depth=8;
        }

      old_bit_depth=ping_bit_depth;

      if (ping_color_type == PNG_COLOR_TYPE_GRAY)
        {
          if (image->alpha_trait == UndefinedPixelTrait &&
               ping_have_non_bw == MagickFalse)
             ping_bit_depth=1;
        }

      if (ping_color_type == PNG_COLOR_TYPE_PALETTE)
        {
           size_t one = 1;
           ping_bit_depth=1;

           if (image->colors == 0)
           {
              /* DO SOMETHING */
                png_error(ping,"image has 0 colors");
           }

           while ((int) (one << ping_bit_depth) < (ssize_t) image_colors)
             ping_bit_depth <<= 1;
        }

      if (logging != MagickFalse)
         {
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Number of colors: %.20g",(double) image_colors);

           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Tentative PNG bit depth: %d",ping_bit_depth);
         }

      if (ping_bit_depth < (int) mng_info->write_png_depth)
         ping_bit_depth = mng_info->write_png_depth;
    }

  image_depth=ping_bit_depth;

  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    Tentative PNG color type: %s (%.20g)",
        PngColorTypeToString(ping_color_type),
        (double) ping_color_type);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    image_info->type: %.20g",(double) image_info->type);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    image_depth: %.20g",(double) image_depth);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),

        "    image->depth: %.20g",(double) image->depth);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    ping_bit_depth: %.20g",(double) ping_bit_depth);
    }

  if (matte != MagickFalse)
    {
      if (mng_info->IsPalette)
        {
          if (mng_info->write_png_colortype == 0)
            {
              ping_color_type=PNG_COLOR_TYPE_GRAY_ALPHA;

              if (ping_have_color != MagickFalse)
                 ping_color_type=PNG_COLOR_TYPE_RGBA;
            }

          /*
           * Determine if there is any transparent color.
          */
          if (number_transparent + number_semitransparent == 0)
            {
              /*
                No transparent pixels are present.  Change 4 or 6 to 0 or 2.
              */

              image_matte=MagickFalse;

              if (mng_info->write_png_colortype == 0)
                ping_color_type&=0x03;
            }

          else
            {
              unsigned int
                mask;

              mask=0xffff;

              if (ping_bit_depth == 8)
                 mask=0x00ff;

              if (ping_bit_depth == 4)
                 mask=0x000f;

              if (ping_bit_depth == 2)
                 mask=0x0003;

              if (ping_bit_depth == 1)
                 mask=0x0001;

              ping_trans_color.red=(png_uint_16)
                (ScaleQuantumToShort(image->colormap[0].red) & mask);

              ping_trans_color.green=(png_uint_16)
                (ScaleQuantumToShort(image->colormap[0].green) & mask);

              ping_trans_color.blue=(png_uint_16)
                (ScaleQuantumToShort(image->colormap[0].blue) & mask);

              ping_trans_color.gray=(png_uint_16)
                (ScaleQuantumToShort(GetPixelInfoIntensity(image,
                   image->colormap)) & mask);

              ping_trans_color.index=(png_byte) 0;

              ping_have_tRNS=MagickTrue;
            }

          if (ping_have_tRNS != MagickFalse)
            {
              /*
               * Determine if there is one and only one transparent color
               * and if so if it is fully transparent.
               */
              if (ping_have_cheap_transparency == MagickFalse)
                ping_have_tRNS=MagickFalse;
            }

          if (ping_have_tRNS != MagickFalse)
            {
              if (mng_info->write_png_colortype == 0)
                ping_color_type &= 0x03;  /* changes 4 or 6 to 0 or 2 */

              if (image_depth == 8)
                {
                  ping_trans_color.red&=0xff;
                  ping_trans_color.green&=0xff;
                  ping_trans_color.blue&=0xff;
                  ping_trans_color.gray&=0xff;
                }
            }
        }
      else
        {
          if (image_depth == 8)
            {
              ping_trans_color.red&=0xff;
              ping_trans_color.green&=0xff;
              ping_trans_color.blue&=0xff;
              ping_trans_color.gray&=0xff;
            }
        }
    }

    matte=image_matte;

    if (ping_have_tRNS != MagickFalse)
      image_matte=MagickFalse;

    if ((mng_info->IsPalette) &&
        mng_info->write_png_colortype-1 != PNG_COLOR_TYPE_PALETTE &&
        ping_have_color == MagickFalse &&
        (image_matte == MagickFalse || image_depth >= 8))
      {
        size_t one=1;

        if (image_matte != MagickFalse)
          ping_color_type=PNG_COLOR_TYPE_GRAY_ALPHA;

        else if (mng_info->write_png_colortype-1 != PNG_COLOR_TYPE_GRAY_ALPHA)
          {
            ping_color_type=PNG_COLOR_TYPE_GRAY;

            if (save_image_depth == 16 && image_depth == 8)
              {
                if (logging != MagickFalse)
                  {
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  Scaling ping_trans_color (0)");
                  }
                    ping_trans_color.gray*=0x0101;
              }
          }

        if (image_depth > MAGICKCORE_QUANTUM_DEPTH)
          image_depth=MAGICKCORE_QUANTUM_DEPTH;

        if ((image_colors == 0) ||
             ((ssize_t) (image_colors-1) > (ssize_t) MaxColormapSize))
          image_colors=(int) (one << image_depth);

        if (image_depth > 8)
          ping_bit_depth=16;

        else
          {
            ping_bit_depth=8;
            if ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE)
              {
                if(!mng_info->write_png_depth)
                  {
                    ping_bit_depth=1;

                    while ((int) (one << ping_bit_depth)
                        < (ssize_t) image_colors)
                      ping_bit_depth <<= 1;
                  }
              }

            else if (ping_color_type ==
                PNG_COLOR_TYPE_GRAY && image_colors < 17 &&
                mng_info->IsPalette)
              {
              /* Check if grayscale is reducible */

                int
                  depth_4_ok=MagickTrue,
                  depth_2_ok=MagickTrue,
                  depth_1_ok=MagickTrue;

                for (i=0; i < (ssize_t) image_colors; i++)
                {
                   unsigned char
                     intensity;

                   intensity=ScaleQuantumToChar(image->colormap[i].red);

                   if ((intensity & 0x0f) != ((intensity & 0xf0) >> 4))
                     depth_4_ok=depth_2_ok=depth_1_ok=MagickFalse;
                   else if ((intensity & 0x03) != ((intensity & 0x0c) >> 2))
                     depth_2_ok=depth_1_ok=MagickFalse;
                   else if ((intensity & 0x01) != ((intensity & 0x02) >> 1))
                     depth_1_ok=MagickFalse;
                }

                if (depth_1_ok && mng_info->write_png_depth <= 1)
                  ping_bit_depth=1;

                else if (depth_2_ok && mng_info->write_png_depth <= 2)
                  ping_bit_depth=2;

                else if (depth_4_ok && mng_info->write_png_depth <= 4)
                  ping_bit_depth=4;
              }
          }

          image_depth=ping_bit_depth;
      }

    else

      if (mng_info->IsPalette)
      {
        number_colors=image_colors;

        if (image_depth <= 8)
          {
            /*
              Set image palette.
            */
            ping_color_type=(png_byte) PNG_COLOR_TYPE_PALETTE;

            if (!(mng_info->have_write_global_plte && matte == MagickFalse))
              {
                for (i=0; i < (ssize_t) number_colors; i++)
                {
                  palette[i].red=ScaleQuantumToChar(image->colormap[i].red);
                  palette[i].green=
                    ScaleQuantumToChar(image->colormap[i].green);
                  palette[i].blue=ScaleQuantumToChar(image->colormap[i].blue);
                }

                if (logging != MagickFalse)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "  Setting up PLTE chunk with %d colors",
                    number_colors);

                ping_have_PLTE=MagickTrue;
              }

            /* color_type is PNG_COLOR_TYPE_PALETTE */
            if (mng_info->write_png_depth == 0)
              {
                size_t
                  one;

                ping_bit_depth=1;
                one=1;

                while ((one << ping_bit_depth) < (size_t) number_colors)
                  ping_bit_depth <<= 1;
              }

            ping_num_trans=0;

            if (matte != MagickFalse)
              {
                /*
                 * Set up trans_colors array.
                 */
                assert(number_colors <= 256);

                ping_num_trans=(unsigned short) (number_transparent +
                  number_semitransparent);

                if (ping_num_trans == 0)
                  ping_have_tRNS=MagickFalse;

                else
                  {
                    if (logging != MagickFalse)
                      {
                        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Scaling ping_trans_color (1)");
                      }
                    ping_have_tRNS=MagickTrue;

                    for (i=0; i < ping_num_trans; i++)
                    {
                       ping_trans_alpha[i]= (png_byte)
                         ScaleQuantumToChar(image->colormap[i].alpha);
                    }
                  }
              }
          }
      }

    else
      {

        if (image_depth < 8)
          image_depth=8;

        if ((save_image_depth == 16) && (image_depth == 8))
          {
            if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    Scaling ping_trans_color from (%d,%d,%d)",
                  (int) ping_trans_color.red,
                  (int) ping_trans_color.green,
                  (int) ping_trans_color.blue);
              }

            ping_trans_color.red*=0x0101;
            ping_trans_color.green*=0x0101;
            ping_trans_color.blue*=0x0101;
            ping_trans_color.gray*=0x0101;

            if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    to (%d,%d,%d)",
                  (int) ping_trans_color.red,
                  (int) ping_trans_color.green,
                  (int) ping_trans_color.blue);
              }
          }
      }

    if (ping_bit_depth <  (ssize_t) mng_info->write_png_depth)
         ping_bit_depth =  (ssize_t) mng_info->write_png_depth;

    /*
      Adjust background and transparency samples in sub-8-bit grayscale files.
    */
    if (ping_bit_depth < 8 && ping_color_type ==
        PNG_COLOR_TYPE_GRAY)
      {
         png_uint_16
           maxval;

         size_t
           one=1;

         maxval=(png_uint_16) ((one << ping_bit_depth)-1);

         if (ping_exclude_bKGD == MagickFalse)
         {

         ping_background.gray=(png_uint_16) ((maxval/65535.)*
           (ScaleQuantumToShort(((GetPixelInfoIntensity(image,
           &image->background_color))) +.5)));

         if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "  Setting up bKGD chunk (2)");
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      background_color index is %d",
             (int) ping_background.index);

         ping_have_bKGD = MagickTrue;
         }

         if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "  Scaling ping_trans_color.gray from %d",
             (int)ping_trans_color.gray);

         ping_trans_color.gray=(png_uint_16) ((maxval/255.)*(
           ping_trans_color.gray)+.5);

         if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      to %d", (int)ping_trans_color.gray);
      }

  if (ping_exclude_bKGD == MagickFalse)
  {
    if (mng_info->IsPalette && (int) ping_color_type == PNG_COLOR_TYPE_PALETTE)
      {
        /*
           Identify which colormap entry is the background color.
        */

        number_colors=image_colors;

        for (i=0; i < (ssize_t) MagickMax(1L*number_colors,1L); i++)
          if (IsPNGColorEqual(image->background_color,image->colormap[i]))
            break;

        ping_background.index=(png_byte) i;

        if (logging != MagickFalse)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Setting up bKGD chunk with index=%d",(int) i);
          }

        if (i < (ssize_t) number_colors)
          {
            ping_have_bKGD = MagickTrue;

            if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "     background   =(%d,%d,%d)",
                        (int) ping_background.red,
                        (int) ping_background.green,
                        (int) ping_background.blue);
              }
          }

        else  /* Can't happen */
          {
            if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      No room in PLTE to add bKGD color");
            ping_have_bKGD = MagickFalse;
          }
      }
  }

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    PNG color type: %s (%d)", PngColorTypeToString(ping_color_type),
      ping_color_type);
  /*
    Initialize compression level and filtering.
  */
  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Setting up deflate compression");

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    Compression buffer size: 32768");
    }

  png_set_compression_buffer_size(ping,32768L);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    Compression mem level: 9");

  png_set_compression_mem_level(ping, 9);

  /* Untangle the "-quality" setting:

     Undefined is 0; the default is used.
     Default is 75

     10's digit:

        0 or omitted: Use Z_HUFFMAN_ONLY strategy with the
           zlib default compression level

        1-9: the zlib compression level

     1's digit:

        0-4: the PNG filter method

        5:   libpng adaptive filtering if compression level > 5
             libpng filter type "none" if compression level <= 5
                or if image is grayscale or palette

        6:   libpng adaptive filtering

        7:   "LOCO" filtering (intrapixel differing) if writing
             a MNG, otherwise "none".  Did not work in IM-6.7.0-9
             and earlier because of a missing "else".

        8:   Z_RLE strategy (or Z_HUFFMAN_ONLY if quality < 10), adaptive
             filtering. Unused prior to IM-6.7.0-10, was same as 6

        9:   Z_RLE strategy (or Z_HUFFMAN_ONLY if quality < 10), no PNG filters
             Unused prior to IM-6.7.0-10, was same as 6

    Note that using the -quality option, not all combinations of
    PNG filter type, zlib compression level, and zlib compression
    strategy are possible.  This will be addressed soon in a
    release that accomodates "-define png:compression-strategy", etc.

   */

  quality=image_info->quality == UndefinedCompressionQuality ? 75UL :
     image_info->quality;

  if (quality <= 9)
    {
      if (mng_info->write_png_compression_strategy == 0)
        mng_info->write_png_compression_strategy = Z_HUFFMAN_ONLY+1;
    }

  else if (mng_info->write_png_compression_level == 0)
    {
      int
        level;

      level=(int) MagickMin((ssize_t) quality/10,9);

      mng_info->write_png_compression_level = level+1;
    }

  if (mng_info->write_png_compression_strategy == 0)
    {
        if ((quality %10) == 8 || (quality %10) == 9)
#ifdef Z_RLE  /* Z_RLE was added to zlib-1.2.0 */
          mng_info->write_png_compression_strategy=Z_RLE+1;
#else
          mng_info->write_png_compression_strategy = Z_DEFAULT_STRATEGY+1;
#endif
    }

  if (mng_info->write_png_compression_filter == 0)
        mng_info->write_png_compression_filter=((int) quality % 10) + 1;

  if (logging != MagickFalse)
    {
        if (mng_info->write_png_compression_level)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Compression level:    %d",
            (int) mng_info->write_png_compression_level-1);

        if (mng_info->write_png_compression_strategy)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Compression strategy: %d",
            (int) mng_info->write_png_compression_strategy-1);

        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  Setting up filtering");

        if (mng_info->write_png_compression_filter == 6)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Base filter method: ADAPTIVE");
        else if (mng_info->write_png_compression_filter == 0 ||
                 mng_info->write_png_compression_filter == 1)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Base filter method: NONE");
        else
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Base filter method: %d",
            (int) mng_info->write_png_compression_filter-1);
    }

  if (mng_info->write_png_compression_level != 0)
    png_set_compression_level(ping,mng_info->write_png_compression_level-1);

  if (mng_info->write_png_compression_filter == 6)
    {
      if (((int) ping_color_type == PNG_COLOR_TYPE_GRAY) ||
         ((int) ping_color_type == PNG_COLOR_TYPE_PALETTE) ||
         (quality < 50))
        png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_NO_FILTERS);
      else
        png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);
     }
  else if (mng_info->write_png_compression_filter == 7 ||
      mng_info->write_png_compression_filter == 10)
    png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);

  else if (mng_info->write_png_compression_filter == 8)
    {
#if defined(PNG_MNG_FEATURES_SUPPORTED) && defined(PNG_INTRAPIXEL_DIFFERENCING)
      if (mng_info->write_mng)
      {
         if (((int) ping_color_type == PNG_COLOR_TYPE_RGB) ||
             ((int) ping_color_type == PNG_COLOR_TYPE_RGBA))
        ping_filter_method=PNG_INTRAPIXEL_DIFFERENCING;
      }
#endif
      png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_NO_FILTERS);
    }

  else if (mng_info->write_png_compression_filter == 9)
    png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_NO_FILTERS);

  else if (mng_info->write_png_compression_filter != 0)
    png_set_filter(ping,PNG_FILTER_TYPE_BASE,
       mng_info->write_png_compression_filter-1);

  if (mng_info->write_png_compression_strategy != 0)
    png_set_compression_strategy(ping,
       mng_info->write_png_compression_strategy-1);

  ping_interlace_method=image_info->interlace != NoInterlace;

  if (mng_info->write_mng)
    png_set_sig_bytes(ping,8);

  /* Bail out if cannot meet defined png:bit-depth or png:color-type */

  if (mng_info->write_png_colortype != 0)
    {
     if (mng_info->write_png_colortype-1 == PNG_COLOR_TYPE_GRAY)
       if (ping_have_color != MagickFalse)
         {
           ping_color_type = PNG_COLOR_TYPE_RGB;

           if (ping_bit_depth < 8)
             ping_bit_depth=8;
         }

     if (mng_info->write_png_colortype-1 == PNG_COLOR_TYPE_GRAY_ALPHA)
       if (ping_have_color != MagickFalse)
         ping_color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    }

  if (ping_need_colortype_warning != MagickFalse ||
     ((mng_info->write_png_depth &&
     (int) mng_info->write_png_depth != ping_bit_depth) ||
     (mng_info->write_png_colortype &&
     ((int) mng_info->write_png_colortype-1 != ping_color_type &&
      mng_info->write_png_colortype != 7 &&
      !(mng_info->write_png_colortype == 5 && ping_color_type == 0)))))
    {
      if (logging != MagickFalse)
        {
          if (ping_need_colortype_warning != MagickFalse)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  Image has transparency but tRNS chunk was excluded");
            }

          if (mng_info->write_png_depth)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Defined png:bit-depth=%u, Computed depth=%u",
                  mng_info->write_png_depth,
                  ping_bit_depth);
            }

          if (mng_info->write_png_colortype)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Defined png:color-type=%u, Computed color type=%u",
                  mng_info->write_png_colortype-1,
                  ping_color_type);
            }
        }

      png_warning(ping,
        "Cannot write image with defined png:bit-depth or png:color-type.");
    }

  if (image_matte != MagickFalse && image->alpha_trait == UndefinedPixelTrait)
    {
      /* Add an opaque matte channel */
      image->alpha_trait = BlendPixelTrait;
      (void) SetImageAlpha(image,OpaqueAlpha,exception);

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  Added an opaque matte channel");
    }

  if (number_transparent != 0 || number_semitransparent != 0)
    {
      if (ping_color_type < 4)
        {
           ping_have_tRNS=MagickTrue;
           if (logging != MagickFalse)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "  Setting ping_have_tRNS=MagickTrue.");
        }
    }

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Writing PNG header chunks");

  png_set_IHDR(ping,ping_info,ping_width,ping_height,
               ping_bit_depth,ping_color_type,
               ping_interlace_method,ping_compression_method,
               ping_filter_method);

  if (ping_color_type == 3 && ping_have_PLTE != MagickFalse)
    {
      png_set_PLTE(ping,ping_info,palette,number_colors);

      if (logging != MagickFalse)
        {
          for (i=0; i< (ssize_t) number_colors; i++)
          {
            if (i < ping_num_trans)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "     PLTE[%d] = (%d,%d,%d), tRNS[%d] = (%d)",
                      (int) i,
                      (int) palette[i].red,
                      (int) palette[i].green,
                      (int) palette[i].blue,
                      (int) i,
                      (int) ping_trans_alpha[i]);
             else
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "     PLTE[%d] = (%d,%d,%d)",
                      (int) i,
                      (int) palette[i].red,
                      (int) palette[i].green,
                      (int) palette[i].blue);
           }
         }
    }

  /* Only write the iCCP chunk if we are not writing the sRGB chunk. */
  if (ping_exclude_sRGB != MagickFalse ||
     (!png_get_valid(ping,ping_info,PNG_INFO_sRGB)))
  {
    if ((ping_exclude_tEXt == MagickFalse ||
       ping_exclude_zTXt == MagickFalse) &&
       (ping_exclude_iCCP == MagickFalse || ping_exclude_zCCP == MagickFalse))
    {
      ResetImageProfileIterator(image);
      for (name=GetNextImageProfile(image); name != (const char *) NULL; )
      {
        profile=GetImageProfile(image,name);

        if (profile != (StringInfo *) NULL)
          {
#ifdef PNG_WRITE_iCCP_SUPPORTED
            if ((LocaleCompare(name,"ICC") == 0) ||
                (LocaleCompare(name,"ICM") == 0))
              {
                ping_have_iCCP = MagickTrue;
                if (ping_exclude_iCCP == MagickFalse)
                  {
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  Setting up iCCP chunk");

                    png_set_iCCP(ping,ping_info,(png_charp) name,0,
#if (PNG_LIBPNG_VER < 10500)
                    (png_charp) GetStringInfoDatum(profile),
#else
                    (const png_byte *) GetStringInfoDatum(profile),
#endif
                    (png_uint_32) GetStringInfoLength(profile));
                  }
                else
                  {
                    /* Do not write hex-encoded ICC chunk */
                       name=GetNextImageProfile(image);
                       continue;
                  }
              }
#endif /* WRITE_iCCP */

            if (LocaleCompare(name,"exif") == 0)
              {
                   /* Do not write hex-encoded ICC chunk; we will
                      write it later as an eXIf chunk */
                   name=GetNextImageProfile(image);
                   continue;
              }

              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  Setting up zTXt chunk with uuencoded %s profile",
                 name);
              Magick_png_write_raw_profile(image_info,ping,ping_info,
                (unsigned char *) name,(unsigned char *) name,
                GetStringInfoDatum(profile),
                (png_uint_32) GetStringInfoLength(profile));
          }
        name=GetNextImageProfile(image);
      }
    }
  }

#if defined(PNG_WRITE_sRGB_SUPPORTED)
  if ((mng_info->have_write_global_srgb == 0) &&
      ping_have_iCCP != MagickTrue &&
      (ping_have_sRGB != MagickFalse ||
      png_get_valid(ping,ping_info,PNG_INFO_sRGB)))
    {
      if (ping_exclude_sRGB == MagickFalse)
        {
          /*
            Note image rendering intent.
          */
          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "  Setting up sRGB chunk");

          (void) png_set_sRGB(ping,ping_info,(
            Magick_RenderingIntent_to_PNG_RenderingIntent(
              image->rendering_intent)));

          ping_have_sRGB = MagickTrue;
        }
    }

  if ((!mng_info->write_mng) || (!png_get_valid(ping,ping_info,PNG_INFO_sRGB)))
#endif
    {
      if (ping_exclude_gAMA == MagickFalse &&
          ping_have_iCCP == MagickFalse &&
          ping_have_sRGB == MagickFalse &&
          (ping_exclude_sRGB == MagickFalse ||
          (image->gamma < .45 || image->gamma > .46)))
      {
      if ((mng_info->have_write_global_gama == 0) && (image->gamma != 0.0))
        {
          /*
            Note image gamma.
            To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
          */
          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Setting up gAMA chunk");

          png_set_gAMA(ping,ping_info,image->gamma);
        }
      }

      if (ping_exclude_cHRM == MagickFalse && ping_have_sRGB == MagickFalse)
        {
          if ((mng_info->have_write_global_chrm == 0) &&
              (image->chromaticity.red_primary.x != 0.0))
            {
              /*
                Note image chromaticity.
                Note: if cHRM+gAMA == sRGB write sRGB instead.
              */
               PrimaryInfo
                 bp,
                 gp,
                 rp,
                 wp;

               wp=image->chromaticity.white_point;
               rp=image->chromaticity.red_primary;
               gp=image->chromaticity.green_primary;
               bp=image->chromaticity.blue_primary;

               if (logging != MagickFalse)
                 (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "  Setting up cHRM chunk");

               png_set_cHRM(ping,ping_info,wp.x,wp.y,rp.x,rp.y,gp.x,gp.y,
                   bp.x,bp.y);
           }
        }
    }

  if (ping_exclude_bKGD == MagickFalse)
    {
      if (ping_have_bKGD != MagickFalse)
        {
          png_set_bKGD(ping,ping_info,&ping_background);
          if (logging != MagickFalse)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    Setting up bKGD chunk");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      background color = (%d,%d,%d)",
                        (int) ping_background.red,
                        (int) ping_background.green,
                        (int) ping_background.blue);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      index = %d, gray=%d",
                        (int) ping_background.index,
                        (int) ping_background.gray);
            }
         }
    }

  if (ping_exclude_pHYs == MagickFalse)
    {
      if (ping_have_pHYs != MagickFalse)
        {
          png_set_pHYs(ping,ping_info,
             ping_pHYs_x_resolution,
             ping_pHYs_y_resolution,
             ping_pHYs_unit_type);

          if (logging != MagickFalse)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    Setting up pHYs chunk");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      x_resolution=%lu",
                   (unsigned long) ping_pHYs_x_resolution);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      y_resolution=%lu",
                   (unsigned long) ping_pHYs_y_resolution);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "      unit_type=%lu",
                   (unsigned long) ping_pHYs_unit_type);
            }
        }
    }

#if defined(PNG_tIME_SUPPORTED)
  if (ping_exclude_tIME == MagickFalse)
    {
      const char
        *timestamp;

      if (image->taint == MagickFalse)
        {
          timestamp=GetImageOption(image_info,"png:tIME");

          if (timestamp == (const char *) NULL)
            timestamp=GetImageProperty(image,"png:tIME",exception);
        }

      else
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "  Reset tIME in tainted image");

          timestamp=GetImageProperty(image,"date:modify",exception);
        }

      if (timestamp != (const char *) NULL)
          write_tIME_chunk(image,ping,ping_info,timestamp,exception);
    }
#endif

  if (mng_info->need_blob != MagickFalse)
  {
    if (OpenBlob(image_info,image,WriteBinaryBlobMode,exception) ==
       MagickFalse)
       png_error(ping,"WriteBlob Failed");

     ping_have_blob=MagickTrue;
  }

  png_write_info_before_PLTE(ping, ping_info);

  if (ping_have_tRNS != MagickFalse && ping_color_type < 4)
    {
      if (logging != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Calling png_set_tRNS with num_trans=%d",ping_num_trans);
        }

      if (ping_color_type == 3)
         (void) png_set_tRNS(ping, ping_info,
                ping_trans_alpha,
                ping_num_trans,
                NULL);

      else
        {
           (void) png_set_tRNS(ping, ping_info,
                  NULL,
                  0,
                  &ping_trans_color);

           if (logging != MagickFalse)
             {
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "     tRNS color   =(%d,%d,%d)",
                       (int) ping_trans_color.red,
                       (int) ping_trans_color.green,
                       (int) ping_trans_color.blue);
             }
         }
    }

  png_write_info(ping,ping_info);

  /* write orNT if image->orientation is defined */
  if (image->orientation != UndefinedOrientation)
    {
      unsigned char
        chunk[6];
      (void) WriteBlobMSBULong(image,1L);  /* data length=1 */
      PNGType(chunk,mng_orNT);
      LogPNGChunk(logging,mng_orNT,1L);
      /* PNG uses Exif orientation values */
      chunk[4]=Magick_Orientation_to_Exif_Orientation(image->orientation);
      (void) WriteBlob(image,5,chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
    }

  ping_wrote_caNv = MagickFalse;

  /* write caNv chunk */
  if (ping_exclude_caNv == MagickFalse)
    {
      if ((image->page.width != 0 && image->page.width != image->columns) ||
          (image->page.height != 0 && image->page.height != image->rows) ||
          image->page.x != 0 || image->page.y != 0)
        {
          unsigned char
            chunk[20];

          (void) WriteBlobMSBULong(image,16L);  /* data length=8 */
          PNGType(chunk,mng_caNv);
          LogPNGChunk(logging,mng_caNv,16L);
          PNGLong(chunk+4,(png_uint_32) image->page.width);
          PNGLong(chunk+8,(png_uint_32) image->page.height);
          PNGsLong(chunk+12,(png_int_32) image->page.x);
          PNGsLong(chunk+16,(png_int_32) image->page.y);
          (void) WriteBlob(image,20,chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,20));
          ping_wrote_caNv = MagickTrue;
        }
    }

#if defined(PNG_oFFs_SUPPORTED)
  if (ping_exclude_oFFs == MagickFalse && ping_wrote_caNv == MagickFalse)
    {
      if (image->page.x || image->page.y)
        {
           png_set_oFFs(ping,ping_info,(png_int_32) image->page.x,
              (png_int_32) image->page.y, 0);

           if (logging != MagickFalse)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "    Setting up oFFs chunk with x=%d, y=%d, units=0",
                 (int) image->page.x, (int) image->page.y);
        }
    }
#endif

#if (PNG_LIBPNG_VER == 10206)
    /* avoid libpng-1.2.6 bug by setting PNG_HAVE_IDAT flag */
#define PNG_HAVE_IDAT               0x04
    ping->mode |= PNG_HAVE_IDAT;
#undef PNG_HAVE_IDAT
#endif

  png_set_packing(ping);
  /*
    Allocate memory.
  */
  rowbytes=image->columns;
  if (image_depth > 8)
    rowbytes*=2;
  switch (ping_color_type)
    {
      case PNG_COLOR_TYPE_RGB:
        rowbytes*=3;
        break;

      case PNG_COLOR_TYPE_GRAY_ALPHA:
        rowbytes*=2;
        break;

      case PNG_COLOR_TYPE_RGBA:
        rowbytes*=4;
        break;

      default:
        break;
    }

  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Writing PNG image data");

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    Allocating %.20g bytes of memory for pixels",(double) rowbytes);
    }
  pixel_info=AcquireVirtualMemory(rowbytes,sizeof(*ping_pixels));
  if (pixel_info == (MemoryInfo *) NULL)
    png_error(ping,"Allocation of memory for pixels failed");
  ping_pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
  /*
    Initialize image scanlines.
  */
  quantum_info=AcquireQuantumInfo(image_info,image);
  if (quantum_info == (QuantumInfo *) NULL)
    png_error(ping,"Memory allocation for quantum_info failed");
  quantum_info->format=UndefinedQuantumFormat;
  SetQuantumDepth(image,quantum_info,image_depth);
  (void) SetQuantumEndian(image,quantum_info,MSBEndian);
  num_passes=png_set_interlace_handling(ping);

  if ((!mng_info->write_png8 && !mng_info->write_png24 &&
       !mng_info->write_png48 && !mng_info->write_png64 &&
       !mng_info->write_png32) &&
       (mng_info->IsPalette ||
       (image_info->type == BilevelType)) &&
       image_matte == MagickFalse &&
       ping_have_non_bw == MagickFalse)
    {
      /* Palette, Bilevel, or Opaque Monochrome */
      register const Quantum
        *p;

      SetQuantumDepth(image,quantum_info,8);
      for (pass=0; pass < num_passes; pass++)
      {
        /*
          Convert PseudoClass image to a PNG monochrome image.
        */
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          if (logging != MagickFalse && y == 0)
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "    Writing row of pixels (0)");

          p=GetVirtualPixels(image,0,y,image->columns,1,exception);

          if (p == (const Quantum *) NULL)
            break;

          if (mng_info->IsPalette)
            {
              (void) ExportQuantumPixels(image,(CacheView *) NULL,
                quantum_info,GrayQuantum,ping_pixels,exception);
              if (mng_info->write_png_colortype-1 == PNG_COLOR_TYPE_PALETTE &&
                  mng_info->write_png_depth &&
                  mng_info->write_png_depth != old_bit_depth)
                {
                  /* Undo pixel scaling */
                  for (i=0; i < (ssize_t) image->columns; i++)
                     *(ping_pixels+i)=(unsigned char) (*(ping_pixels+i)
                     >> (8-old_bit_depth));
                }
            }

          else
            {
              (void) ExportQuantumPixels(image,(CacheView *) NULL,
                quantum_info,RedQuantum,ping_pixels,exception);
            }

          if (mng_info->write_png_colortype-1 != PNG_COLOR_TYPE_PALETTE)
            for (i=0; i < (ssize_t) image->columns; i++)
               *(ping_pixels+i)=(unsigned char) ((*(ping_pixels+i) > 127) ?
                      255 : 0);

          if (logging != MagickFalse && y == 0)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "    Writing row of pixels (1)");

          png_write_row(ping,ping_pixels);

          status=SetImageProgress(image,SaveImageTag,
              (MagickOffsetType) (pass * image->rows + y),
              num_passes * image->rows);

          if (status == MagickFalse)
            break;
        }
      }
    }

  else   /* Not Palette, Bilevel, or Opaque Monochrome */
    {
      if ((!mng_info->write_png8 && !mng_info->write_png24 &&
          !mng_info->write_png48 && !mng_info->write_png64 &&
          !mng_info->write_png32) && (image_matte != MagickFalse ||
          (ping_bit_depth >= MAGICKCORE_QUANTUM_DEPTH)) &&
          (mng_info->IsPalette) && ping_have_color == MagickFalse)
        {
          register const Quantum
            *p;

          for (pass=0; pass < num_passes; pass++)
          {

          for (y=0; y < (ssize_t) image->rows; y++)
          {
            p=GetVirtualPixels(image,0,y,image->columns,1,exception);

            if (p == (const Quantum *) NULL)
              break;

            if (ping_color_type == PNG_COLOR_TYPE_GRAY)
              {
                if (mng_info->IsPalette)
                  (void) ExportQuantumPixels(image,(CacheView *) NULL,
                    quantum_info,GrayQuantum,ping_pixels,exception);

                else
                  (void) ExportQuantumPixels(image,(CacheView *) NULL,
                    quantum_info,RedQuantum,ping_pixels,exception);

                if (logging != MagickFalse && y == 0)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "    Writing GRAY PNG pixels (2)");
              }

            else /* PNG_COLOR_TYPE_GRAY_ALPHA */
              {
                if (logging != MagickFalse && y == 0)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                         "    Writing GRAY_ALPHA PNG pixels (2)");

                (void) ExportQuantumPixels(image,(CacheView *) NULL,
                  quantum_info,GrayAlphaQuantum,ping_pixels,exception);
              }

            if (logging != MagickFalse && y == 0)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    Writing row of pixels (2)");

            png_write_row(ping,ping_pixels);

            status=SetImageProgress(image,SaveImageTag,
              (MagickOffsetType) (pass * image->rows + y),
              num_passes * image->rows);

            if (status == MagickFalse)
              break;
            }
          }
        }

      else
        {
          register const Quantum
            *p;

          for (pass=0; pass < num_passes; pass++)
          {
            if ((image_depth > 8) ||
                mng_info->write_png24 ||
                mng_info->write_png32 ||
                mng_info->write_png48 ||
                mng_info->write_png64 ||
                (!mng_info->write_png8 && !mng_info->IsPalette))
            {
              for (y=0; y < (ssize_t) image->rows; y++)
              {
                p=GetVirtualPixels(image,0,y,image->columns,1, exception);

                if (p == (const Quantum *) NULL)
                  break;

                if (ping_color_type == PNG_COLOR_TYPE_GRAY)
                  {
                    if (image->storage_class == DirectClass)
                      (void) ExportQuantumPixels(image,(CacheView *) NULL,
                        quantum_info,RedQuantum,ping_pixels,exception);

                    else
                      (void) ExportQuantumPixels(image,(CacheView *) NULL,
                        quantum_info,GrayQuantum,ping_pixels,exception);
                  }

                else if (ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                  {
                    (void) ExportQuantumPixels(image,(CacheView *) NULL,
                      quantum_info,GrayAlphaQuantum,ping_pixels,
                      exception);

                    if (logging != MagickFalse && y == 0)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                           "    Writing GRAY_ALPHA PNG pixels (3)");
                  }

                else if (image_matte != MagickFalse)
                  (void) ExportQuantumPixels(image,(CacheView *) NULL,
                    quantum_info,RGBAQuantum,ping_pixels,exception);

                else
                  (void) ExportQuantumPixels(image,(CacheView *) NULL,
                    quantum_info,RGBQuantum,ping_pixels,exception);

                if (logging != MagickFalse && y == 0)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                      "    Writing row of pixels (3)");

                png_write_row(ping,ping_pixels);

                status=SetImageProgress(image,SaveImageTag,
                  (MagickOffsetType) (pass * image->rows + y),
                  num_passes * image->rows);

                if (status == MagickFalse)
                  break;
              }
            }

          else
            /* not ((image_depth > 8) ||
                mng_info->write_png24 || mng_info->write_png32 ||
                mng_info->write_png48 || mng_info->write_png64 ||
                (!mng_info->write_png8 && !mng_info->IsPalette))
             */
            {
              if ((ping_color_type != PNG_COLOR_TYPE_GRAY) &&
                  (ping_color_type != PNG_COLOR_TYPE_GRAY_ALPHA))
                {
                  if (logging != MagickFalse)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                      "  pass %d, Image Is not GRAY or GRAY_ALPHA",pass);

                  SetQuantumDepth(image,quantum_info,8);
                  image_depth=8;
                }

              for (y=0; y < (ssize_t) image->rows; y++)
              {
                if (logging != MagickFalse && y == 0)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "  pass %d, Image Is RGB, 16-bit GRAY, or GRAY_ALPHA",
                    pass);

                p=GetVirtualPixels(image,0,y,image->columns,1, exception);

                if (p == (const Quantum *) NULL)
                  break;

                if (ping_color_type == PNG_COLOR_TYPE_GRAY)
                  {
                    SetQuantumDepth(image,quantum_info,image->depth);

                    (void) ExportQuantumPixels(image,(CacheView *) NULL,
                       quantum_info,GrayQuantum,ping_pixels,exception);
                  }

                else if (ping_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                  {
                    if (logging != MagickFalse && y == 0)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                           "  Writing GRAY_ALPHA PNG pixels (4)");

                    (void) ExportQuantumPixels(image,(CacheView *) NULL,
                         quantum_info,GrayAlphaQuantum,ping_pixels,
                         exception);
                  }

                else
                  {
                    (void) ExportQuantumPixels(image,(CacheView *) NULL,
                      quantum_info,IndexQuantum,ping_pixels,exception);

                    if (logging != MagickFalse && y <= 2)
                    {
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Writing row of non-gray pixels (4)");

                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  ping_pixels[0]=%d,ping_pixels[1]=%d",
                          (int)ping_pixels[0],(int)ping_pixels[1]);
                    }
                  }
                png_write_row(ping,ping_pixels);

                status=SetImageProgress(image,SaveImageTag,
                  (MagickOffsetType) (pass * image->rows + y),
                  num_passes * image->rows);

                if (status == MagickFalse)
                  break;
              }
            }
          }
        }
    }

  if (quantum_info != (QuantumInfo *) NULL)
    quantum_info=DestroyQuantumInfo(quantum_info);

  if (logging != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Wrote PNG image data");

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    Width: %.20g",(double) ping_width);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    Height: %.20g",(double) ping_height);

      if (mng_info->write_png_depth)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Defined png:bit-depth: %d",mng_info->write_png_depth);
        }

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    PNG bit-depth written: %d",ping_bit_depth);

      if (mng_info->write_png_colortype)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Defined png:color-type: %d",mng_info->write_png_colortype-1);
        }

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    PNG color-type written: %d",ping_color_type);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "    PNG Interlace method: %d",ping_interlace_method);
    }
  /*
    Generate text chunks after IDAT.
  */
  if (ping_exclude_tEXt == MagickFalse || ping_exclude_zTXt == MagickFalse)
  {
    ResetImagePropertyIterator(image);
    property=GetNextImageProperty(image);
    while (property != (const char *) NULL)
    {
      png_textp
        text;

      value=GetImageProperty(image,property,exception);

      /* Don't write any "png:" or "jpeg:" properties; those are just for
       * "identify" or for passing through to another JPEG
       */
      if ((LocaleNCompare(property,"png:",4) != 0 &&
           LocaleNCompare(property,"jpeg:",5) != 0) &&


          /* Suppress density and units if we wrote a pHYs chunk */
          (ping_exclude_pHYs != MagickFalse      ||
          LocaleCompare(property,"density") != 0 ||
          LocaleCompare(property,"units") != 0) &&

          /* Suppress the IM-generated Date:create and Date:modify */
          (ping_exclude_date == MagickFalse      ||
          LocaleNCompare(property, "Date:",5) != 0))
        {
        if (value != (const char *) NULL)
          {

#if PNG_LIBPNG_VER >= 10400
            text=(png_textp) png_malloc(ping,
                 (png_alloc_size_t) sizeof(png_text));
#else
            text=(png_textp) png_malloc(ping,(png_size_t) sizeof(png_text));
#endif
            text[0].key=(char *) property;
            text[0].text=(char *) value;
            text[0].text_length=strlen(value);

            if (ping_exclude_tEXt != MagickFalse)
               text[0].compression=PNG_TEXT_COMPRESSION_zTXt;

            else if (ping_exclude_zTXt != MagickFalse)
               text[0].compression=PNG_TEXT_COMPRESSION_NONE;

            else
            {
               text[0].compression=image_info->compression == NoCompression ||
                 (image_info->compression == UndefinedCompression &&
                 text[0].text_length < 128) ? PNG_TEXT_COMPRESSION_NONE :
                 PNG_TEXT_COMPRESSION_zTXt ;
            }

            if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "  Setting up text chunk");

                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    keyword: '%s'",text[0].key);
              }

            png_set_text(ping,ping_info,text,1);
            png_free(ping,text);
          }
        }
      property=GetNextImageProperty(image);
    }
  }

  /* write eXIf profile */
  if (ping_have_eXIf != MagickFalse && ping_exclude_eXIf == MagickFalse)
    {
      char
        *name;

      ResetImageProfileIterator(image);

      for (name=GetNextImageProfile(image); name != (const char *) NULL; )
      {
        if (LocaleCompare(name,"exif") == 0)
          {
            const StringInfo
              *profile;

            profile=GetImageProfile(image,name);

            if (profile != (StringInfo *) NULL)
              {
                png_uint_32
                  length;

                unsigned char
                  chunk[4],
                  *data;

                StringInfo
                  *ping_profile;

                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "  Have eXIf profile");

                ping_profile=CloneStringInfo(profile);
                data=GetStringInfoDatum(ping_profile),
                length=(png_uint_32) GetStringInfoLength(ping_profile);

                PNGType(chunk,mng_eXIf);
                if (length < 7)
                  {
                    ping_profile=DestroyStringInfo(ping_profile);
                    break;  /* otherwise crashes */
                  }

                if (*data == 'E' && *(data+1) == 'x' && *(data+2) == 'i' &&
                    *(data+3) == 'f' && *(data+4) == '\0' && *(data+5) == '\0')
                  {
                    /* skip the "Exif\0\0" JFIF Exif Header ID */
                    length -= 6;
                    data += 6;
                  }

                LogPNGChunk(logging,chunk,length);
                (void) WriteBlobMSBULong(image,length);
                (void) WriteBlob(image,4,chunk);
                (void) WriteBlob(image,length,data);
                (void) WriteBlobMSBULong(image,crc32(crc32(0,chunk,4), data,
                  (uInt) length));
                ping_profile=DestroyStringInfo(ping_profile);
                break;
             }
         }
       name=GetNextImageProfile(image);
     }
  }

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Writing PNG end info");

  png_write_end(ping,ping_info);

  if (mng_info->need_fram && (int) image->dispose == BackgroundDispose)
    {
      if (mng_info->page.x || mng_info->page.y ||
          (ping_width != mng_info->page.width) ||
          (ping_height != mng_info->page.height))
        {
          unsigned char
            chunk[32];

          /*
            Write FRAM 4 with clipping boundaries followed by FRAM 1.
          */
          (void) WriteBlobMSBULong(image,27L);  /* data length=27 */
          PNGType(chunk,mng_FRAM);
          LogPNGChunk(logging,mng_FRAM,27L);
          chunk[4]=4;
          chunk[5]=0;  /* frame name separator (no name) */
          chunk[6]=1;  /* flag for changing delay, for next frame only */
          chunk[7]=0;  /* flag for changing frame timeout */
          chunk[8]=1;  /* flag for changing frame clipping for next frame */
          chunk[9]=0;  /* flag for changing frame sync_id */
          PNGLong(chunk+10,(png_uint_32) (0L)); /* temporary 0 delay */
          chunk[14]=0; /* clipping boundaries delta type */
          PNGLong(chunk+15,(png_uint_32) (mng_info->page.x)); /* left cb */
          PNGLong(chunk+19,
             (png_uint_32) (mng_info->page.x + ping_width));
          PNGLong(chunk+23,(png_uint_32) (mng_info->page.y)); /* top cb */
          PNGLong(chunk+27,
             (png_uint_32) (mng_info->page.y + ping_height));
          (void) WriteBlob(image,31,chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,31));
          mng_info->old_framing_mode=4;
          mng_info->framing_mode=1;
        }

      else
        mng_info->framing_mode=3;
    }
  if (mng_info->write_mng && !mng_info->need_fram &&
      ((int) image->dispose == 3))
     png_error(ping, "Cannot convert GIF with disposal method 3 to MNG-LC");

  /*
    Free PNG resources.
  */

  png_destroy_write_struct(&ping,&ping_info);

  pixel_info=RelinquishVirtualMemory(pixel_info);

  if (ping_have_blob != MagickFalse)
     (void) CloseBlob(image);

  image_info=DestroyImageInfo(image_info);
  image=DestroyImage(image);

  /* Store bit depth actually written */
  s[0]=(char) ping_bit_depth;
  s[1]='\0';

  (void) SetImageProperty(IMimage,"png:bit-depth-written",s,exception);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  exit WriteOnePNGImage()");

#ifdef IMPNG_SETJMP_NOT_THREAD_SAFE
  UnlockSemaphoreInfo(ping_semaphore);
#endif

   /* }  for navigation to beginning of SETJMP-protected block. Revert to
    *    Throwing an Exception when an error occurs.
    */

  return(MagickTrue);
/*  End write one PNG image */

}