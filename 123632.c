static void LogPrimitiveInfo(const PrimitiveInfo *primitive_info)
{
  const char
    *methods[] =
    {
      "point",
      "replace",
      "floodfill",
      "filltoborder",
      "reset",
      "?"
    };

  PointInfo
    p,
    point,
    q;

  ssize_t
    i,
    x;

  ssize_t
    coordinates,
    y;

  x=CastDoubleToLong(ceil(primitive_info->point.x-0.5));
  y=CastDoubleToLong(ceil(primitive_info->point.y-0.5));
  switch (primitive_info->primitive)
  {
    case AlphaPrimitive:
    {
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "AlphaPrimitive %.20g,%.20g %s",(double) x,(double) y,
        methods[primitive_info->method]);
      return;
    }
    case ColorPrimitive:
    {
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "ColorPrimitive %.20g,%.20g %s",(double) x,(double) y,
        methods[primitive_info->method]);
      return;
    }
    case ImagePrimitive:
    {
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "ImagePrimitive %.20g,%.20g",(double) x,(double) y);
      return;
    }
    case PointPrimitive:
    {
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "PointPrimitive %.20g,%.20g %s",(double) x,(double) y,
        methods[primitive_info->method]);
      return;
    }
    case TextPrimitive:
    {
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "TextPrimitive %.20g,%.20g",(double) x,(double) y);
      return;
    }
    default:
      break;
  }
  coordinates=0;
  p=primitive_info[0].point;
  q.x=(-1.0);
  q.y=(-1.0);
  for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
  {
    point=primitive_info[i].point;
    if (coordinates <= 0)
      {
        coordinates=(ssize_t) primitive_info[i].coordinates;
        (void) LogMagickEvent(DrawEvent,GetMagickModule(),
          "    begin open (%.20g)",(double) coordinates);
        p=point;
      }
    point=primitive_info[i].point;
    if ((fabs(q.x-point.x) >= MagickEpsilon) ||
        (fabs(q.y-point.y) >= MagickEpsilon))
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "      %.20g: %.18g,%.18g",(double) coordinates,point.x,point.y);
    else
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),
        "      %.20g: %g %g (duplicate)",(double) coordinates,point.x,point.y);
    q=point;
    coordinates--;
    if (coordinates > 0)
      continue;
    if ((fabs(p.x-point.x) >= MagickEpsilon) ||
        (fabs(p.y-point.y) >= MagickEpsilon))
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    end last (%.20g)",
        (double) coordinates);
    else
      (void) LogMagickEvent(DrawEvent,GetMagickModule(),"    end open (%.20g)",
        (double) coordinates);
  }
}