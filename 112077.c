static void WritePascalString(Image* inImage,const char *inString,int inPad)
{
  size_t
    length;

  register ssize_t
    i;

  /*
    Max length is 255.
  */
  length=(strlen(inString) > 255UL ) ? 255UL : strlen(inString);
  if (length ==  0)
    (void) WriteBlobByte(inImage,0);
  else
    {
      (void) WriteBlobByte(inImage,(unsigned char) length);
      (void) WriteBlob(inImage, length, (const unsigned char *) inString);
    }
  length++;
  if ((length % inPad) == 0)
    return;
  for (i=0; i < (ssize_t) (inPad-(length % inPad)); i++)
    (void) WriteBlobByte(inImage,0);
}