static ssize_t parse8BIMW(Image *ifile, Image *ofile)
{
  char
    brkused,
    quoted,
    *line,
    *token,
    *newstr,
    *name;

  int
    state,
    next;

  unsigned char
    dataset;

  unsigned int
    recnum;

  int
    inputlen = MagickPathExtent;

  ssize_t
    savedolen = 0L,
    outputlen = 0L;

  MagickOffsetType
    savedpos,
    currentpos;

  TokenInfo
    *token_info;

  dataset = 0;
  recnum = 0;
  line=(char *) AcquireQuantumMemory((size_t) inputlen,sizeof(*line));
  if (line == (char *) NULL)
    return(-1);
  newstr = name = token = (char *) NULL;
  savedpos = 0;
  token_info=AcquireTokenInfo();
  while (super_fgets_w(&line,&inputlen,ifile) != NULL)
  {
    state=0;
    next=0;

    token=(char *) AcquireQuantumMemory((size_t) inputlen,sizeof(*token));
    if (token == (char *) NULL)
      break;
    newstr=(char *) AcquireQuantumMemory((size_t) inputlen,sizeof(*newstr));
    if (newstr == (char *) NULL)
      break;
    while (Tokenizer(token_info,0,token,(size_t) inputlen,line,"","=","\"",0,
      &brkused,&next,&quoted)==0)
    {
      if (state == 0)
        {
          int
            state,
            next;

          char
            brkused,
            quoted;

          state=0;
          next=0;
          while (Tokenizer(token_info,0,newstr,(size_t) inputlen,token,"","#",
            "",0,&brkused,&next,&quoted)==0)
          {
            switch (state)
            {
              case 0:
                if (strcmp(newstr,"8BIM")==0)
                  dataset = 255;
                else
                  dataset = (unsigned char) StringToLong(newstr);
                break;
              case 1:
                recnum=(unsigned int) StringToUnsignedLong(newstr);
                break;
              case 2:
                name=(char *) AcquireQuantumMemory(strlen(newstr)+MagickPathExtent,
                  sizeof(*name));
                if (name)
                  (void) CopyMagickString(name,newstr,strlen(newstr)+MagickPathExtent);
                break;
            }
            state++;
          }
        }
      else
        if (state == 1)
          {
            int
              next;

            ssize_t
              len;

            char
              brkused,
              quoted;

            next=0;
            len = (ssize_t) strlen(token);
            while (Tokenizer(token_info,0,newstr,(size_t) inputlen,token,"","&",
              "",0,&brkused,&next,&quoted)==0)
            {
              if (brkused && next > 0)
                {
                  char
                    *s = &token[next-1];

                  len -= (ssize_t) convertHTMLcodes(s,(int) strlen(s));
                }
            }

            if (dataset == 255)
              {
                unsigned char
                  nlen = 0;

                int
                  i;

                if (savedolen > 0)
                  {
                    MagickOffsetType
                      offset;

                    ssize_t diff = outputlen - savedolen;
                    currentpos = TellBlob(ofile);
                    if (currentpos < 0)
                      return(-1);
                    offset=SeekBlob(ofile,savedpos,SEEK_SET);
                    if (offset < 0)
                      return(-1);
                    (void) WriteBlobMSBLong(ofile,(unsigned int) diff);
                    offset=SeekBlob(ofile,currentpos,SEEK_SET);
                    if (offset < 0)
                      return(-1);
                    savedolen = 0L;
                  }
                if (outputlen & 1)
                  {
                    (void) WriteBlobByte(ofile,0x00);
                    outputlen++;
                  }
                (void) WriteBlobString(ofile,"8BIM");
                (void) WriteBlobMSBShort(ofile,(unsigned short) recnum);
                outputlen += 6;
                if (name)
                  nlen = (unsigned char) strlen(name);
                (void) WriteBlobByte(ofile,(unsigned char) nlen);
                outputlen++;
                for (i=0; i<nlen; i++)
                  (void) WriteBlobByte(ofile,(unsigned char) name[i]);
                outputlen += nlen;
                if ((nlen & 0x01) == 0)
                  {
                    (void) WriteBlobByte(ofile,0x00);
                    outputlen++;
                  }
                if (recnum != IPTC_ID)
                  {
                    (void) WriteBlobMSBLong(ofile,(unsigned int) len);
                    outputlen += 4;

                    next=0;
                    outputlen += len;
                    while (len--)
                      (void) WriteBlobByte(ofile,(unsigned char) token[next++]);

                    if (outputlen & 1)
                      {
                        (void) WriteBlobByte(ofile,0x00);
                        outputlen++;
                      }
                  }
                else
                  {
                    /* patch in a fake length for now and fix it later */
                    savedpos = TellBlob(ofile);
                    if (savedpos < 0)
                      return(-1);
                    (void) WriteBlobMSBLong(ofile,0xFFFFFFFFU);
                    outputlen += 4;
                    savedolen = outputlen;
                  }
              }
            else
              {
                if (len <= 0x7FFF)
                  {
                    (void) WriteBlobByte(ofile,0x1c);
                    (void) WriteBlobByte(ofile,dataset);
                    (void) WriteBlobByte(ofile,(unsigned char) (recnum & 0xff));
                    (void) WriteBlobMSBShort(ofile,(unsigned short) len);
                    outputlen += 5;
                    next=0;
                    outputlen += len;
                    while (len--)
                      (void) WriteBlobByte(ofile,(unsigned char) token[next++]);
                  }
              }
          }
      state++;
    }
    if (token != (char *) NULL)
      token=DestroyString(token);
    if (newstr != (char *) NULL)
      newstr=DestroyString(newstr);
    if (name != (char *) NULL)
      name=DestroyString(name);
  }
  token_info=DestroyTokenInfo(token_info);
  if (token != (char *) NULL)
    token=DestroyString(token);
  if (newstr != (char *) NULL)
    newstr=DestroyString(newstr);
  if (name != (char *) NULL)
    name=DestroyString(name);
  line=DestroyString(line);
  if (savedolen > 0)
    {
      MagickOffsetType
        offset;

      ssize_t diff = outputlen - savedolen;

      currentpos = TellBlob(ofile);
      if (currentpos < 0)
        return(-1);
      offset=SeekBlob(ofile,savedpos,SEEK_SET);
      if (offset < 0)
        return(-1);
      (void) WriteBlobMSBLong(ofile,(unsigned int) diff);
      offset=SeekBlob(ofile,currentpos,SEEK_SET);
      if (offset < 0)
        return(-1);
      savedolen = 0L;
    }
  return(outputlen);
}