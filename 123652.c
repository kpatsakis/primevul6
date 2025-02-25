MagickExport void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  char
    *next_token;

  const char
    *option;

  ExceptionInfo
    *exception;

  ImageInfo
    *clone_info;

  /*
    Initialize draw attributes.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(draw_info != (DrawInfo *) NULL);
  (void) memset(draw_info,0,sizeof(*draw_info));
  clone_info=CloneImageInfo(image_info);
  GetAffineMatrix(&draw_info->affine);
  exception=AcquireExceptionInfo();
  (void) QueryColorCompliance("#000F",AllCompliance,&draw_info->fill,
    exception);
  (void) QueryColorCompliance("#FFF0",AllCompliance,&draw_info->stroke,
    exception);
  draw_info->stroke_antialias=clone_info->antialias;
  draw_info->stroke_width=1.0;
  draw_info->fill_rule=EvenOddRule;
  draw_info->alpha=OpaqueAlpha;
  draw_info->fill_alpha=OpaqueAlpha;
  draw_info->stroke_alpha=OpaqueAlpha;
  draw_info->linecap=ButtCap;
  draw_info->linejoin=MiterJoin;
  draw_info->miterlimit=10;
  draw_info->decorate=NoDecoration;
  draw_info->pointsize=12.0;
  draw_info->undercolor.alpha=(MagickRealType) TransparentAlpha;
  draw_info->compose=OverCompositeOp;
  draw_info->render=MagickTrue;
  draw_info->clip_path=MagickFalse;
  draw_info->debug=IsEventLogging();
  if (clone_info->font != (char *) NULL)
    draw_info->font=AcquireString(clone_info->font);
  if (clone_info->density != (char *) NULL)
    draw_info->density=AcquireString(clone_info->density);
  draw_info->text_antialias=clone_info->antialias;
  if (fabs(clone_info->pointsize) >= MagickEpsilon)
    draw_info->pointsize=clone_info->pointsize;
  draw_info->border_color=clone_info->border_color;
  if (clone_info->server_name != (char *) NULL)
    draw_info->server_name=AcquireString(clone_info->server_name);
  option=GetImageOption(clone_info,"direction");
  if (option != (const char *) NULL)
    draw_info->direction=(DirectionType) ParseCommandOption(
      MagickDirectionOptions,MagickFalse,option);
  else
    draw_info->direction=UndefinedDirection;
  option=GetImageOption(clone_info,"encoding");
  if (option != (const char *) NULL)
    (void) CloneString(&draw_info->encoding,option);
  option=GetImageOption(clone_info,"family");
  if (option != (const char *) NULL)
    (void) CloneString(&draw_info->family,option);
  option=GetImageOption(clone_info,"fill");
  if (option != (const char *) NULL)
    (void) QueryColorCompliance(option,AllCompliance,&draw_info->fill,
      exception);
  option=GetImageOption(clone_info,"gravity");
  if (option != (const char *) NULL)
    draw_info->gravity=(GravityType) ParseCommandOption(MagickGravityOptions,
      MagickFalse,option);
  option=GetImageOption(clone_info,"interline-spacing");
  if (option != (const char *) NULL)
    draw_info->interline_spacing=GetDrawValue(option,&next_token);
  option=GetImageOption(clone_info,"interword-spacing");
  if (option != (const char *) NULL)
    draw_info->interword_spacing=GetDrawValue(option,&next_token);
  option=GetImageOption(clone_info,"kerning");
  if (option != (const char *) NULL)
    draw_info->kerning=GetDrawValue(option,&next_token);
  option=GetImageOption(clone_info,"stroke");
  if (option != (const char *) NULL)
    (void) QueryColorCompliance(option,AllCompliance,&draw_info->stroke,
      exception);
  option=GetImageOption(clone_info,"strokewidth");
  if (option != (const char *) NULL)
    draw_info->stroke_width=GetDrawValue(option,&next_token);
  option=GetImageOption(clone_info,"style");
  if (option != (const char *) NULL)
    draw_info->style=(StyleType) ParseCommandOption(MagickStyleOptions,
      MagickFalse,option);
  option=GetImageOption(clone_info,"undercolor");
  if (option != (const char *) NULL)
    (void) QueryColorCompliance(option,AllCompliance,&draw_info->undercolor,
      exception);
  option=GetImageOption(clone_info,"weight");
  if (option != (const char *) NULL)
    {
      ssize_t
        weight;

      weight=ParseCommandOption(MagickWeightOptions,MagickFalse,option);
      if (weight == -1)
        weight=(ssize_t) StringToUnsignedLong(option);
      draw_info->weight=(size_t) weight;
    }
  exception=DestroyExceptionInfo(exception);
  draw_info->signature=MagickCoreSignature;
  clone_info=DestroyImageInfo(clone_info);
}