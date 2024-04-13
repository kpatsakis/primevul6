MagickExport const char *GetImageProperty(const Image *image,
  const char *property,ExceptionInfo *exception)
{
  MagickBooleanType
    read_from_properties;

  const char
    *p;

  size_t
    property_length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((property == (const char *) NULL) || (*property == '\0'))
    return((const char *) NULL);
  read_from_properties=MagickTrue;
  property_length=strlen(property);
  if ((property_length > 2) && (*(property+(property_length-2)) == ':') &&
      (*(property+(property_length-1)) == '*'))
    read_from_properties=MagickFalse;
  if (read_from_properties != MagickFalse)
    {
      p=(const char *) NULL;
      if (image->properties != (void *) NULL)
        {
          if (property == (const char *) NULL)
            return((const char *) GetRootValueFromSplayTree((SplayTreeInfo *)
              image->properties));
          p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
            image->properties,property);
          if (p != (const char *) NULL)
            return(p);
        }
      if ((property == (const char *) NULL) ||
          (strchr(property,':') == (char *) NULL))
        return(p);
    }
  switch (*property)
  {
    case '8':
    {
      if (LocaleNCompare("8bim:",property,5) == 0)
        {
          (void) Get8BIMProperty(image,property,exception);
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleNCompare("exif:",property,5) == 0)
        {
          (void) GetEXIFProperty(image,property,exception);
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if ((LocaleNCompare("icc:",property,4) == 0) ||
          (LocaleNCompare("icm:",property,4) == 0))
        {
          (void) GetICCProperty(image,property,exception);
          break;
        }
      if (LocaleNCompare("iptc:",property,5) == 0)
        {
          (void) GetIPTCProperty(image,property,exception);
          break;
        }
      break;
    }
    case 'X':
    case 'x':
    {
      if (LocaleNCompare("xmp:",property,4) == 0)
        {
          (void) GetXMPProperty(image,property);
          break;
        }
      break;
    }
    default:
      break;
  }
  if ((image->properties != (void *) NULL) &&
      (read_from_properties != MagickFalse))
    {
      p=(const char *) GetValueFromSplayTree((SplayTreeInfo *)
        image->properties,property);
      return(p);
    }
  return((const char *) NULL);
}