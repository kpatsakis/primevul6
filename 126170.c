MagickExport char *InterpretImageProperties(ImageInfo *image_info,Image *image,
  const char *embed_text,ExceptionInfo *exception)
{
#define ExtendInterpretText(string_length) \
{ \
  size_t length=(string_length); \
  if ((size_t) (q-interpret_text+length+1) >= extent) \
    { \
      extent+=length; \
      interpret_text=(char *) ResizeQuantumMemory(interpret_text,extent+ \
        MagickPathExtent,sizeof(*interpret_text)); \
      if (interpret_text == (char *) NULL) \
        { \
          if (property_image != image) \
            property_image=DestroyImage(property_image); \
          if (property_info != image_info) \
            property_info=DestroyImageInfo(property_info); \
          return((char *) NULL); \
        } \
      q=interpret_text+strlen(interpret_text); \
   } \
}

#define AppendKeyValue2Text(key,value)\
{ \
  size_t length=strlen(key)+strlen(value)+2; \
  if ((size_t) (q-interpret_text+length+1) >= extent) \
    { \
      extent+=length; \
      interpret_text=(char *) ResizeQuantumMemory(interpret_text,extent+ \
        MagickPathExtent,sizeof(*interpret_text)); \
      if (interpret_text == (char *) NULL) \
        { \
          if (property_image != image) \
            property_image=DestroyImage(property_image); \
          if (property_info != image_info) \
            property_info=DestroyImageInfo(property_info); \
          return((char *) NULL); \
        } \
      q=interpret_text+strlen(interpret_text); \
     } \
   q+=FormatLocaleString(q,extent,"%s=%s\n",(key),(value)); \
}

#define AppendString2Text(string) \
{ \
  size_t length=strlen((string)); \
  if ((size_t) (q-interpret_text+length+1) >= extent) \
    { \
      extent+=length; \
      interpret_text=(char *) ResizeQuantumMemory(interpret_text,extent+ \
        MagickPathExtent,sizeof(*interpret_text)); \
      if (interpret_text == (char *) NULL) \
        { \
          if (property_image != image) \
            property_image=DestroyImage(property_image); \
          if (property_info != image_info) \
            property_info=DestroyImageInfo(property_info); \
          return((char *) NULL); \
        } \
      q=interpret_text+strlen(interpret_text); \
    } \
  (void) CopyMagickString(q,(string),extent); \
  q+=length; \
}

  char
    *interpret_text;

  Image
    *property_image;

  ImageInfo
    *property_info;

  MagickBooleanType
    number;

  char
    *q;  /* current position in interpret_text */

  const char
    *p;  /* position in embed_text string being expanded */

  size_t
    extent;  /* allocated length of interpret_text */

  if ((image != (Image *) NULL) && (image->debug != MagickFalse))
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  else
   if ((image_info != (ImageInfo *) NULL) && (image_info->debug != MagickFalse))
     (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
       image_info->filename);
   else
     (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s","no image");
  if (embed_text == (const char *) NULL)
    return(ConstantString(""));
  p=embed_text;
  while ((isspace((int) ((unsigned char) *p)) != 0) && (*p != '\0'))
    p++;
  if (*p == '\0')
    return(ConstantString(""));
  if ((*p == '@') && (IsPathAccessible(p+1) != MagickFalse))
    {
      /*
        Handle a '@' replace string from file.
      */
      if (IsRightsAuthorized(PathPolicyDomain,ReadPolicyRights,p) == MagickFalse)
        {
          errno=EPERM;
          (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
            "NotAuthorized","`%s'",p);
          return(ConstantString(""));
        }
      interpret_text=FileToString(p+1,~0UL,exception);
      if (interpret_text != (char *) NULL)
        return(interpret_text);
    }
  /*
    Translate any embedded format characters.
  */
  if (image_info != (ImageInfo *) NULL)
    property_info=image_info;
  else
    property_info=CloneImageInfo(image_info);
  if ((image != (Image *) NULL) && (image->columns != 0) && (image->rows != 0))
    property_image=image;
  else
    {
      property_image=AcquireImage(image_info,exception);
      (void) SetImageExtent(property_image,1,1,exception);
      (void) SetImageBackgroundColor(property_image,exception);
    }
  interpret_text=AcquireString(embed_text); /* new string with extra space */
  extent=MagickPathExtent;                  /* allocated space in string */
  number=MagickFalse;                       /* is last char a number? */
  for (q=interpret_text; *p!='\0'; number=isdigit((int) ((unsigned char) *p)) ? MagickTrue : MagickFalse,p++)
  {
    /*
      Look for the various escapes, (and handle other specials)
    */
    *q='\0';
    ExtendInterpretText(MagickPathExtent);
    switch (*p)
    {
      case '\\':
      {
        switch (*(p+1))
        {
          case '\0':
            continue;
          case 'r':  /* convert to RETURN */
          {
            *q++='\r';
            p++;
            continue;
          }
          case 'n':  /* convert to NEWLINE */
          {
            *q++='\n';
            p++;
            continue;
          }
          case '\n':  /* EOL removal UNIX,MacOSX */
          {
            p++;
            continue;
          }
          case '\r':  /* EOL removal DOS,Windows */
          {
            p++;
            if (*p == '\n') /* return-newline EOL */
              p++;
            continue;
          }
          default:
          {
            p++;
            *q++=(*p);
          }
        }
        continue;
      }
      case '&':
      {
        if (LocaleNCompare("&lt;",p,4) == 0)
          {
            *q++='<';
            p+=3;
          }
        else
          if (LocaleNCompare("&gt;",p,4) == 0)
            {
              *q++='>';
              p+=3;
            }
          else
            if (LocaleNCompare("&amp;",p,5) == 0)
              {
                *q++='&';
                p+=4;
              }
            else
              *q++=(*p);
        continue;
      }
      case '%':
        break;  /* continue to next set of handlers */
      default:
      {
        *q++=(*p);  /* any thing else is 'as normal' */
        continue;
      }
    }
    p++; /* advance beyond the percent */
    /*
      Doubled Percent - or percent at end of string.
    */
    if ((*p == '\0') || (*p == '\'') || (*p == '"'))
      p--;
    if (*p == '%')
      {
        *q++='%';
        continue;
      }
    /*
      Single letter escapes %c.
    */
    if (*p != '[')
      {
        const char
          *string;

        if (number != MagickFalse)
          {
            /*
              But only if not preceeded by a number!
            */
            *q++='%'; /* do NOT substitute the percent */
            p--;      /* back up one */
            continue;
          }
        string=GetMagickPropertyLetter(property_info,image,*p,exception);
        if (string != (char *) NULL)
          {
            AppendString2Text(string);
            (void) DeleteImageArtifact(property_image,"magick-property");
            (void) DeleteImageOption(property_info,"magick-property");
            continue;
          }
        (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
          "UnknownImageProperty","\"%%%c\"",*p);
        continue;
      }
    {
      char
        pattern[2*MagickPathExtent];

      const char
        *key,
        *string;

      ssize_t
        len;

      ssize_t
        depth;

      /*
        Braced Percent Escape %[...].
      */
      p++;  /* advance p to just inside the opening brace */
      depth=1;
      if (*p == ']')
        {
          (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
            "UnknownImageProperty","\"%%[]\"");
          break;
        }
      for (len=0; len < (MagickPathExtent-1L) && (*p != '\0'); )
      {
        if ((*p == '\\') && (*(p+1) != '\0'))
          {
            /*
              Skip escaped braces within braced pattern.
            */
            pattern[len++]=(*p++);
            pattern[len++]=(*p++);
            continue;
          }
        if (*p == '[')
          depth++;
        if (*p == ']')
          depth--;
        if (depth <= 0)
          break;
        pattern[len++]=(*p++);
      }
      pattern[len]='\0';
      if (depth != 0)
        {
          /*
            Check for unmatched final ']' for "%[...]".
          */
          if (len >= 64)
            {
              pattern[61] = '.';  /* truncate string for error message */
              pattern[62] = '.';
              pattern[63] = '.';
              pattern[64] = '\0';
            }
          (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "UnbalancedBraces","\"%%[%s\"",pattern);
          interpret_text=DestroyString(interpret_text);
          if (property_image != image)
            property_image=DestroyImage(property_image);
          if (property_info != image_info)
            property_info=DestroyImageInfo(property_info);
          return((char *) NULL);
        }
      /*
        Special Lookup Prefixes %[prefix:...].
      */
      if (LocaleNCompare("fx:",pattern,3) == 0)
        {
          double
            value;

          FxInfo
            *fx_info;

          MagickBooleanType
            status;

          /*
            FX - value calculator.
          */
          fx_info=AcquireFxInfo(property_image,pattern+3,exception);
          if (fx_info == (FxInfo *) NULL)
            continue;
          status=FxEvaluateChannelExpression(fx_info,CompositePixelChannel,0,0,
            &value,exception);
          fx_info=DestroyFxInfo(fx_info);
          if (status != MagickFalse)
            {
              char
                result[MagickPathExtent];

              (void) FormatLocaleString(result,MagickPathExtent,"%.*g",
                GetMagickPrecision(),(double) value);
              AppendString2Text(result);
            }
          continue;
        }
      if (LocaleNCompare("hex:",pattern,4) == 0)
        {
          double
            value;

          FxInfo
            *fx_info;

          MagickStatusType
            status;

          PixelInfo
            pixel;

          /*
            Pixel - color value calculator.
          */
          GetPixelInfo(property_image,&pixel);
          fx_info=AcquireFxInfo(property_image,pattern+4,exception);
          if (fx_info == (FxInfo *) NULL)
            continue;
          status=FxEvaluateChannelExpression(fx_info,RedPixelChannel,0,0,
            &value,exception);
          pixel.red=(double) QuantumRange*value;
          status&=FxEvaluateChannelExpression(fx_info,GreenPixelChannel,0,0,
            &value,exception);
          pixel.green=(double) QuantumRange*value;
          status&=FxEvaluateChannelExpression(fx_info,BluePixelChannel,0,0,
            &value,exception);
          pixel.blue=(double) QuantumRange*value;
          if (property_image->colorspace == CMYKColorspace)
            {
              status&=FxEvaluateChannelExpression(fx_info,BlackPixelChannel,0,0,
                &value,exception);
              pixel.black=(double) QuantumRange*value;
            }
          status&=FxEvaluateChannelExpression(fx_info,AlphaPixelChannel,0,0,
            &value,exception);
          pixel.alpha=(double) QuantumRange*value;
          fx_info=DestroyFxInfo(fx_info);
          if (status != MagickFalse)
            {
              char
                hex[MagickPathExtent];

              GetColorTuple(&pixel,MagickTrue,hex);
              AppendString2Text(hex+1);
            }
          continue;
        }
      if (LocaleNCompare("pixel:",pattern,6) == 0)
        {
          double
            value;

          FxInfo
            *fx_info;

          MagickStatusType
            status;

          PixelInfo
            pixel;

          /*
            Pixel - color value calculator.
          */
          GetPixelInfo(property_image,&pixel);
          fx_info=AcquireFxInfo(property_image,pattern+6,exception);
          if (fx_info == (FxInfo *) NULL)
            continue;
          status=FxEvaluateChannelExpression(fx_info,RedPixelChannel,0,0,
            &value,exception);
          pixel.red=(double) QuantumRange*value;
          status&=FxEvaluateChannelExpression(fx_info,GreenPixelChannel,0,0,
            &value,exception);
          pixel.green=(double) QuantumRange*value;
          status&=FxEvaluateChannelExpression(fx_info,BluePixelChannel,0,0,
            &value,exception);
          pixel.blue=(double) QuantumRange*value;
          if (property_image->colorspace == CMYKColorspace)
            {
              status&=FxEvaluateChannelExpression(fx_info,BlackPixelChannel,0,0,
                &value,exception);
              pixel.black=(double) QuantumRange*value;
            }
          status&=FxEvaluateChannelExpression(fx_info,AlphaPixelChannel,0,0,
            &value,exception);
          pixel.alpha=(double) QuantumRange*value;
          fx_info=DestroyFxInfo(fx_info);
          if (status != MagickFalse)
            {
              char
                name[MagickPathExtent];

              GetColorTuple(&pixel,MagickFalse,name);
              string=GetImageArtifact(property_image,"pixel:compliance");
              if (string != (char *) NULL)
                {
                  ComplianceType compliance=(ComplianceType) ParseCommandOption(
                    MagickComplianceOptions,MagickFalse,string);
                  (void) QueryColorname(property_image,&pixel,compliance,name,
                    exception);
                }
              AppendString2Text(name);
            }
          continue;
        }
      if (LocaleNCompare("option:",pattern,7) == 0)
        {
          /*
            Option - direct global option lookup (with globbing).
          */
          if (IsGlob(pattern+7) != MagickFalse)
            {
              ResetImageOptionIterator(property_info);
              while ((key=GetNextImageOption(property_info)) != (const char *) NULL)
                if (GlobExpression(key,pattern+7,MagickTrue) != MagickFalse)
                  {
                    string=GetImageOption(property_info,key);
                    if (string != (const char *) NULL)
                      AppendKeyValue2Text(key,string);
                    /* else - assertion failure? key found but no string value! */
                  }
              continue;
            }
          string=GetImageOption(property_info,pattern+7);
          if (string == (char *) NULL)
            goto PropertyLookupFailure; /* no artifact of this specifc name */
          AppendString2Text(string);
          continue;
        }
      if (LocaleNCompare("artifact:",pattern,9) == 0)
        {
          /*
            Artifact - direct image artifact lookup (with glob).
          */
          if (IsGlob(pattern+9) != MagickFalse)
            {
              ResetImageArtifactIterator(property_image);
              while ((key=GetNextImageArtifact(property_image)) != (const char *) NULL)
              if (GlobExpression(key,pattern+9,MagickTrue) != MagickFalse)
                {
                  string=GetImageArtifact(property_image,key);
                  if (string != (const char *) NULL)
                    AppendKeyValue2Text(key,string);
                  /* else - assertion failure? key found but no string value! */
                }
              continue;
            }
          string=GetImageArtifact(property_image,pattern+9);
          if (string == (char *) NULL)
            goto PropertyLookupFailure; /* no artifact of this specifc name */
          AppendString2Text(string);
          continue;
        }
      if (LocaleNCompare("property:",pattern,9) == 0)
        {
          /*
            Property - direct image property lookup (with glob).
          */
          if (IsGlob(pattern+9) != MagickFalse)
            {
              ResetImagePropertyIterator(property_image);
              while ((key=GetNextImageProperty(property_image)) != (const char *) NULL)
                if (GlobExpression(key,pattern,MagickTrue) != MagickFalse)
                  {
                    string=GetImageProperty(property_image,key,exception);
                    if (string != (const char *) NULL)
                      AppendKeyValue2Text(key,string);
                    /* else - assertion failure? */
                  }
              continue;
            }
          string=GetImageProperty(property_image,pattern+9,exception);
          if (string == (char *) NULL)
            goto PropertyLookupFailure; /* no artifact of this specifc name */
          AppendString2Text(string);
          continue;
        }
      /*
        Properties without special prefix.  This handles attributes,
        properties, and profiles such as %[exif:...].  Note the profile
        properties may also include a glob expansion pattern.
      */
      string=GetImageProperty(property_image,pattern,exception);
      if (string != (const char *) NULL)
        {
          AppendString2Text(string);
          (void) DeleteImageArtifact(property_image,"magick-property");
          (void) DeleteImageOption(property_info,"magick-property");
          continue;
        }
      if (IsGlob(pattern) != MagickFalse)
        {
          /*
            Handle property 'glob' patterns such as:
            %[*] %[user:array_??] %[filename:e*]>
          */
          ResetImagePropertyIterator(property_image);
          while ((key=GetNextImageProperty(property_image)) != (const char *) NULL)
            if (GlobExpression(key,pattern,MagickTrue) != MagickFalse)
              {
                string=GetImageProperty(property_image,key,exception);
                if (string != (const char *) NULL)
                  AppendKeyValue2Text(key,string);
                /* else - assertion failure? */
              }
          continue;
        }
      /*
        Look for a known property or image attribute such as
        %[basename] %[denisty] %[delay].  Also handles a braced single
        letter: %[b] %[G] %[g].
      */
      string=GetMagickProperty(property_info,property_image,pattern,exception);
      if (string != (const char *) NULL)
        {
          AppendString2Text(string);
          continue;
        }
      /*
        Look for a per-image artifact. This includes option lookup
        (FUTURE: interpreted according to image).
      */
      string=GetImageArtifact(property_image,pattern);
      if (string != (char *) NULL)
        {
          AppendString2Text(string);
          continue;
        }
      /*
        No image, so direct 'option' lookup (no delayed percent escapes).
      */
      string=GetImageOption(property_info,pattern);
      if (string != (char *) NULL)
        {
          AppendString2Text(string);
          continue;
        }
PropertyLookupFailure:
      /*
        Failed to find any match anywhere!
      */
      if (len >= 64)
        {
          pattern[61] = '.';  /* truncate string for error message */
          pattern[62] = '.';
          pattern[63] = '.';
          pattern[64] = '\0';
        }
      (void) ThrowMagickException(exception,GetMagickModule(),OptionWarning,
        "UnknownImageProperty","\"%%[%s]\"",pattern);
    }
  }
  *q='\0';
  if (property_image != image)
    property_image=DestroyImage(property_image);
  if (property_info != image_info)
    property_info=DestroyImageInfo(property_info);
  return(interpret_text);
}