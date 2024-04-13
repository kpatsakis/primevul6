static MagickBooleanType TraceRoundRectangle(MVGInfo *mvg_info,
  const PointInfo start,const PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    point,
    segment;

  PrimitiveInfo
    *primitive_info;

  PrimitiveInfo
    *p;

  ssize_t
    i;

  ssize_t
    offset;

  offset=mvg_info->offset;
  segment.x=fabs(end.x-start.x);
  segment.y=fabs(end.y-start.y);
  if ((segment.x < MagickEpsilon) || (segment.y < MagickEpsilon))
    {
      (*mvg_info->primitive_info+mvg_info->offset)->coordinates=0;
      return(MagickTrue);
    }
  if (arc.x > (0.5*segment.x))
    arc.x=0.5*segment.x;
  if (arc.y > (0.5*segment.y))
    arc.y=0.5*segment.y;
  point.x=start.x+segment.x-arc.x;
  point.y=start.y+arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  if (TraceEllipse(mvg_info,point,arc,degrees) == MagickFalse)
    return(MagickFalse);
  p=(*mvg_info->primitive_info)+mvg_info->offset;
  mvg_info->offset+=p->coordinates;
  point.x=start.x+segment.x-arc.x;
  point.y=start.y+segment.y-arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  if (TraceEllipse(mvg_info,point,arc,degrees) == MagickFalse)
    return(MagickFalse);
  p=(*mvg_info->primitive_info)+mvg_info->offset;
  mvg_info->offset+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+segment.y-arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  if (TraceEllipse(mvg_info,point,arc,degrees) == MagickFalse)
    return(MagickFalse);
  p=(*mvg_info->primitive_info)+mvg_info->offset;
  mvg_info->offset+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  if (TraceEllipse(mvg_info,point,arc,degrees) == MagickFalse)
    return(MagickFalse);
  p=(*mvg_info->primitive_info)+mvg_info->offset;
  mvg_info->offset+=p->coordinates;
  if (CheckPrimitiveExtent(mvg_info,PrimitiveExtentPad) == MagickFalse)
    return(MagickFalse);
  p=(*mvg_info->primitive_info)+mvg_info->offset;
  if (TracePoint(p,(*mvg_info->primitive_info+offset)->point) == MagickFalse)
    return(MagickFalse);
  p+=p->coordinates;
  mvg_info->offset=offset;
  primitive_info=(*mvg_info->primitive_info)+offset;
  primitive_info->coordinates=(size_t) (p-primitive_info);
  primitive_info->closed_subpath=MagickTrue;
  for (i=0; i < (ssize_t) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  return(MagickTrue);
}