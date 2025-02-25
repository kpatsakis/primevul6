static Image *ReadLABELImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    geometry[MagickPathExtent],
    *property;

  const char
    *label;

  DrawInfo
    *draw_info;

  Image
    *image;

  MagickBooleanType
    status;

  TypeMetric
    metrics;

  size_t
    height,
    width;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  (void) ResetImagePage(image,"0x0+0+0");
  property=InterpretImageProperties((ImageInfo *) image_info,image,
    image_info->filename,exception);
  (void) SetImageProperty(image,"label",property,exception);
  property=DestroyString(property);
  label=GetImageProperty(image,"label",exception);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->text=ConstantString(label);
  metrics.width=0;
  metrics.ascent=0.0;
  status=GetMultilineTypeMetrics(image,draw_info,&metrics,exception);
  if ((image->columns == 0) && (image->rows == 0))
    {
      image->columns=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
      image->rows=(size_t) floor(metrics.height+draw_info->stroke_width+0.5);
    }
  else
    if (((image->columns == 0) || (image->rows == 0)) ||
        (fabs(image_info->pointsize) < MagickEpsilon))
      {
        double
          high,
          low;

        /*
          Auto fit text into bounding box.
        */
        for ( ; ; draw_info->pointsize*=2.0)
        {
          (void) FormatLocaleString(geometry,MagickPathExtent,"%+g%+g",
            -metrics.bounds.x1,metrics.ascent);
          if (draw_info->gravity == UndefinedGravity)
            (void) CloneString(&draw_info->geometry,geometry);
          status=GetMultilineTypeMetrics(image,draw_info,&metrics,exception);
          width=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
          height=(size_t) floor(metrics.height+draw_info->stroke_width+0.5);
          if ((image->columns != 0) && (image->rows != 0))
            {
              if ((width >= image->columns) && (height >= image->rows))
                break;
            }
          else
            if (((image->columns != 0) && (width >= image->columns)) ||
                ((image->rows != 0) && (height >= image->rows)))
              break;
        }
        high=draw_info->pointsize;
        for (low=1.0; (high-low) > 0.5; )
        {
          draw_info->pointsize=(low+high)/2.0;
          (void) FormatLocaleString(geometry,MagickPathExtent,"%+g%+g",
            -metrics.bounds.x1,metrics.ascent);
          if (draw_info->gravity == UndefinedGravity)
            (void) CloneString(&draw_info->geometry,geometry);
          status=GetMultilineTypeMetrics(image,draw_info,&metrics,exception);
          width=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
          height=(size_t) floor(metrics.height+draw_info->stroke_width+0.5);
          if ((image->columns != 0) && (image->rows != 0))
            {
              if ((width < image->columns) && (height < image->rows))
                low=draw_info->pointsize+0.5;
              else
                high=draw_info->pointsize-0.5;
            }
          else
            if (((image->columns != 0) && (width < image->columns)) ||
                ((image->rows != 0) && (height < image->rows)))
              low=draw_info->pointsize+0.5;
            else
              high=draw_info->pointsize-0.5;
        }
        draw_info->pointsize=(low+high)/2.0-0.5;
      }
   status=GetMultilineTypeMetrics(image,draw_info,&metrics,exception);
   if (status == MagickFalse)
     {
      draw_info=DestroyDrawInfo(draw_info);
       image=DestroyImageList(image);
       return((Image *) NULL);
     }
  if (image->columns == 0)
    image->columns=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
  if (image->columns == 0)
    image->columns=(size_t) floor(draw_info->pointsize+draw_info->stroke_width+
      0.5);
  if (image->rows == 0)
    image->rows=(size_t) floor(metrics.ascent-metrics.descent+
      draw_info->stroke_width+0.5);
  if (image->rows == 0)
    image->rows=(size_t) floor(draw_info->pointsize+draw_info->stroke_width+
      0.5);
  status=SetImageExtent(image,image->columns,image->rows,exception);
  if (status == MagickFalse)
    {
      draw_info=DestroyDrawInfo(draw_info);
      return(DestroyImageList(image));
    }
  if (SetImageBackgroundColor(image,exception) == MagickFalse)
    {
      draw_info=DestroyDrawInfo(draw_info);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Draw label.
  */
  (void) FormatLocaleString(geometry,MagickPathExtent,"%+g%+g",
    draw_info->direction == RightToLeftDirection ? image->columns-
    metrics.bounds.x2 : 0.0,draw_info->gravity == UndefinedGravity ?
    metrics.ascent : 0.0);
  draw_info->geometry=AcquireString(geometry);
  status=AnnotateImage(image,draw_info,exception);
  if (image_info->pointsize == 0.0)
    {
      char
        pointsize[MagickPathExtent];

      (void) FormatLocaleString(pointsize,MagickPathExtent,"%.20g",
        draw_info->pointsize);
      (void) SetImageProperty(image,"label:pointsize",pointsize,exception);
    }
  draw_info=DestroyDrawInfo(draw_info);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  return(GetFirstImageInList(image));
}