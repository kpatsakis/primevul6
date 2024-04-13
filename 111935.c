static void ParseAdditionalInfo(LayerInfo *layer_info)
{
  char
    key[5];

  size_t
    remaining_length;

  unsigned char
    *p;

  unsigned int
    size;

  p=GetStringInfoDatum(layer_info->info);
  remaining_length=GetStringInfoLength(layer_info->info);
  while (remaining_length >= 12)
  {
    /* skip over signature */
    p+=4;
    key[0]=(char) (*p++);
    key[1]=(char) (*p++);
    key[2]=(char) (*p++);
    key[3]=(char) (*p++);
    key[4]='\0';
    size=(unsigned int) (*p++) << 24;
    size|=(unsigned int) (*p++) << 16;
    size|=(unsigned int) (*p++) << 8;
    size|=(unsigned int) (*p++);
    size=size & 0xffffffff;
    remaining_length-=12;
    if ((size_t) size > remaining_length)
      break;
    if (LocaleNCompare(key,"luni",sizeof(key)) == 0)
      {
        unsigned char
          *name;

        unsigned int
          length;

        length=(unsigned int) (*p++) << 24;
        length|=(unsigned int) (*p++) << 16;
        length|=(unsigned int) (*p++) << 8;
        length|=(unsigned int) (*p++);
        if (length * 2 > size - 4)
          break;
        if (sizeof(layer_info->name) <= length)
          break;
        name=layer_info->name;
        while (length > 0)
        {
          /* Only ASCII strings are supported */
          if (*p++ != '\0')
            break;
          *name++=*p++;
          length--;
        }
        if (length == 0)
          *name='\0';
        break;
      }
    else
      p+=size;
    remaining_length-=(size_t) size;
  }
}