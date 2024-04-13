static MagickBooleanType DrawStrokePolygon(Image *image,
  const DrawInfo *draw_info,const PrimitiveInfo *primitive_info,
  ExceptionInfo *exception)
{
  DrawInfo
    *clone_info;

  MagickBooleanType
    closed_path;

  MagickStatusType
    status;

  PrimitiveInfo
    *stroke_polygon;

  const PrimitiveInfo
    *p,
    *q;

  /*
    Draw stroked polygon.
  */
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),
      "    begin draw-stroke-polygon");
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  clone_info->fill=draw_info->stroke;
  if (clone_info->fill_pattern != (Image *) NULL)
    clone_info->fill_pattern=DestroyImage(clone_info->fill_pattern);
  if (clone_info->stroke_pattern != (Image *) NULL)
    clone_info->fill_pattern=CloneImage(clone_info->stroke_pattern,0,0,
      MagickTrue,exception);
  clone_info->stroke.alpha=(MagickRealType) TransparentAlpha;
  clone_info->stroke_width=0.0;
  clone_info->fill_rule=NonZeroRule;
  status=MagickTrue;
  for (p=primitive_info; p->primitive != UndefinedPrimitive; p+=p->coordinates)
  {
    if (p->coordinates == 1)
      continue;
    stroke_polygon=TraceStrokePolygon(draw_info,p,exception);
    if (stroke_polygon == (PrimitiveInfo *) NULL)
      {
        status=0;
        break;
      }
    status&=DrawPolygonPrimitive(image,clone_info,stroke_polygon,exception);
    stroke_polygon=(PrimitiveInfo *) RelinquishMagickMemory(stroke_polygon);
    if (status == 0)
      break;
    q=p+p->coordinates-1;
    closed_path=p->closed_subpath;
    if ((draw_info->linecap == RoundCap) && (closed_path == MagickFalse))
      {
        status&=DrawRoundLinecap(image,draw_info,p,exception);
        status&=DrawRoundLinecap(image,draw_info,q,exception);
      }
  }
  clone_info=DestroyDrawInfo(clone_info);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),
      "    end draw-stroke-polygon");
  return(status != 0 ? MagickTrue : MagickFalse);
}