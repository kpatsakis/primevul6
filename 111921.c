static MagickBooleanType ReadPSDChannelZip(Image *image,
  const PixelChannel channel,const PSDCompressionType compression,
  const size_t compact_size,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  unsigned char
    *p;

  size_t
    count,
    packet_size,
    row_size;

  ssize_t
    y;

  unsigned char
    *compact_pixels,
    *pixels;

  z_stream
    stream;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
       "      layer data is ZIP compressed");

  if ((MagickSizeType) compact_size > GetBlobSize(image))
    ThrowBinaryException(CorruptImageError,"UnexpectedEndOfFile",
      image->filename);
  compact_pixels=(unsigned char *) AcquireQuantumMemory(compact_size,
    sizeof(*compact_pixels));
  if (compact_pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);

  packet_size=GetPSDPacketSize(image);
  row_size=image->columns*packet_size;
  count=image->rows*row_size;

  pixels=(unsigned char *) AcquireQuantumMemory(count,sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    {
      compact_pixels=(unsigned char *) RelinquishMagickMemory(compact_pixels);
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        image->filename);
    }
  if (ReadBlob(image,compact_size,compact_pixels) != (ssize_t) compact_size)
    {
      pixels=(unsigned char *) RelinquishMagickMemory(pixels);
      compact_pixels=(unsigned char *) RelinquishMagickMemory(compact_pixels);
      ThrowBinaryException(CorruptImageError,"UnexpectedEndOfFile",
        image->filename);
    }

  memset(&stream,0,sizeof(stream));
  stream.data_type=Z_BINARY;
  stream.next_in=(Bytef *)compact_pixels;
  stream.avail_in=(uInt) compact_size;
  stream.next_out=(Bytef *)pixels;
  stream.avail_out=(uInt) count;

  if (inflateInit(&stream) == Z_OK)
    {
      int
        ret;

      while (stream.avail_out > 0)
      {
        ret=inflate(&stream,Z_SYNC_FLUSH);
        if ((ret != Z_OK) && (ret != Z_STREAM_END))
          {
            (void) inflateEnd(&stream);
            compact_pixels=(unsigned char *) RelinquishMagickMemory(
              compact_pixels);
            pixels=(unsigned char *) RelinquishMagickMemory(pixels);
            return(MagickFalse);
          }
        if (ret == Z_STREAM_END)
          break;
      }
      (void) inflateEnd(&stream);
    }

  if (compression == ZipWithPrediction)
    {
      if (packet_size == 1)
        Unpredict8Bit(image,pixels,count,row_size);
      else if (packet_size == 2)
        Unpredict16Bit(image,pixels,count,row_size);
      else if (packet_size == 4)
      {
        unsigned char
          *output_pixels;

        output_pixels=(unsigned char *) AcquireQuantumMemory(count,
          sizeof(*output_pixels));
        if (pixels == (unsigned char *) NULL)
          {
            compact_pixels=(unsigned char *) RelinquishMagickMemory(
              compact_pixels);
            pixels=(unsigned char *) RelinquishMagickMemory(pixels);
            ThrowBinaryException(ResourceLimitError,
              "MemoryAllocationFailed",image->filename);
          }
        Unpredict32Bit(image,pixels,output_pixels,row_size);
        pixels=(unsigned char *) RelinquishMagickMemory(pixels);
        pixels=output_pixels;
      }
    }

  status=MagickTrue;
  p=pixels;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    status=ReadPSDChannelPixels(image,y,channel,p,exception);
    if (status == MagickFalse)
      break;

    p+=row_size;
  }

  compact_pixels=(unsigned char *) RelinquishMagickMemory(compact_pixels);
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  return(status);
}