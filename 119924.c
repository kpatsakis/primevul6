static Image *ReadPSImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define BoundingBox  "BoundingBox:"
#define BeginDocument  "BeginDocument:"
#define BeginXMPPacket  "<?xpacket begin="
#define EndXMPPacket  "<?xpacket end="
#define ICCProfile "BeginICCProfile:"
#define CMYKCustomColor  "CMYKCustomColor:"
#define CMYKProcessColor  "CMYKProcessColor:"
#define DocumentMedia  "DocumentMedia:"
#define DocumentCustomColors  "DocumentCustomColors:"
#define DocumentProcessColors  "DocumentProcessColors:"
#define EndDocument  "EndDocument:"
#define HiResBoundingBox  "HiResBoundingBox:"
#define ImageData  "ImageData:"
#define PageBoundingBox  "PageBoundingBox:"
#define LanguageLevel  "LanguageLevel:"
#define PageMedia  "PageMedia:"
#define Pages  "Pages:"
#define PhotoshopProfile  "BeginPhotoshop:"
#define PostscriptLevel  "!PS-"
#define RenderPostscriptText  "  Rendering Postscript...  "
#define SpotColor  "+ "

  char
    command[MagickPathExtent],
    *density,
    filename[MagickPathExtent],
    geometry[MagickPathExtent],
    input_filename[MagickPathExtent],
    message[MagickPathExtent],
    *options,
    postscript_filename[MagickPathExtent];

  const char
    *option;

  const DelegateInfo
    *delegate_info;

  GeometryInfo
    geometry_info;

  Image
    *image,
    *next,
    *postscript_image;

  ImageInfo
    *read_info;

  int
    c,
    file;

  MagickBooleanType
    cmyk,
    fitPage,
    skip,
    status;

  MagickStatusType
    flags;

  PointInfo
    delta,
    resolution;

  RectangleInfo
    page;

  register char
    *p;

  register ssize_t
    i;

  SegmentInfo
    bounds,
    hires_bounds;

  short int
    hex_digits[256];

  size_t
    length;

  ssize_t
    count,
    priority;

  StringInfo
    *profile;

  unsigned long
    columns,
    extent,
    language_level,
    pages,
    rows,
    scene,
    spotcolor;

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
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  status=AcquireUniqueSymbolicLink(image_info->filename,input_filename);
  if (status == MagickFalse)
    {
      ThrowFileException(exception,FileOpenError,"UnableToCreateTemporaryFile",
        image_info->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Initialize hex values.
  */
  (void) ResetMagickMemory(hex_digits,0,sizeof(hex_digits));
  hex_digits[(int) '0']=0;
  hex_digits[(int) '1']=1;
  hex_digits[(int) '2']=2;
  hex_digits[(int) '3']=3;
  hex_digits[(int) '4']=4;
  hex_digits[(int) '5']=5;
  hex_digits[(int) '6']=6;
  hex_digits[(int) '7']=7;
  hex_digits[(int) '8']=8;
  hex_digits[(int) '9']=9;
  hex_digits[(int) 'a']=10;
  hex_digits[(int) 'b']=11;
  hex_digits[(int) 'c']=12;
  hex_digits[(int) 'd']=13;
  hex_digits[(int) 'e']=14;
  hex_digits[(int) 'f']=15;
  hex_digits[(int) 'A']=10;
  hex_digits[(int) 'B']=11;
  hex_digits[(int) 'C']=12;
  hex_digits[(int) 'D']=13;
  hex_digits[(int) 'E']=14;
  hex_digits[(int) 'F']=15;
  /*
    Set the page density.
  */
  delta.x=DefaultResolution;
  delta.y=DefaultResolution;
  if ((image->resolution.x == 0.0) || (image->resolution.y == 0.0))
    {
      flags=ParseGeometry(PSDensityGeometry,&geometry_info);
      image->resolution.x=geometry_info.rho;
      image->resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->resolution.y=image->resolution.x;
    }
  if (image_info->density != (char *) NULL)
    {
      flags=ParseGeometry(image_info->density,&geometry_info);
      image->resolution.x=geometry_info.rho;
      image->resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->resolution.y=image->resolution.x;
    }
  (void) ParseAbsoluteGeometry(PSPageGeometry,&page);
  if (image_info->page != (char *) NULL)
    (void) ParseAbsoluteGeometry(image_info->page,&page);
  resolution=image->resolution;
  page.width=(size_t) ceil((double) (page.width*resolution.x/delta.x)-0.5);
  page.height=(size_t) ceil((double) (page.height*resolution.y/delta.y)-0.5);
  /*
    Determine page geometry from the Postscript bounding box.
  */
  (void) ResetMagickMemory(&bounds,0,sizeof(bounds));
  (void) ResetMagickMemory(command,0,sizeof(command));
  cmyk=image_info->colorspace == CMYKColorspace ? MagickTrue : MagickFalse;
  (void) ResetMagickMemory(&hires_bounds,0,sizeof(hires_bounds));
  columns=0;
  rows=0;
  priority=0;
  rows=0;
  extent=0;
  spotcolor=0;
  language_level=1;
  pages=(~0UL);
  skip=MagickFalse;
  p=command;
  for (c=ReadBlobByte(image); c != EOF; c=ReadBlobByte(image))
  {
    /*
      Note document structuring comments.
    */
    *p++=(char) c;
    if ((strchr("\n\r%",c) == (char *) NULL) &&
        ((size_t) (p-command) < (MagickPathExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Skip %%BeginDocument thru %%EndDocument.
    */
    if (LocaleNCompare(BeginDocument,command,strlen(BeginDocument)) == 0)
      skip=MagickTrue;
    if (LocaleNCompare(EndDocument,command,strlen(EndDocument)) == 0)
      skip=MagickFalse;
    if (skip != MagickFalse)
      continue;
    if (LocaleNCompare(PostscriptLevel,command,strlen(PostscriptLevel)) == 0)
      {
        (void) SetImageProperty(image,"ps:Level",command+4,exception);
        if (GlobExpression(command,"*EPSF-*",MagickTrue) != MagickFalse)
          pages=1;
      }
    if (LocaleNCompare(LanguageLevel,command,strlen(LanguageLevel)) == 0)
      (void) sscanf(command,LanguageLevel " %lu",&language_level);
    if (LocaleNCompare(Pages,command,strlen(Pages)) == 0)
      (void) sscanf(command,Pages " %lu",&pages);
    if (LocaleNCompare(ImageData,command,strlen(ImageData)) == 0)
      (void) sscanf(command,ImageData " %lu %lu",&columns,&rows);
    if (LocaleNCompare(ICCProfile,command,strlen(ICCProfile)) == 0)
      {
        unsigned char
          *datum;

        /*
          Read ICC profile.
        */
        profile=AcquireStringInfo(MagickPathExtent);
        datum=GetStringInfoDatum(profile);
        for (i=0; (c=ProfileInteger(image,hex_digits)) != EOF; i++)
        {
          if (i >= (ssize_t) GetStringInfoLength(profile))
            {
              SetStringInfoLength(profile,(size_t) i << 1);
              datum=GetStringInfoDatum(profile);
            }
          datum[i]=(unsigned char) c;
        }
        SetStringInfoLength(profile,(size_t) i+1);
        (void) SetImageProfile(image,"icc",profile,exception);
        profile=DestroyStringInfo(profile);
        continue;
      }
    if (LocaleNCompare(PhotoshopProfile,command,strlen(PhotoshopProfile)) == 0)
      {
        unsigned char
          *q;

        /*
          Read Photoshop profile.
        */
        count=(ssize_t) sscanf(command,PhotoshopProfile " %lu",&extent);
        if (count != 1)
          continue;
        length=extent;
        if (length > GetBlobSize(image))
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        profile=BlobToStringInfo((const void *) NULL,length);
        if (profile != (StringInfo *) NULL)
          {
            q=GetStringInfoDatum(profile);
            for (i=0; i < (ssize_t) length; i++)
              *q++=(unsigned char) ProfileInteger(image,hex_digits);
            (void) SetImageProfile(image,"8bim",profile,exception);
            profile=DestroyStringInfo(profile);
          }
        continue;
      }
    if (LocaleNCompare(BeginXMPPacket,command,strlen(BeginXMPPacket)) == 0)
      {
        /*
          Read XMP profile.
        */
        p=command;
        profile=StringToStringInfo(command);
        for (i=GetStringInfoLength(profile)-1; c != EOF; i++)
        {
          SetStringInfoLength(profile,i+1);
          c=ReadBlobByte(image);
          GetStringInfoDatum(profile)[i]=(unsigned char) c;
          *p++=(char) c;
          if ((strchr("\n\r%",c) == (char *) NULL) &&
              ((size_t) (p-command) < (MagickPathExtent-1)))
            continue;
          *p='\0';
          p=command;
          if (LocaleNCompare(EndXMPPacket,command,strlen(EndXMPPacket)) == 0)
            break;
        }
        SetStringInfoLength(profile,i);
        (void) SetImageProfile(image,"xmp",profile,exception);
        profile=DestroyStringInfo(profile);
        continue;
      }
    /*
      Is this a CMYK document?
    */
    length=strlen(DocumentProcessColors);
    if (LocaleNCompare(DocumentProcessColors,command,length) == 0)
      {
        if ((GlobExpression(command,"*Cyan*",MagickTrue) != MagickFalse) ||
            (GlobExpression(command,"*Magenta*",MagickTrue) != MagickFalse) ||
            (GlobExpression(command,"*Yellow*",MagickTrue) != MagickFalse))
          cmyk=MagickTrue;
      }
    if (LocaleNCompare(CMYKCustomColor,command,strlen(CMYKCustomColor)) == 0)
      cmyk=MagickTrue;
    if (LocaleNCompare(CMYKProcessColor,command,strlen(CMYKProcessColor)) == 0)
      cmyk=MagickTrue;
    length=strlen(DocumentCustomColors);
    if ((LocaleNCompare(DocumentCustomColors,command,length) == 0) ||
        (LocaleNCompare(CMYKCustomColor,command,strlen(CMYKCustomColor)) == 0) ||
        (LocaleNCompare(SpotColor,command,strlen(SpotColor)) == 0))
      {
        char
          property[MagickPathExtent],
          *value;

        register char
          *q;

        /*
          Note spot names.
        */
        (void) FormatLocaleString(property,MagickPathExtent,
          "ps:SpotColor-%.20g",(double) (spotcolor++));
        for (q=command; *q != '\0'; q++)
          if (isspace((int) (unsigned char) *q) != 0)
            break;
        value=AcquireString(q);
        (void) SubstituteString(&value,"(","");
        (void) SubstituteString(&value,")","");
        (void) StripString(value);
        (void) SetImageProperty(image,property,value,exception);
        value=DestroyString(value);
        continue;
      }
    if (image_info->page != (char *) NULL)
      continue;
    /*
      Note region defined by bounding box.
    */
    count=0;
    i=0;
    if (LocaleNCompare(BoundingBox,command,strlen(BoundingBox)) == 0)
      {
        count=(ssize_t) sscanf(command,BoundingBox " %lf %lf %lf %lf",
          &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
        i=2;
      }
    if (LocaleNCompare(DocumentMedia,command,strlen(DocumentMedia)) == 0)
      {
        count=(ssize_t) sscanf(command,DocumentMedia " %lf %lf %lf %lf",
          &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
        i=1;
      }
    if (LocaleNCompare(HiResBoundingBox,command,strlen(HiResBoundingBox)) == 0)
      {
        count=(ssize_t) sscanf(command,HiResBoundingBox " %lf %lf %lf %lf",
          &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
        i=3;
      }
    if (LocaleNCompare(PageBoundingBox,command,strlen(PageBoundingBox)) == 0)
      {
        count=(ssize_t) sscanf(command,PageBoundingBox " %lf %lf %lf %lf",
          &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
        i=1;
      }
    if (LocaleNCompare(PageMedia,command,strlen(PageMedia)) == 0)
      {
        count=(ssize_t) sscanf(command,PageMedia " %lf %lf %lf %lf",
          &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
        i=1;
      }
    if ((count != 4) || (i < (ssize_t) priority))
      continue;
    if ((fabs(bounds.x2-bounds.x1) <= fabs(hires_bounds.x2-hires_bounds.x1)) ||
        (fabs(bounds.y2-bounds.y1) <= fabs(hires_bounds.y2-hires_bounds.y1)))
      if (i ==  (ssize_t) priority)
        continue;
    hires_bounds=bounds;
    priority=i;
  }
  if ((fabs(hires_bounds.x2-hires_bounds.x1) >= MagickEpsilon) && 
      (fabs(hires_bounds.y2-hires_bounds.y1) >= MagickEpsilon))
    {
      /*
        Set Postscript render geometry.
      */
      (void) FormatLocaleString(geometry,MagickPathExtent,"%gx%g%+.15g%+.15g",
        hires_bounds.x2-hires_bounds.x1,hires_bounds.y2-hires_bounds.y1,
        hires_bounds.x1,hires_bounds.y1);
      (void) SetImageProperty(image,"ps:HiResBoundingBox",geometry,exception);
      page.width=(size_t) ceil((double) ((hires_bounds.x2-hires_bounds.x1)*
        resolution.x/delta.x)-0.5);
      page.height=(size_t) ceil((double) ((hires_bounds.y2-hires_bounds.y1)*
        resolution.y/delta.y)-0.5);
    }
  fitPage=MagickFalse;
  option=GetImageOption(image_info,"eps:fit-page");
  if (option != (char *) NULL)
  {
    char
      *page_geometry;

    page_geometry=GetPageGeometry(option);
    flags=ParseMetaGeometry(page_geometry,&page.x,&page.y,&page.width,
      &page.height);
    if (flags == NoValue)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
          "InvalidGeometry","`%s'",option);
        image=DestroyImage(image);
        return((Image *) NULL);
      }
    page.width=(size_t) ceil((double) (page.width*image->resolution.x/delta.x)
      -0.5);
    page.height=(size_t) ceil((double) (page.height*image->resolution.y/
      delta.y) -0.5);
    page_geometry=DestroyString(page_geometry);
    fitPage=MagickTrue;
  }
  (void) CloseBlob(image);
  if (IssRGBCompatibleColorspace(image_info->colorspace) != MagickFalse)
    cmyk=MagickFalse;
  /*
    Create Ghostscript control file.
  */
  file=AcquireUniqueFileResource(postscript_filename);
  if (file == -1)
    {
      ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
        image_info->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  (void) CopyMagickString(command,"/setpagedevice {pop} bind 1 index where {"
    "dup wcheck {3 1 roll put} {pop def} ifelse} {def} ifelse\n"
    "<</UseCIEColor true>>setpagedevice\n",MagickPathExtent);
  count=write(file,command,(unsigned int) strlen(command));
  if (image_info->page == (char *) NULL)
    {
      char
        translate_geometry[MagickPathExtent];

      (void) FormatLocaleString(translate_geometry,MagickPathExtent,
        "%g %g translate\n",-bounds.x1,-bounds.y1);
      count=write(file,translate_geometry,(unsigned int)
        strlen(translate_geometry));
    }
  file=close(file)-1;
  /*
    Render Postscript with the Ghostscript delegate.
  */
  if (image_info->monochrome != MagickFalse)
    delegate_info=GetDelegateInfo("ps:mono",(char *) NULL,exception);
  else
    if (cmyk != MagickFalse)
      delegate_info=GetDelegateInfo("ps:cmyk",(char *) NULL,exception);
    else
      delegate_info=GetDelegateInfo("ps:alpha",(char *) NULL,exception);
  if (delegate_info == (const DelegateInfo *) NULL)
    {
      (void) RelinquishUniqueFileResource(postscript_filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  density=AcquireString("");
  options=AcquireString("");
  (void) FormatLocaleString(density,MagickPathExtent,"%gx%g",resolution.x,
    resolution.y);
  (void) FormatLocaleString(options,MagickPathExtent,"-g%.20gx%.20g ",(double)
    page.width,(double) page.height);
  read_info=CloneImageInfo(image_info);
  *read_info->magick='\0';
  if (read_info->number_scenes != 0)
    {
      char
        pages[MagickPathExtent];

      (void) FormatLocaleString(pages,MagickPathExtent,"-dFirstPage=%.20g "
        "-dLastPage=%.20g ",(double) read_info->scene+1,(double)
        (read_info->scene+read_info->number_scenes));
      (void) ConcatenateMagickString(options,pages,MagickPathExtent);
      read_info->number_scenes=0;
      if (read_info->scenes != (char *) NULL)
        *read_info->scenes='\0';
    }
  if (*image_info->magick == 'E')
    {
      option=GetImageOption(image_info,"eps:use-cropbox");
      if ((option == (const char *) NULL) ||
          (IsStringTrue(option) != MagickFalse))
        (void) ConcatenateMagickString(options,"-dEPSCrop ",MagickPathExtent);
      if (fitPage != MagickFalse)
        (void) ConcatenateMagickString(options,"-dEPSFitPage ",
          MagickPathExtent);
    }
  (void) CopyMagickString(filename,read_info->filename,MagickPathExtent);
  (void) AcquireUniqueFilename(filename);
  (void) RelinquishUniqueFileResource(filename);
  (void) ConcatenateMagickString(filename,"%d",MagickPathExtent);
  (void) FormatLocaleString(command,MagickPathExtent,
    GetDelegateCommands(delegate_info),
    read_info->antialias != MagickFalse ? 4 : 1,
    read_info->antialias != MagickFalse ? 4 : 1,density,options,filename,
    postscript_filename,input_filename);
  options=DestroyString(options);
  density=DestroyString(density);
  *message='\0';
  status=InvokePostscriptDelegate(read_info->verbose,command,message,exception);
  (void) InterpretImageFilename(image_info,image,filename,1,
    read_info->filename,exception);
  if ((status == MagickFalse) ||
      (IsPostscriptRendered(read_info->filename) == MagickFalse))
    {
      (void) ConcatenateMagickString(command," -c showpage",MagickPathExtent);
      status=InvokePostscriptDelegate(read_info->verbose,command,message,
        exception);
    }
  (void) RelinquishUniqueFileResource(postscript_filename);
  (void) RelinquishUniqueFileResource(input_filename);
  postscript_image=(Image *) NULL;
  if (status == MagickFalse)
    for (i=1; ; i++)
    {
      (void) InterpretImageFilename(image_info,image,filename,(int) i,
        read_info->filename,exception);
      if (IsPostscriptRendered(read_info->filename) == MagickFalse)
        break;
      (void) RelinquishUniqueFileResource(read_info->filename);
    }
  else
    for (i=1; ; i++)
    {
      (void) InterpretImageFilename(image_info,image,filename,(int) i,
        read_info->filename,exception);
      if (IsPostscriptRendered(read_info->filename) == MagickFalse)
        break;
      read_info->blob=NULL;
      read_info->length=0;
      next=ReadImage(read_info,exception);
      (void) RelinquishUniqueFileResource(read_info->filename);
      if (next == (Image *) NULL)
        break;
      AppendImageToList(&postscript_image,next);
    }
  (void) RelinquishUniqueFileResource(read_info->filename);
  read_info=DestroyImageInfo(read_info);
  if (postscript_image == (Image *) NULL)
    {
      if (*message != '\0')
        (void) ThrowMagickException(exception,GetMagickModule(),
          DelegateError,"PostscriptDelegateFailed","`%s'",message);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if (LocaleCompare(postscript_image->magick,"BMP") == 0)
    {
      Image
        *cmyk_image;

      cmyk_image=ConsolidateCMYKImages(postscript_image,exception);
      if (cmyk_image != (Image *) NULL)
        {
          postscript_image=DestroyImageList(postscript_image);
          postscript_image=cmyk_image;
        }
    }
  if (image_info->number_scenes != 0)
    {
      Image
        *clone_image;

      /*
        Add place holder images to meet the subimage specification requirement.
      */
      for (i=0; i < (ssize_t) image_info->scene; i++)
      {
        clone_image=CloneImage(postscript_image,1,1,MagickTrue,exception);
        if (clone_image != (Image *) NULL)
          PrependImageToList(&postscript_image,clone_image);
      }
    }
  do
  {
    (void) CopyMagickString(postscript_image->filename,filename,
      MagickPathExtent);
    (void) CopyMagickString(postscript_image->magick,image->magick,
      MagickPathExtent);
    if (columns != 0)
      postscript_image->magick_columns=columns;
    if (rows != 0)
      postscript_image->magick_rows=rows;
    postscript_image->page=page;
    (void) CloneImageProfiles(postscript_image,image);
    (void) CloneImageProperties(postscript_image,image);
    next=SyncNextImageInList(postscript_image);
    if (next != (Image *) NULL)
      postscript_image=next;
  } while (next != (Image *) NULL);
  image=DestroyImageList(image);
  scene=0;
  for (next=GetFirstImageInList(postscript_image); next != (Image *) NULL; )
  {
    next->scene=scene++;
    next=GetNextImageInList(next);
  }
  return(GetFirstImageInList(postscript_image));
}