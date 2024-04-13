static Image *ReadXWDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define CheckOverflowException(length,width,height) \
  (((height) != 0) && ((length)/((size_t) height) != ((size_t) width)))

  char
    *comment;

  Image
    *image;

  int
    x_status;

  MagickBooleanType
    authentic_colormap;

  MagickStatusType
    status;

  Quantum
    index;

  register ssize_t
    x;

  register Quantum
    *q;

  register ssize_t
    i;

  register size_t
    pixel;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned long
    lsb_first;

  XColor
    *colors;

  XImage
    *ximage;

  XWDFileHeader
    header;

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
     Read in header information.
  */
  count=ReadBlob(image,sz_XWDheader,(unsigned char *) &header);
  if (count != sz_XWDheader)
    ThrowReaderException(CorruptImageError,"UnableToReadImageHeader");
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if ((int) (*(char *) &lsb_first) != 0)
    MSBOrderLong((unsigned char *) &header,sz_XWDheader);
  /*
    Check to see if the dump file is in the proper format.
  */
  if (header.file_version != XWD_FILE_VERSION)
    ThrowReaderException(CorruptImageError,"FileFormatVersionMismatch");
  if (header.header_size < sz_XWDheader)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if ((header.bits_per_pixel == 0) || (header.bits_per_pixel > 32))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (((header.bitmap_pad % 8) != 0) || (header.bitmap_pad > 32))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (header.bitmap_unit > 32)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  if (header.ncolors > 256)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  switch (header.visual_class)
  {
    case StaticGray:
    case GrayScale:
    case StaticColor:
    case PseudoColor:
    case TrueColor:
    case DirectColor:
      break;
    default:
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  }
  switch (header.pixmap_format)
  {
    case XYBitmap:
    case XYPixmap:
    case ZPixmap:
      break;
    default:
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  }
  length=(size_t) (header.header_size-sz_XWDheader);
  comment=(char *) AcquireQuantumMemory(length+1,sizeof(*comment));
  if (comment == (char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  count=ReadBlob(image,length,(unsigned char *) comment);
  comment[length]='\0';
  (void) SetImageProperty(image,"comment",comment,exception);
  comment=DestroyString(comment);
  if (count != (ssize_t) length)
    ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
  /*
    Initialize the X image.
  */
  ximage=(XImage *) AcquireMagickMemory(sizeof(*ximage));
  if (ximage == (XImage *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  ximage->depth=(int) header.pixmap_depth;
  ximage->format=(int) header.pixmap_format;
  ximage->xoffset=(int) header.xoffset;
  ximage->data=(char *) NULL;
  ximage->width=(int) header.pixmap_width;
  ximage->height=(int) header.pixmap_height;
  ximage->bitmap_pad=(int) header.bitmap_pad;
  ximage->bytes_per_line=(int) header.bytes_per_line;
  ximage->byte_order=(int) header.byte_order;
  ximage->bitmap_unit=(int) header.bitmap_unit;
  ximage->bitmap_bit_order=(int) header.bitmap_bit_order;
  ximage->bits_per_pixel=(int) header.bits_per_pixel;
  ximage->red_mask=header.red_mask;
  ximage->green_mask=header.green_mask;
  ximage->blue_mask=header.blue_mask;
  if ((ximage->width < 0) || (ximage->height < 0) || (ximage->depth < 0) ||
      (ximage->format < 0) || (ximage->byte_order < 0) ||
      (ximage->bitmap_bit_order < 0) || (ximage->bitmap_pad < 0) ||
      (ximage->bytes_per_line < 0))
    {
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
  if ((ximage->width > 65535) || (ximage->height > 65535))
    {
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
  if ((ximage->bits_per_pixel > 32) || (ximage->bitmap_unit > 32))
    {
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
  x_status=XInitImage(ximage);
  if (x_status == 0)
    {
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
    }
  /*
    Read colormap.
  */
  authentic_colormap=MagickFalse;
  colors=(XColor *) NULL;
  if (header.ncolors != 0)
    {
      XWDColor
        color;

      length=(size_t) header.ncolors;
      if (length > ((~0UL)/sizeof(*colors)))
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      colors=(XColor *) AcquireQuantumMemory(length,sizeof(*colors));
      if (colors == (XColor *) NULL)
        {
          ximage=(XImage *) RelinquishMagickMemory(ximage);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      for (i=0; i < (ssize_t) header.ncolors; i++)
      {
        count=ReadBlob(image,sz_XWDColor,(unsigned char *) &color);
        if (count != sz_XWDColor)
          {
            colors=(XColor *) RelinquishMagickMemory(colors);
            ximage=(XImage *) RelinquishMagickMemory(ximage);
            ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
          }
        colors[i].pixel=color.pixel;
        colors[i].red=color.red;
        colors[i].green=color.green;
        colors[i].blue=color.blue;
        colors[i].flags=(char) color.flags;
        if (color.flags != 0)
          authentic_colormap=MagickTrue;
      }
      /*
        Ensure the header byte-order is most-significant byte first.
      */
      lsb_first=1;
      if ((int) (*(char *) &lsb_first) != 0)
        for (i=0; i < (ssize_t) header.ncolors; i++)
        {
          MSBOrderLong((unsigned char *) &colors[i].pixel,
            sizeof(colors[i].pixel));
          MSBOrderShort((unsigned char *) &colors[i].red,3*
            sizeof(colors[i].red));
        }
    }
  /*
    Allocate the pixel buffer.
  */
  length=(size_t) ximage->bytes_per_line*ximage->height;
  if (CheckOverflowException(length,ximage->bytes_per_line,ximage->height))
    {
      if (header.ncolors != 0)
        colors=(XColor *) RelinquishMagickMemory(colors);
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    }
  if (ximage->format != ZPixmap)
    {
      size_t
        extent;

      extent=length;
      length*=ximage->depth;
      if (CheckOverflowException(length,extent,ximage->depth))
        {
          if (header.ncolors != 0)
            colors=(XColor *) RelinquishMagickMemory(colors);
          ximage=(XImage *) RelinquishMagickMemory(ximage);
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        }
    }
  ximage->data=(char *) AcquireQuantumMemory(length,sizeof(*ximage->data));
  if (ximage->data == (char *) NULL)
    {
      if (header.ncolors != 0)
        colors=(XColor *) RelinquishMagickMemory(colors);
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  count=ReadBlob(image,length,(unsigned char *) ximage->data);
  if (count != (ssize_t) length)
    {
      if (header.ncolors != 0)
        colors=(XColor *) RelinquishMagickMemory(colors);
      ximage->data=DestroyString(ximage->data);
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      ThrowReaderException(CorruptImageError,"UnableToReadImageData");
    }
  /*
    Convert image to MIFF format.
  */
  image->columns=(size_t) ximage->width;
  image->rows=(size_t) ximage->height;
  image->depth=8;
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    {
      if (header.ncolors != 0)
        colors=(XColor *) RelinquishMagickMemory(colors);
      ximage->data=DestroyString(ximage->data);
      ximage=(XImage *) RelinquishMagickMemory(ximage);
      return(DestroyImageList(image));
    }
  if ((header.ncolors == 0U) || (ximage->red_mask != 0) ||
      (ximage->green_mask != 0) || (ximage->blue_mask != 0))
    image->storage_class=DirectClass;
  else
    image->storage_class=PseudoClass;
  image->colors=header.ncolors;
  if (image_info->ping == MagickFalse)
    switch (image->storage_class)
    {
      case DirectClass:
      default:
      {
        register size_t
          color;

        size_t
          blue_mask,
          blue_shift,
          green_mask,
          green_shift,
          red_mask,
          red_shift;

        /*
          Determine shift and mask for red, green, and blue.
        */
        red_mask=ximage->red_mask;
        red_shift=0;
        while ((red_mask != 0) && ((red_mask & 0x01) == 0))
        {
          red_mask>>=1;
          red_shift++;
        }
        green_mask=ximage->green_mask;
        green_shift=0;
        while ((green_mask != 0) && ((green_mask & 0x01) == 0))
        {
          green_mask>>=1;
          green_shift++;
        }
        blue_mask=ximage->blue_mask;
        blue_shift=0;
        while ((blue_mask != 0) && ((blue_mask & 0x01) == 0))
        {
          blue_mask>>=1;
          blue_shift++;
        }
        /*
          Convert X image to DirectClass packets.
        */
        if ((image->colors != 0) && (authentic_colormap != MagickFalse))
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
            if (q == (Quantum *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              pixel=XGetPixel(ximage,(int) x,(int) y);
              index=(Quantum) ConstrainColormapIndex(image,(ssize_t) (pixel >>
                red_shift) & red_mask,exception);
              SetPixelRed(image,ScaleShortToQuantum(
                colors[(ssize_t) index].red),q);
              index=(Quantum) ConstrainColormapIndex(image,(ssize_t) (pixel >>
                green_shift) & green_mask,exception);
              SetPixelGreen(image,ScaleShortToQuantum(
                colors[(ssize_t) index].green),q);
              index=(Quantum) ConstrainColormapIndex(image,(ssize_t) (pixel >>
                blue_shift) & blue_mask,exception);
              SetPixelBlue(image,ScaleShortToQuantum(
                colors[(ssize_t) index].blue),q);
              q+=GetPixelChannels(image);
            }
            if (SyncAuthenticPixels(image,exception) == MagickFalse)
              break;
            status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
              image->rows);
            if (status == MagickFalse)
              break;
          }
        else
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
            if (q == (Quantum *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              pixel=XGetPixel(ximage,(int) x,(int) y);
              color=(pixel >> red_shift) & red_mask;
              if (red_mask != 0)
                color=(color*65535UL)/red_mask;
              SetPixelRed(image,ScaleShortToQuantum((unsigned short) color),q);
              color=(pixel >> green_shift) & green_mask;
              if (green_mask != 0)
                color=(color*65535UL)/green_mask;
              SetPixelGreen(image,ScaleShortToQuantum((unsigned short) color),
                q);
              color=(pixel >> blue_shift) & blue_mask;
              if (blue_mask != 0)
                color=(color*65535UL)/blue_mask;
              SetPixelBlue(image,ScaleShortToQuantum((unsigned short) color),q);
              q+=GetPixelChannels(image);
            }
            if (SyncAuthenticPixels(image,exception) == MagickFalse)
              break;
            status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
              image->rows);
            if (status == MagickFalse)
              break;
          }
        break;
      }
      case PseudoClass:
      {
        /*
          Convert X image to PseudoClass packets.
        */
        if (AcquireImageColormap(image,image->colors,exception) == MagickFalse)
          {
            if (header.ncolors != 0)
              colors=(XColor *) RelinquishMagickMemory(colors);
            ximage->data=DestroyString(ximage->data);
            ximage=(XImage *) RelinquishMagickMemory(ximage);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          image->colormap[i].red=(MagickRealType) ScaleShortToQuantum(
            colors[i].red);
          image->colormap[i].green=(MagickRealType) ScaleShortToQuantum(
            colors[i].green);
          image->colormap[i].blue=(MagickRealType) ScaleShortToQuantum(
            colors[i].blue);
        }
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            index=(Quantum) ConstrainColormapIndex(image,(ssize_t)
              XGetPixel(ximage,(int) x,(int) y),exception);
            SetPixelIndex(image,index,q);
            SetPixelViaPixelInfo(image,image->colormap+(ssize_t) index,q);
            q+=GetPixelChannels(image);
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
        break;
      }
    }
  /*
    Free image and colormap.
  */
  if (header.ncolors != 0)
    colors=(XColor *) RelinquishMagickMemory(colors);
  ximage->data=DestroyString(ximage->data);
  ximage=(XImage *) RelinquishMagickMemory(ximage);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}