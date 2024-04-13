static inline void WriteProfileLong(const EndianType endian,
  const size_t value,unsigned char *p)
{
  unsigned char
    buffer[4];

  if (endian == LSBEndian)
    {
      buffer[0]=(unsigned char) value;
      buffer[1]=(unsigned char) (value >> 8);
      buffer[2]=(unsigned char) (value >> 16);
      buffer[3]=(unsigned char) (value >> 24);
      (void) CopyMagickMemory(p,buffer,4);
      return;
    }
  buffer[0]=(unsigned char) (value >> 24);
  buffer[1]=(unsigned char) (value >> 16);
  buffer[2]=(unsigned char) (value >> 8);
  buffer[3]=(unsigned char) value;
  (void) CopyMagickMemory(p,buffer,4);
}