static Image *ReadMATImageV4(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  typedef struct {
    unsigned char Type[4];
    unsigned int nRows;
    unsigned int nCols;
    unsigned int imagf;
    unsigned int nameLen;
  } MAT4_HDR;

  long
    ldblk;

  EndianType
    endian;

  Image
    *rotate_image;

  MagickBooleanType
    status;

  MAT4_HDR
    HDR;

  QuantumInfo
    *quantum_info;

  QuantumFormatType
    format_type;

  register ssize_t
    i;

  ssize_t
    count,
    y;

  unsigned char
    *pixels;

  unsigned int
    depth;


  quantum_info=(QuantumInfo *) NULL;
  (void) SeekBlob(image,0,SEEK_SET);
  while (EOFBlob(image) != MagickFalse)
  {
    /*
     Object parser loop.
    */
    ldblk=ReadBlobLSBLong(image);
    if ((ldblk > 9999) || (ldblk < 0))
      break;
    HDR.Type[3]=ldblk % 10; ldblk /= 10;  /* T digit */
    HDR.Type[2]=ldblk % 10; ldblk /= 10;  /* P digit */
    HDR.Type[1]=ldblk % 10; ldblk /= 10;  /* O digit */
    HDR.Type[0]=ldblk;        /* M digit */
    if (HDR.Type[3] != 0)
      break;  /* Data format */
    if (HDR.Type[2] != 0)
      break;  /* Always 0 */
    if (HDR.Type[0] == 0)
      {
        HDR.nRows=ReadBlobLSBLong(image);
        HDR.nCols=ReadBlobLSBLong(image);
        HDR.imagf=ReadBlobLSBLong(image);
        HDR.nameLen=ReadBlobLSBLong(image);
        endian=LSBEndian;
      }
    else
      {
        HDR.nRows=ReadBlobMSBLong(image);
        HDR.nCols=ReadBlobMSBLong(image);
        HDR.imagf=ReadBlobMSBLong(image);
        HDR.nameLen=ReadBlobMSBLong(image);
        endian=MSBEndian;
      }
    if ((HDR.imagf != 0) && (HDR.imagf != 1))
      break;
    if (HDR.nameLen > 0xFFFF)
      return((Image *) NULL);
    for (i=0; i < (ssize_t) HDR.nameLen; i++)
    {
      int
        byte;

      /*
        Skip matrix name.
      */
      byte=ReadBlobByte(image);
      if (byte == EOF)
        {
          ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
            image->filename);
          break;
        }
    }
    image->columns=(size_t) HDR.nRows;
    image->rows=(size_t) HDR.nCols;
    SetImageColorspace(image,GRAYColorspace,exception);
    if (image_info->ping != MagickFalse)
      {
        Swap(image->columns,image->rows);
        return(image);
      }
    status=SetImageExtent(image,image->columns,image->rows,exception);
    if (status == MagickFalse)
      return((Image *) NULL);
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      return((Image *) NULL);
    switch(HDR.Type[1])
    {
      case 0:
        format_type=FloatingPointQuantumFormat;
        depth=64;
        break;
      case 1:
        format_type=FloatingPointQuantumFormat;
        depth=32;
        break;
      case 2:
        format_type=UnsignedQuantumFormat;
        depth=16;
        break;
      case 3:
        format_type=SignedQuantumFormat;
        depth=16;
        break;
      case 4:
        format_type=UnsignedQuantumFormat;
        depth=8;
        break;
      default:
        format_type=UnsignedQuantumFormat;
        depth=8;
        break;
    }
    image->depth=depth;
    if (HDR.Type[0] != 0)
      SetQuantumEndian(image,quantum_info,MSBEndian);
    status=SetQuantumFormat(image,quantum_info,format_type);
    status=SetQuantumDepth(image,quantum_info,depth);
    status=SetQuantumEndian(image,quantum_info,endian);
    SetQuantumScale(quantum_info,1.0);
    pixels=(unsigned char *) GetQuantumPixels(quantum_info);
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      register Quantum
        *magick_restrict q;

      count=ReadBlob(image,depth/8*image->columns,(char *) pixels);
      if (count == -1)
        break;
      q=QueueAuthenticPixels(image,0,image->rows-y-1,image->columns,1,
        exception);
      if (q == (Quantum *) NULL)
        break;
      (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
        GrayQuantum,pixels,exception);
      if ((HDR.Type[1] == 2) || (HDR.Type[1] == 3))
        FixSignedValues(image,q,(int) image->columns);
      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        break;
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
    }
    if (HDR.imagf == 1)
      for (y=0; y < (ssize_t) image->rows; y++)
      {
        /*
          Read complex pixels.
        */
        count=ReadBlob(image,depth/8*image->columns,(char *) pixels);
        if (count == -1)
          break;
        if (HDR.Type[1] == 0)
          InsertComplexDoubleRow(image,(double *) pixels,y,0,0,exception);
        else
          InsertComplexFloatRow(image,(float *) pixels,y,0,0,exception);
      }
    if (quantum_info != (QuantumInfo *) NULL)
      quantum_info=DestroyQuantumInfo(quantum_info);
    rotate_image=RotateImage(image,90.0,exception);
    if (rotate_image != (Image *) NULL)
      {
        image=DestroyImage(image);
        image=rotate_image;
      }
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    /*
      Allocate next image structure.
    */
    AcquireNextImage(image_info,image,exception);
    if (GetNextImageInList(image) == (Image *) NULL)
      {
        image=DestroyImageList(image);
        return((Image *) NULL);
      }
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
      GetBlobSize(image));
    if (status == MagickFalse)
      break;
  }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}