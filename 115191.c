static int read_user_chunk_callback(png_struct *ping, png_unknown_chunkp chunk)
{
  Image
    *image;


  /* The unknown chunk structure contains the chunk data:
     png_byte name[5];
     png_byte *data;
     png_size_t size;

     Note that libpng has already taken care of the CRC handling.

     Returns one of the following:
         return(-n);  chunk had an error
         return(0);  did not recognize
         return(n);  success
  */

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
     "    read_user_chunk: found %c%c%c%c chunk",
       chunk->name[0],chunk->name[1],chunk->name[2],chunk->name[3]);

#if defined(zxIf_SUPPORTED)
  if ((chunk->name[0]  == 122 || chunk->name[0] == 117 ) &&
      (chunk->name[1] ==   88 || chunk->name[1] == 120 ) &&
      chunk->name[2] ==   73 &&
      chunk-> name[3] == 102)
    {
      /* process uxIf or zxIf chunk */
      StringInfo *
        profile;

      PNGErrorInfo
        *error_info;

      unsigned char
        *p;

      png_byte
        *s;

      size_t
        i;

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "     recognized uxIf|zxIf chunk");

      image=(Image *) png_get_user_chunk_ptr(ping);

      error_info=(PNGErrorInfo *) png_get_error_ptr(ping);

      profile=BlobToStringInfo((const void *) NULL,chunk->size+6);

      if (profile == (StringInfo *) NULL)
        {
          (void) ThrowMagickException(error_info->exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'",
            image->filename);
          return(-1);
        }
      p=GetStringInfoDatum(profile);

      /* Initialize profile with "Exif\0\0" */
      *p++ ='E';
      *p++ ='x';
      *p++ ='i';
      *p++ ='f';
      *p++ ='\0';
      *p++ ='\0';

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "     initialized uxIf|zxIf chunk");

      switch (chunk->data[0])
        {
          PNGErrorInfo
            *error_info;
          case 'E':
          case 'I':
          {
            /* Uncompressed */
            /* copy chunk->data to profile */
            s=chunk->data;
            for (i=0; i<chunk->size; i++)
              *p++ = *s++;

            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "     SetImageProfile with %lu bytes",
               (unsigned long) chunk->size+6);
            error_info=(PNGErrorInfo *) png_get_error_ptr(ping);
            (void) SetImageProfile(image,"exif",profile,
              error_info->exception);
            return(1);
          }
          case '\0':
          {
            /* Zlib compressed */

            unsigned char *
              temp;

            png_uint_32 inflated_size;

            png_free(ping,profile);

            if (chunk->size < 5)
               return(-1);

            s=chunk->data;
            s++;  // skip compression byte

            inflated_size = (png_uint_32)
               (((s[0] & 0xff) << 24) | ((s[1] & 0xff) << 16) |
                ((s[2] & 0xff) <<  8) | ((s[3] & 0xff)      ));

            s+=4;

            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "     inflated_size = %lu bytes",inflated_size);

            /* uncompress chunk->data to temporary profile */
            inflated_size=exif_inf(ping,s,&temp,chunk->size-1,inflated_size);

            if (inflated_size <= 0)
            {
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "     inflated_size = %lu bytes",inflated_size);
               return(-1);
            }
           error_info=(PNGErrorInfo *) png_get_error_ptr(ping);

           profile=BlobToStringInfo((const void *) NULL,inflated_size+6);

           if (profile == (StringInfo *) NULL)
             {
               (void) ThrowMagickException(error_info->exception,
                 GetMagickModule(),
                 ResourceLimitError,"MemoryAllocationFailed","`%s'",
                 image->filename);
               return(-1);
             }
           p=GetStringInfoDatum(profile);
           /* Initialize profile with "Exif\0\0" */
           *p++ ='E';
           *p++ ='x';
           *p++ ='i';
           *p++ ='f';
           *p++ ='\0';
           *p++ ='\0';

           for (i=0; i<inflated_size; i++)
              *p++=temp[i];

            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "     SetImageProfile with %lu bytes",
               (unsigned long) inflated_size+6);
            error_info=(PNGErrorInfo *) png_get_error_ptr(ping);
            (void) SetImageProfile(image,"exif",profile,
              error_info->exception);

            png_free(ping,temp);

            return(1);
          }
       }
    }
#endif /* zxIf_SUPPORTED */

  if (chunk->name[0]  == 101 &&
      (chunk->name[1] ==  88 || chunk->name[1] == 120 ) &&
      chunk->name[2] ==   73 &&
      chunk-> name[3] == 102)
    {
      /* process eXIf or exIf chunk */

      PNGErrorInfo
        *error_info;

      StringInfo
        *profile;

      unsigned char
        *p;

      png_byte
        *s;

      size_t
        i;

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        " recognized eXIf|exIf chunk");

      image=(Image *) png_get_user_chunk_ptr(ping);

      error_info=(PNGErrorInfo *) png_get_error_ptr(ping);

      profile=BlobToStringInfo((const void *) NULL,chunk->size+6);

      if (profile == (StringInfo *) NULL)
        {
          (void) ThrowMagickException(error_info->exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'",
            image->filename);
          return(-1);
        }
      p=GetStringInfoDatum(profile);

      /* Initialize profile with "Exif\0\0" */
      *p++ ='E';
      *p++ ='x';
      *p++ ='i';
      *p++ ='f';
      *p++ ='\0';
      *p++ ='\0';

      /* copy chunk->data to profile */
      s=chunk->data;
      for (i=0; i<chunk->size; i++)
        *p++ = *s++;

      error_info=(PNGErrorInfo *) png_get_error_ptr(ping);
      (void) SetImageProfile(image,"exif",profile,
        error_info->exception);

      return(1);
    }

  /* vpAg (deprecated, replaced by caNv) */
  if (chunk->name[0] == 118 &&
      chunk->name[1] == 112 &&
      chunk->name[2] ==  65 &&
      chunk->name[3] == 103)
    {
     /* recognized vpAg */

     if (chunk->size != 9)
       return(-1); /* Error return */

     if (chunk->data[8] != 0)
       return(0);  /* ImageMagick requires pixel units */

     image=(Image *) png_get_user_chunk_ptr(ping);

     image->page.width=(size_t) ((chunk->data[0] << 24) |
        (chunk->data[1] << 16) | (chunk->data[2] << 8) | chunk->data[3]);

     image->page.height=(size_t) ((chunk->data[4] << 24) |
        (chunk->data[5] << 16) | (chunk->data[6] << 8) | chunk->data[7]);

     return(1);
    }

  /* caNv */
  if (chunk->name[0] ==  99 &&
      chunk->name[1] ==  97 &&
      chunk->name[2] ==  78 &&
      chunk->name[3] == 118)
    {
     /* recognized caNv */

     if (chunk->size != 16)
       return(-1); /* Error return */

     image=(Image *) png_get_user_chunk_ptr(ping);

     image->page.width=(size_t) ((chunk->data[0] << 24) |
        (chunk->data[1] << 16) | (chunk->data[2] << 8) | chunk->data[3]);

     image->page.height=(size_t) ((chunk->data[4] << 24) |
        (chunk->data[5] << 16) | (chunk->data[6] << 8) | chunk->data[7]);

     image->page.x=(size_t) ((chunk->data[8] << 24) |
        (chunk->data[9] << 16) | (chunk->data[10] << 8) | chunk->data[11]);

     image->page.y=(size_t) ((chunk->data[12] << 24) |
        (chunk->data[13] << 16) | (chunk->data[14] << 8) | chunk->data[15]);

     return(1);
    }

  return(0); /* Did not recognize */
}