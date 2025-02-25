static MagickBooleanType WritePICTImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
#define MaxCount  128
#define PictCropRegionOp  0x01
#define PictEndOfPictureOp  0xff
#define PictJPEGOp  0x8200
#define PictInfoOp  0x0C00
#define PictInfoSize  512
#define PictPixmapOp  0x9A
#define PictPICTOp  0x98
#define PictVersion  0x11

  const StringInfo
    *profile;

  double
    x_resolution,
    y_resolution;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  PICTPixmap
    pixmap;

  PICTRectangle
    bounds,
    crop_rectangle,
    destination_rectangle,
    frame_rectangle,
    size_rectangle,
    source_rectangle;

  register const Quantum
    *p;

  register ssize_t
    i,
    x;

  size_t
    bytes_per_line,
    count,
    row_bytes,
    storage_class;

  ssize_t
    y;

  unsigned char
    *buffer,
    *packed_scanline,
    *scanline;

  unsigned short
    base_address,
    transfer_mode;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns > 65535L) || (image->rows > 65535L))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  (void) TransformImageColorspace(image,sRGBColorspace,exception);
  /*
    Initialize image info.
  */
  size_rectangle.top=0;
  size_rectangle.left=0;
  size_rectangle.bottom=(short) image->rows;
  size_rectangle.right=(short) image->columns;
  frame_rectangle=size_rectangle;
  crop_rectangle=size_rectangle;
  source_rectangle=size_rectangle;
  destination_rectangle=size_rectangle;
  base_address=0xff;
  row_bytes=image->columns;
  bounds.top=0;
  bounds.left=0;
  bounds.bottom=(short) image->rows;
  bounds.right=(short) image->columns;
  pixmap.version=0;
  pixmap.pack_type=0;
  pixmap.pack_size=0;
  pixmap.pixel_type=0;
  pixmap.bits_per_pixel=8;
  pixmap.component_count=1;
  pixmap.component_size=8;
  pixmap.plane_bytes=0;
  pixmap.table=0;
  pixmap.reserved=0;
  transfer_mode=0;
  x_resolution=image->resolution.x != 0.0 ? image->resolution.x :
    DefaultResolution;
  y_resolution=image->resolution.y != 0.0 ? image->resolution.y :
    DefaultResolution;
  storage_class=image->storage_class;
  if (image_info->compression == JPEGCompression)
    storage_class=DirectClass;
  if (storage_class == DirectClass)
    {
      pixmap.component_count=image->alpha_trait != UndefinedPixelTrait ? 4 : 3;
      pixmap.pixel_type=16;
      pixmap.bits_per_pixel=32;
      pixmap.pack_type=0x04;
      transfer_mode=0x40;
      row_bytes=4*image->columns;
    }
  /*
    Allocate memory.
  */
  bytes_per_line=image->columns;
  if (storage_class == DirectClass)
    bytes_per_line*=image->alpha_trait != UndefinedPixelTrait ? 4 : 3;
  buffer=(unsigned char *) AcquireQuantumMemory(PictInfoSize,sizeof(*buffer));
  packed_scanline=(unsigned char *) AcquireQuantumMemory((size_t)
   (row_bytes+MaxCount),sizeof(*packed_scanline));
  scanline=(unsigned char *) AcquireQuantumMemory(row_bytes,sizeof(*scanline));
  if ((buffer == (unsigned char *) NULL) ||
      (packed_scanline == (unsigned char *) NULL) ||
      (scanline == (unsigned char *) NULL))
    {
      if (scanline != (unsigned char *) NULL)
        scanline=(unsigned char *) RelinquishMagickMemory(scanline);
      if (packed_scanline != (unsigned char *) NULL)
        packed_scanline=(unsigned char *) RelinquishMagickMemory(
          packed_scanline);
      if (buffer != (unsigned char *) NULL)
        buffer=(unsigned char *) RelinquishMagickMemory(buffer);
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    }
  (void) ResetMagickMemory(scanline,0,row_bytes);
  (void) ResetMagickMemory(packed_scanline,0,(size_t) (row_bytes+MaxCount));
  /*
    Write header, header size, size bounding box, version, and reserved.
  */
  (void) ResetMagickMemory(buffer,0,PictInfoSize);
  (void) WriteBlob(image,PictInfoSize,buffer);
  (void) WriteBlobMSBShort(image,0);
  (void) WriteBlobMSBShort(image,(unsigned short) size_rectangle.top);
  (void) WriteBlobMSBShort(image,(unsigned short) size_rectangle.left);
  (void) WriteBlobMSBShort(image,(unsigned short) size_rectangle.bottom);
  (void) WriteBlobMSBShort(image,(unsigned short) size_rectangle.right);
  (void) WriteBlobMSBShort(image,PictVersion);
  (void) WriteBlobMSBShort(image,0x02ff);  /* version #2 */
  (void) WriteBlobMSBShort(image,PictInfoOp);
  (void) WriteBlobMSBLong(image,0xFFFE0000UL);
  /*
    Write full size of the file, resolution, frame bounding box, and reserved.
  */
  (void) WriteBlobMSBShort(image,(unsigned short) x_resolution);
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,(unsigned short) y_resolution);
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,(unsigned short) frame_rectangle.top);
  (void) WriteBlobMSBShort(image,(unsigned short) frame_rectangle.left);
  (void) WriteBlobMSBShort(image,(unsigned short) frame_rectangle.bottom);
  (void) WriteBlobMSBShort(image,(unsigned short) frame_rectangle.right);
  (void) WriteBlobMSBLong(image,0x00000000L);
  profile=GetImageProfile(image,"iptc");
  if (profile != (StringInfo *) NULL)
    {
      (void) WriteBlobMSBShort(image,0xa1);
      (void) WriteBlobMSBShort(image,0x1f2);
      (void) WriteBlobMSBShort(image,(unsigned short)
        (GetStringInfoLength(profile)+4));
      (void) WriteBlobString(image,"8BIM");
      (void) WriteBlob(image,GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
    }
  profile=GetImageProfile(image,"icc");
  if (profile != (StringInfo *) NULL)
    {
      (void) WriteBlobMSBShort(image,0xa1);
      (void) WriteBlobMSBShort(image,0xe0);
      (void) WriteBlobMSBShort(image,(unsigned short)
        (GetStringInfoLength(profile)+4));
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlob(image,GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
      (void) WriteBlobMSBShort(image,0xa1);
      (void) WriteBlobMSBShort(image,0xe0);
      (void) WriteBlobMSBShort(image,4);
      (void) WriteBlobMSBLong(image,0x00000002UL);
    }
  /*
    Write crop region opcode and crop bounding box.
  */
  (void) WriteBlobMSBShort(image,PictCropRegionOp);
  (void) WriteBlobMSBShort(image,0xa);
  (void) WriteBlobMSBShort(image,(unsigned short) crop_rectangle.top);
  (void) WriteBlobMSBShort(image,(unsigned short) crop_rectangle.left);
  (void) WriteBlobMSBShort(image,(unsigned short) crop_rectangle.bottom);
  (void) WriteBlobMSBShort(image,(unsigned short) crop_rectangle.right);
  if (image_info->compression == JPEGCompression)
    {
      Image
        *jpeg_image;

      ImageInfo
        *jpeg_info;

      size_t
        length;

      unsigned char
        *blob;

      jpeg_image=CloneImage(image,0,0,MagickTrue,exception);
      if (jpeg_image == (Image *) NULL)
        {
          (void) CloseBlob(image);
          return(MagickFalse);
        }
      jpeg_info=CloneImageInfo(image_info);
      (void) CopyMagickString(jpeg_info->magick,"JPEG",MagickPathExtent);
      length=0;
      blob=(unsigned char *) ImageToBlob(jpeg_info,jpeg_image,&length,
        exception);
      jpeg_info=DestroyImageInfo(jpeg_info);
      if (blob == (unsigned char *) NULL)
        return(MagickFalse);
      jpeg_image=DestroyImage(jpeg_image);
      (void) WriteBlobMSBShort(image,PictJPEGOp);
      (void) WriteBlobMSBLong(image,(unsigned int) length+154);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBLong(image,0x00010000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00010000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x40000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00400000UL);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBShort(image,(unsigned short) image->rows);
      (void) WriteBlobMSBShort(image,(unsigned short) image->columns);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBShort(image,768);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00566A70UL);
      (void) WriteBlobMSBLong(image,0x65670000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000001UL);
      (void) WriteBlobMSBLong(image,0x00016170UL);
      (void) WriteBlobMSBLong(image,0x706C0000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBShort(image,768);
      (void) WriteBlobMSBShort(image,(unsigned short) image->columns);
      (void) WriteBlobMSBShort(image,(unsigned short) image->rows);
      (void) WriteBlobMSBShort(image,(unsigned short) x_resolution);
      (void) WriteBlobMSBShort(image,0x0000);
      (void) WriteBlobMSBShort(image,(unsigned short) y_resolution);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x87AC0001UL);
      (void) WriteBlobMSBLong(image,0x0B466F74UL);
      (void) WriteBlobMSBLong(image,0x6F202D20UL);
      (void) WriteBlobMSBLong(image,0x4A504547UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x00000000UL);
      (void) WriteBlobMSBLong(image,0x0018FFFFUL);
      (void) WriteBlob(image,length,blob);
      if ((length & 0x01) != 0)
        (void) WriteBlobByte(image,'\0');
      blob=(unsigned char *) RelinquishMagickMemory(blob);
    }
  /*
    Write picture opcode, row bytes, and picture bounding box, and version.
  */
  if (storage_class == PseudoClass)
    (void) WriteBlobMSBShort(image,PictPICTOp);
  else
    {
      (void) WriteBlobMSBShort(image,PictPixmapOp);
      (void) WriteBlobMSBLong(image,(size_t) base_address);
    }
  (void) WriteBlobMSBShort(image,(unsigned short) (row_bytes | 0x8000));
  (void) WriteBlobMSBShort(image,(unsigned short) bounds.top);
  (void) WriteBlobMSBShort(image,(unsigned short) bounds.left);
  (void) WriteBlobMSBShort(image,(unsigned short) bounds.bottom);
  (void) WriteBlobMSBShort(image,(unsigned short) bounds.right);
  /*
    Write pack type, pack size, resolution, pixel type, and pixel size.
  */
  (void) WriteBlobMSBShort(image,(unsigned short) pixmap.version);
  (void) WriteBlobMSBShort(image,(unsigned short) pixmap.pack_type);
  (void) WriteBlobMSBLong(image,(unsigned int) pixmap.pack_size);
  (void) WriteBlobMSBShort(image,(unsigned short) (x_resolution+0.5));
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,(unsigned short) (y_resolution+0.5));
  (void) WriteBlobMSBShort(image,0x0000);
  (void) WriteBlobMSBShort(image,(unsigned short) pixmap.pixel_type);
  (void) WriteBlobMSBShort(image,(unsigned short) pixmap.bits_per_pixel);
  /*
    Write component count, size, plane bytes, table size, and reserved.
  */
  (void) WriteBlobMSBShort(image,(unsigned short) pixmap.component_count);
  (void) WriteBlobMSBShort(image,(unsigned short) pixmap.component_size);
  (void) WriteBlobMSBLong(image,(unsigned int) pixmap.plane_bytes);
  (void) WriteBlobMSBLong(image,(unsigned int) pixmap.table);
  (void) WriteBlobMSBLong(image,(unsigned int) pixmap.reserved);
  if (storage_class == PseudoClass)
    {
      /*
        Write image colormap.
      */
      (void) WriteBlobMSBLong(image,0x00000000L);  /* color seed */
      (void) WriteBlobMSBShort(image,0L);  /* color flags */
      (void) WriteBlobMSBShort(image,(unsigned short) (image->colors-1));
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        (void) WriteBlobMSBShort(image,(unsigned short) i);
        (void) WriteBlobMSBShort(image,ScaleQuantumToShort(
          image->colormap[i].red));
        (void) WriteBlobMSBShort(image,ScaleQuantumToShort(
          image->colormap[i].green));
        (void) WriteBlobMSBShort(image,ScaleQuantumToShort(
          image->colormap[i].blue));
      }
    }
  /*
    Write source and destination rectangle.
  */
  (void) WriteBlobMSBShort(image,(unsigned short) source_rectangle.top);
  (void) WriteBlobMSBShort(image,(unsigned short) source_rectangle.left);
  (void) WriteBlobMSBShort(image,(unsigned short) source_rectangle.bottom);
  (void) WriteBlobMSBShort(image,(unsigned short) source_rectangle.right);
  (void) WriteBlobMSBShort(image,(unsigned short) destination_rectangle.top);
  (void) WriteBlobMSBShort(image,(unsigned short) destination_rectangle.left);
  (void) WriteBlobMSBShort(image,(unsigned short) destination_rectangle.bottom);
  (void) WriteBlobMSBShort(image,(unsigned short) destination_rectangle.right);
  (void) WriteBlobMSBShort(image,(unsigned short) transfer_mode);
  /*
    Write picture data.
  */
  count=0;
  if (storage_class == PseudoClass)
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        break;
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        scanline[x]=(unsigned char) GetPixelIndex(image,p);
        p+=GetPixelChannels(image);
      }
      count+=EncodeImage(image,scanline,(size_t) (row_bytes & 0x7FFF),
        packed_scanline);
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
    }
  else
    if (image_info->compression == JPEGCompression)
      {
        (void) ResetMagickMemory(scanline,0,row_bytes);
        for (y=0; y < (ssize_t) image->rows; y++)
          count+=EncodeImage(image,scanline,(size_t) (row_bytes & 0x7FFF),
            packed_scanline);
      }
    else
      {
        register unsigned char
          *blue,
          *green,
          *opacity,
          *red;

        red=scanline;
        green=scanline+image->columns;
        blue=scanline+2*image->columns;
        opacity=scanline+3*image->columns;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          p=GetVirtualPixels(image,0,y,image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          red=scanline;
          green=scanline+image->columns;
          blue=scanline+2*image->columns;
          if (image->alpha_trait != UndefinedPixelTrait)
            {
              opacity=scanline;
              red=scanline+image->columns;
              green=scanline+2*image->columns;
              blue=scanline+3*image->columns;
            }
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *red++=ScaleQuantumToChar(GetPixelRed(image,p));
            *green++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *blue++=ScaleQuantumToChar(GetPixelBlue(image,p));
            if (image->alpha_trait != UndefinedPixelTrait)
              *opacity++=ScaleQuantumToChar((Quantum) (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
          }
          count+=EncodeImage(image,scanline,bytes_per_line,packed_scanline);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
      }
  if ((count & 0x01) != 0)
    (void) WriteBlobByte(image,'\0');
  (void) WriteBlobMSBShort(image,PictEndOfPictureOp);
  offset=TellBlob(image);
  offset=SeekBlob(image,512,SEEK_SET);
  (void) WriteBlobMSBShort(image,(unsigned short) offset);
  scanline=(unsigned char *) RelinquishMagickMemory(scanline);
  packed_scanline=(unsigned char *) RelinquishMagickMemory(packed_scanline);
  buffer=(unsigned char *) RelinquishMagickMemory(buffer);
  (void) CloseBlob(image);
  return(MagickTrue);
}