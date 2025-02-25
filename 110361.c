static Image *ReadCINImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MonoColorType  1
#define RGBColorType  3

  char
    property[MaxTextExtent];

  CINInfo
    cin;

  const unsigned char
    *pixels;

  Image
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register ssize_t
    i;

  register PixelPacket
    *q;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
    magick[4];

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
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    File information.
  */
  offset=0;
  count=ReadBlob(image,4,magick);
  offset+=count;
  if ((count != 4) ||
      ((LocaleNCompare((char *) magick,"\200\052\137\327",4) != 0)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  memset(&cin,0,sizeof(cin));
  image->endian=(magick[0] == 0x80) && (magick[1] == 0x2a) &&
    (magick[2] == 0x5f) && (magick[3] == 0xd7) ? MSBEndian : LSBEndian;
  cin.file.image_offset=ReadBlobLong(image);
  offset+=4;
  cin.file.generic_length=ReadBlobLong(image);
  offset+=4;
  cin.file.industry_length=ReadBlobLong(image);
  offset+=4;
  cin.file.user_length=ReadBlobLong(image);
  offset+=4;
  cin.file.file_size=ReadBlobLong(image);
  offset+=4;
  offset+=ReadBlob(image,sizeof(cin.file.version),(unsigned char *)
    cin.file.version);
  (void) CopyMagickString(property,cin.file.version,sizeof(cin.file.version));
  (void) SetImageProperty(image,"dpx:file.version",property);
  offset+=ReadBlob(image,sizeof(cin.file.filename),(unsigned char *)
    cin.file.filename);
  (void) CopyMagickString(property,cin.file.filename,sizeof(cin.file.filename));
  (void) SetImageProperty(image,"dpx:file.filename",property);
  offset+=ReadBlob(image,sizeof(cin.file.create_date),(unsigned char *)
    cin.file.create_date);
  (void) CopyMagickString(property,cin.file.create_date,
    sizeof(cin.file.create_date));
  (void) SetImageProperty(image,"dpx:file.create_date",property);
  offset+=ReadBlob(image,sizeof(cin.file.create_time),(unsigned char *)
    cin.file.create_time);
  (void) CopyMagickString(property,cin.file.create_time,
     sizeof(cin.file.create_time));
  (void) SetImageProperty(image,"dpx:file.create_time",property);
  offset+=ReadBlob(image,sizeof(cin.file.reserve),(unsigned char *)
    cin.file.reserve);
  /*
    Image information.
  */
  cin.image.orientation=(unsigned char) ReadBlobByte(image);
  offset++;
  if (cin.image.orientation != (unsigned char) (~0))
    (void) FormatImageProperty(image,"dpx:image.orientation","%d",
      cin.image.orientation);
  switch (cin.image.orientation)
  {
    default:
    case 0: image->orientation=TopLeftOrientation; break;
    case 1: image->orientation=TopRightOrientation; break;
    case 2: image->orientation=BottomLeftOrientation; break;
    case 3: image->orientation=BottomRightOrientation; break;
    case 4: image->orientation=LeftTopOrientation; break;
    case 5: image->orientation=RightTopOrientation; break;
    case 6: image->orientation=LeftBottomOrientation; break;
    case 7: image->orientation=RightBottomOrientation; break;
  }
  cin.image.number_channels=(unsigned char) ReadBlobByte(image);
  offset++;
  offset+=ReadBlob(image,sizeof(cin.image.reserve1),(unsigned char *)
    cin.image.reserve1);
  for (i=0; i < 8; i++)
  {
    cin.image.channel[i].designator[0]=(unsigned char) ReadBlobByte(image);
    offset++;
    cin.image.channel[i].designator[1]=(unsigned char) ReadBlobByte(image);
    offset++;
    cin.image.channel[i].bits_per_pixel=(unsigned char) ReadBlobByte(image);
    offset++;
    cin.image.channel[i].reserve=(unsigned char) ReadBlobByte(image);
    offset++;
    cin.image.channel[i].pixels_per_line=ReadBlobLong(image);
    offset+=4;
    cin.image.channel[i].lines_per_image=ReadBlobLong(image);
    offset+=4;
    cin.image.channel[i].min_data=ReadBlobFloat(image);
    offset+=4;
    cin.image.channel[i].min_quantity=ReadBlobFloat(image);
    offset+=4;
    cin.image.channel[i].max_data=ReadBlobFloat(image);
    offset+=4;
    cin.image.channel[i].max_quantity=ReadBlobFloat(image);
    offset+=4;
  }
  cin.image.white_point[0]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.white_point[0]) != MagickFalse)
    image->chromaticity.white_point.x=cin.image.white_point[0];
  cin.image.white_point[1]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.white_point[1]) != MagickFalse)
    image->chromaticity.white_point.y=cin.image.white_point[1];
  cin.image.red_primary_chromaticity[0]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.red_primary_chromaticity[0]) != MagickFalse)
    image->chromaticity.red_primary.x=cin.image.red_primary_chromaticity[0];
  cin.image.red_primary_chromaticity[1]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.red_primary_chromaticity[1]) != MagickFalse)
    image->chromaticity.red_primary.y=cin.image.red_primary_chromaticity[1];
  cin.image.green_primary_chromaticity[0]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.green_primary_chromaticity[0]) != MagickFalse)
    image->chromaticity.red_primary.x=cin.image.green_primary_chromaticity[0];
  cin.image.green_primary_chromaticity[1]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.green_primary_chromaticity[1]) != MagickFalse)
    image->chromaticity.green_primary.y=cin.image.green_primary_chromaticity[1];
  cin.image.blue_primary_chromaticity[0]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.blue_primary_chromaticity[0]) != MagickFalse)
    image->chromaticity.blue_primary.x=cin.image.blue_primary_chromaticity[0];
  cin.image.blue_primary_chromaticity[1]=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.image.blue_primary_chromaticity[1]) != MagickFalse)
    image->chromaticity.blue_primary.y=cin.image.blue_primary_chromaticity[1];
  offset+=ReadBlob(image,sizeof(cin.image.label),(unsigned char *)
    cin.image.label);
  (void) CopyMagickString(property,cin.image.label,sizeof(cin.image.label));
  (void) SetImageProperty(image,"dpx:image.label",property);
  offset+=ReadBlob(image,sizeof(cin.image.reserve),(unsigned char *)
    cin.image.reserve);
  /*
    Image data format information.
  */
  cin.data_format.interleave=(unsigned char) ReadBlobByte(image);
  offset++;
  cin.data_format.packing=(unsigned char) ReadBlobByte(image);
  offset++;
  cin.data_format.sign=(unsigned char) ReadBlobByte(image);
  offset++;
  cin.data_format.sense=(unsigned char) ReadBlobByte(image);
  offset++;
  cin.data_format.line_pad=ReadBlobLong(image);
  offset+=4;
  cin.data_format.channel_pad=ReadBlobLong(image);
  offset+=4;
  offset+=ReadBlob(image,sizeof(cin.data_format.reserve),(unsigned char *)
    cin.data_format.reserve);
  /*
    Image origination information.
  */
  cin.origination.x_offset=ReadBlobSignedLong(image);
  offset+=4;
  if ((size_t) cin.origination.x_offset != ~0UL)
    (void) FormatImageProperty(image,"dpx:origination.x_offset","%.20g",
      (double) cin.origination.x_offset);
  cin.origination.y_offset=(ssize_t) ReadBlobLong(image);
  offset+=4;
  if ((size_t) cin.origination.y_offset != ~0UL)
    (void) FormatImageProperty(image,"dpx:origination.y_offset","%.20g",
      (double) cin.origination.y_offset);
  offset+=ReadBlob(image,sizeof(cin.origination.filename),(unsigned char *)
    cin.origination.filename);
  (void) CopyMagickString(property,cin.origination.filename,
    sizeof(cin.origination.filename));
  (void) SetImageProperty(image,"dpx:origination.filename",property);
  offset+=ReadBlob(image,sizeof(cin.origination.create_date),(unsigned char *)
    cin.origination.create_date);
  (void) CopyMagickString(property,cin.origination.create_date,
    sizeof(cin.origination.create_date));
  (void) SetImageProperty(image,"dpx:origination.create_date",property);
  offset+=ReadBlob(image,sizeof(cin.origination.create_time),(unsigned char *)
    cin.origination.create_time);
  (void) CopyMagickString(property,cin.origination.create_time,
    sizeof(cin.origination.create_time));
  (void) SetImageProperty(image,"dpx:origination.create_time",property);
  offset+=ReadBlob(image,sizeof(cin.origination.device),(unsigned char *)
    cin.origination.device);
  (void) CopyMagickString(property,cin.origination.device,
    sizeof(cin.origination.device));
  (void) SetImageProperty(image,"dpx:origination.device",property);
  offset+=ReadBlob(image,sizeof(cin.origination.model),(unsigned char *)
    cin.origination.model);
  (void) CopyMagickString(property,cin.origination.model,
    sizeof(cin.origination.model));
  (void) SetImageProperty(image,"dpx:origination.model",property);
  (void) memset(cin.origination.serial,0,
    sizeof(cin.origination.serial));
  offset+=ReadBlob(image,sizeof(cin.origination.serial),(unsigned char *)
    cin.origination.serial);
  (void) CopyMagickString(property,cin.origination.serial,
    sizeof(cin.origination.serial));
  (void) SetImageProperty(image,"dpx:origination.serial",property);
  cin.origination.x_pitch=ReadBlobFloat(image);
  offset+=4;
  cin.origination.y_pitch=ReadBlobFloat(image);
  offset+=4;
  cin.origination.gamma=ReadBlobFloat(image);
  offset+=4;
  if (IsFloatDefined(cin.origination.gamma) != MagickFalse)
    image->gamma=cin.origination.gamma;
  offset+=ReadBlob(image,sizeof(cin.origination.reserve),(unsigned char *)
    cin.origination.reserve);
  if ((cin.file.image_offset > 2048) && (cin.file.user_length != 0))
    {
      int
        c;

      /*
        Image film information.
      */
      cin.film.id=ReadBlobByte(image);
      offset++;
      c=cin.film.id;
      if (c != ~0)
        (void) FormatImageProperty(image,"dpx:film.id","%d",cin.film.id);
      cin.film.type=ReadBlobByte(image);
      offset++;
      c=cin.film.type;
      if (c != ~0)
        (void) FormatImageProperty(image,"dpx:film.type","%d",cin.film.type);
      cin.film.offset=ReadBlobByte(image);
      offset++;
      c=cin.film.offset;
      if (c != ~0)
        (void) FormatImageProperty(image,"dpx:film.offset","%d",
          cin.film.offset);
      cin.film.reserve1=ReadBlobByte(image);
      offset++;
      cin.film.prefix=ReadBlobLong(image);
      offset+=4;
      if (cin.film.prefix != ~0UL)
        (void) FormatImageProperty(image,"dpx:film.prefix","%.20g",(double)
          cin.film.prefix);
      cin.film.count=ReadBlobLong(image);
      offset+=4;
      offset+=ReadBlob(image,sizeof(cin.film.format),(unsigned char *)
        cin.film.format);
      (void) CopyMagickString(property,cin.film.format,
        sizeof(cin.film.format));
      (void) SetImageProperty(image,"dpx:film.format",property);
      cin.film.frame_position=ReadBlobLong(image);
      offset+=4;
      if (cin.film.frame_position != ~0UL)
        (void) FormatImageProperty(image,"dpx:film.frame_position","%.20g",
          (double) cin.film.frame_position);
      cin.film.frame_rate=ReadBlobFloat(image);
      offset+=4;
      if (IsFloatDefined(cin.film.frame_rate) != MagickFalse)
        (void) FormatImageProperty(image,"dpx:film.frame_rate","%g",
          cin.film.frame_rate);
      offset+=ReadBlob(image,sizeof(cin.film.frame_id),(unsigned char *)
        cin.film.frame_id);
      (void) CopyMagickString(property,cin.film.frame_id,
        sizeof(cin.film.frame_id));
      (void) SetImageProperty(image,"dpx:film.frame_id",property);
      offset+=ReadBlob(image,sizeof(cin.film.slate_info),(unsigned char *)
        cin.film.slate_info);
      (void) CopyMagickString(property,cin.film.slate_info,
        sizeof(cin.film.slate_info));
      (void) SetImageProperty(image,"dpx:film.slate_info",property);
      offset+=ReadBlob(image,sizeof(cin.film.reserve),(unsigned char *)
        cin.film.reserve);
    }
  if ((cin.file.image_offset > 2048) && (cin.file.user_length != 0))
    {
      StringInfo
        *profile;

      /*
        User defined data.
      */
      if (cin.file.user_length > GetBlobSize(image))
        ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
      profile=BlobToStringInfo((const void *) NULL,cin.file.user_length);
      if (profile == (StringInfo *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      offset+=ReadBlob(image,GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
      (void) SetImageProfile(image,"dpx:user.data",profile);
      profile=DestroyStringInfo(profile);
    }
  image->depth=cin.image.channel[0].bits_per_pixel;
  image->columns=cin.image.channel[0].pixels_per_line;
  image->rows=cin.image.channel[0].lines_per_image;
  if (image_info->ping != MagickFalse)
    {
      (void) CloseBlob(image);
      return(image);
    }
  for ( ; offset < (MagickOffsetType) cin.file.image_offset; offset++)
  {
    int
      c;

    c=ReadBlobByte(image);
    if (c == EOF)
      break;
  }
  if (offset < (MagickOffsetType) cin.file.image_offset)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
  (void) SetImageBackgroundColor(image);
  /*
    Convert CIN raster image to pixel packets.
  */
  quantum_info=AcquireQuantumInfo(image_info,image);
  if (quantum_info == (QuantumInfo *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  quantum_info->quantum=32;
  quantum_info->pack=MagickFalse;
  quantum_type=RGBQuantum;
  length=GetQuantumExtent(image,quantum_info,quantum_type);
  length=GetBytesPerRow(image->columns,3,image->depth,MagickTrue);
  if (cin.image.number_channels == 1)
    {
      quantum_type=GrayQuantum;
      length=GetBytesPerRow(image->columns,1,image->depth,MagickTrue);
    }
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    pixels=(const unsigned char *) ReadBlobStream(image,length,
      GetQuantumPixels(quantum_info),&count);
    if (count != (ssize_t) length)
      break;
    (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
      quantum_type,pixels,exception);
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
  SetQuantumImageType(image,quantum_type);
  quantum_info=DestroyQuantumInfo(quantum_info);
  if (EOFBlob(image) != MagickFalse)
    ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  SetImageColorspace(image,LogColorspace);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}