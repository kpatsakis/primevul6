static int format8BIM(Image *ifile, Image *ofile)
{
  char
    temp[MagickPathExtent];

  unsigned int
    foundOSType;

  int
    ID,
    resCount,
    i,
    c;

  ssize_t
    count;

  unsigned char
    *PString,
    *str;

  resCount=0;
  foundOSType=0; /* found the OSType */
  (void) foundOSType;
  c=ReadBlobByte(ifile);
  while (c != EOF)
  {
    if (c == '8')
      {
        unsigned char
          buffer[5];

        buffer[0]=(unsigned char) c;
        for (i=1; i<4; i++)
        {
          c=ReadBlobByte(ifile);
          if (c == EOF)
            return(-1);
          buffer[i] = (unsigned char) c;
        }
        buffer[4]=0;
        if (strcmp((const char *)buffer, "8BIM") == 0)
          foundOSType=1;
        else
          continue;
      }
    else
      {
        c=ReadBlobByte(ifile);
        continue;
      }
    /*
      We found the OSType (8BIM) and now grab the ID, PString, and Size fields.
    */
    ID=(int) ReadBlobMSBShort(ifile);
    if (ID < 0)
      return(-1);
    {
      unsigned char
        plen;

      c=ReadBlobByte(ifile);
      if (c == EOF)
        return(-1);
      plen = (unsigned char) c;
      PString=(unsigned char *) AcquireQuantumMemory((size_t) (plen+
        MagickPathExtent),sizeof(*PString));
      if (PString == (unsigned char *) NULL)
        {
          printf("MemoryAllocationFailed");
          return 0;
        }
      for (i=0; i<plen; i++)
      {
        c=ReadBlobByte(ifile);
        if (c == EOF) return -1;
        PString[i] = (unsigned char) c;
      }
      PString[ plen ] = 0;
      if ((plen & 0x01) == 0)
      {
        c=ReadBlobByte(ifile);
        if (c == EOF)
          return(-1);
      }
    }
    count = (int) ReadBlobMSBLong(ifile);
    if (count < 0) return -1;
    /* make a buffer to hold the datand snag it from the input stream */
    str=(unsigned char *) AcquireQuantumMemory((size_t) count,sizeof(*str));
    if (str == (unsigned char *) NULL)
      {
        printf("MemoryAllocationFailed");
        return 0;
      }
    for (i=0; i < (ssize_t) count; i++)
    {
      c=ReadBlobByte(ifile);
      if (c == EOF)
        return(-1);
      str[i]=(unsigned char) c;
    }

    /* we currently skip thumbnails, since it does not make
     * any sense preserving them in a real world application
     */
    if (ID != THUMBNAIL_ID)
      {
        /* now finish up by formatting this binary data into
         * ASCII equivalent
         */
        if (strlen((const char *)PString) > 0)
          (void) FormatLocaleString(temp,MagickPathExtent,"8BIM#%d#%s=",ID,
            PString);
        else
          (void) FormatLocaleString(temp,MagickPathExtent,"8BIM#%d=",ID);
        (void) WriteBlobString(ofile,temp);
        if (ID == IPTC_ID)
          {
            formatString(ofile, "IPTC", 4);
            formatIPTCfromBuffer(ofile, (char *)str, (ssize_t) count);
          }
        else
          formatString(ofile, (char *)str, (ssize_t) count);
      }
    str=(unsigned char *) RelinquishMagickMemory(str);
    PString=(unsigned char *) RelinquishMagickMemory(PString);
    resCount++;
    c=ReadBlobByte(ifile);
  }
  return resCount;
}