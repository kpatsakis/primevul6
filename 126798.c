static int UnpackWPG2Raster(Image *image,int bpp,ExceptionInfo *exception)
{
  int
    RunCount,
    XorMe = 0;

  size_t
    x,
    y;

  ssize_t
    i,
    ldblk;

  unsigned int
    SampleSize=1;

  unsigned char
    bbuf,
    *BImgBuff,
    SampleBuffer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  x=0;
  y=0;
  ldblk=(ssize_t) ((bpp*image->columns+7)/8);
  BImgBuff=(unsigned char *) AcquireQuantumMemory((size_t) ldblk,
    sizeof(*BImgBuff));
  if(BImgBuff==NULL)
    return(-2);

  while( y< image->rows)
    {
      bbuf=ReadBlobByte(image);

      switch(bbuf)
        {
        case 0x7D:
          SampleSize=ReadBlobByte(image);  /* DSZ */
          if(SampleSize>8)
            {
              BImgBuff=(unsigned char *) RelinquishMagickMemory(BImgBuff);
              return(-2);
            }
          if(SampleSize<1)
            {
              BImgBuff=(unsigned char *) RelinquishMagickMemory(BImgBuff);
              return(-2);
            }
          break;
        case 0x7E:
          (void) FormatLocaleFile(stderr,
            "\nUnsupported WPG token XOR, please report!");
          XorMe=!XorMe;
          break;
        case 0x7F:
          RunCount=ReadBlobByte(image);   /* BLK */
          if (RunCount < 0)
            break;
          for(i=0; i < SampleSize*(RunCount+1); i++)
            {
              InsertByte6(0);
            }
          break;
        case 0xFD:
          RunCount=ReadBlobByte(image);   /* EXT */
          if (RunCount < 0)
            break;
          for(i=0; i<= RunCount;i++)
            for(bbuf=0; bbuf < SampleSize; bbuf++)
              InsertByte6(SampleBuffer[bbuf]);          
          break;
        case 0xFE:
          RunCount=ReadBlobByte(image);  /* RST */
          if (RunCount < 0)
            break;
          if(x!=0)
            {
              (void) FormatLocaleFile(stderr,
                "\nUnsupported WPG2 unaligned token RST x=%.20g, please report!\n"
                ,(double) x);
              BImgBuff=(unsigned char *) RelinquishMagickMemory(BImgBuff);
              return(-3);
            }
          {
            /* duplicate the previous row RunCount x */
            for(i=0;i<=RunCount;i++)
              {      
                InsertRow(image,BImgBuff,(ssize_t) (image->rows >= y ? y : image->rows-1),
                          bpp,exception);
                y++;
              }    
          }
          break;
        case 0xFF:
          RunCount=ReadBlobByte(image);   /* WHT */
          if (RunCount < 0)
            break;
          for(i=0; i < SampleSize*(RunCount+1); i++)
            {
              InsertByte6(0xFF);
            }
          break;
        default:
          RunCount=bbuf & 0x7F;

          if(bbuf & 0x80)     /* REP */
            {  
              for(i=0; i < SampleSize; i++)
                SampleBuffer[i]=ReadBlobByte(image);
              for(i=0;i<=RunCount;i++)
                for(bbuf=0;bbuf<SampleSize;bbuf++)
                  InsertByte6(SampleBuffer[bbuf]);
            }
          else {      /* NRP */
            for(i=0; i< SampleSize*(RunCount+1);i++)
              {
                bbuf=ReadBlobByte(image);
                InsertByte6(bbuf);
              }
          }
        }
      if (EOFBlob(image) != MagickFalse)
        break;
    }
  BImgBuff=(unsigned char *) RelinquishMagickMemory(BImgBuff);
  return(0);
}