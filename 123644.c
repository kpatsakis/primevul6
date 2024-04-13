static MagickBooleanType TraceArcPath(MVGInfo *mvg_info,const PointInfo start,
  const PointInfo end,const PointInfo arc,const double angle,
  const MagickBooleanType large_arc,const MagickBooleanType sweep)
{
  double
    alpha,
    beta,
    delta,
    factor,
    gamma,
    theta;

  MagickStatusType
    status;

  PointInfo
    center,
    points[3],
    radii;

  double
    cosine,
    sine;

  PrimitiveInfo
    *primitive_info;

  PrimitiveInfo
    *p;

  ssize_t
    i;

  size_t
    arc_segments;

  ssize_t
    offset;

  offset=mvg_info->offset;
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  primitive_info->coordinates=0;
  if ((fabs(start.x-end.x) < MagickEpsilon) &&
      (fabs(start.y-end.y) < MagickEpsilon))
    return(TracePoint(primitive_info,end));
  radii.x=fabs(arc.x);
  radii.y=fabs(arc.y);
  if ((radii.x < MagickEpsilon) || (radii.y < MagickEpsilon))
    return(TraceLine(primitive_info,start,end));
  cosine=cos(DegreesToRadians(fmod((double) angle,360.0)));
  sine=sin(DegreesToRadians(fmod((double) angle,360.0)));
  center.x=(double) (cosine*(end.x-start.x)/2+sine*(end.y-start.y)/2);
  center.y=(double) (cosine*(end.y-start.y)/2-sine*(end.x-start.x)/2);
  delta=(center.x*center.x)/(radii.x*radii.x)+(center.y*center.y)/
    (radii.y*radii.y);
  if (delta < MagickEpsilon)
    return(TraceLine(primitive_info,start,end));
  if (delta > 1.0)
    {
      radii.x*=sqrt((double) delta);
      radii.y*=sqrt((double) delta);
    }
  points[0].x=(double) (cosine*start.x/radii.x+sine*start.y/radii.x);
  points[0].y=(double) (cosine*start.y/radii.y-sine*start.x/radii.y);
  points[1].x=(double) (cosine*end.x/radii.x+sine*end.y/radii.x);
  points[1].y=(double) (cosine*end.y/radii.y-sine*end.x/radii.y);
  alpha=points[1].x-points[0].x;
  beta=points[1].y-points[0].y;
  if (fabs(alpha*alpha+beta*beta) < MagickEpsilon)
    return(TraceLine(primitive_info,start,end));
  factor=PerceptibleReciprocal(alpha*alpha+beta*beta)-0.25;
  if (factor <= 0.0)
    factor=0.0;
  else
    {
      factor=sqrt((double) factor);
      if (sweep == large_arc)
        factor=(-factor);
    }
  center.x=(double) ((points[0].x+points[1].x)/2-factor*beta);
  center.y=(double) ((points[0].y+points[1].y)/2+factor*alpha);
  alpha=atan2(points[0].y-center.y,points[0].x-center.x);
  theta=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((theta < 0.0) && (sweep != MagickFalse))
    theta+=2.0*MagickPI;
  else
    if ((theta > 0.0) && (sweep == MagickFalse))
      theta-=2.0*MagickPI;
  arc_segments=(size_t) CastDoubleToLong(ceil(fabs((double) (theta/(0.5*
    MagickPI+MagickEpsilon)))));
  status=MagickTrue;
  p=primitive_info;
  for (i=0; i < (ssize_t) arc_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*theta/arc_segments)-(alpha+i*theta/arc_segments));
    gamma=(8.0/3.0)*sin(fmod((double) (0.5*beta),DegreesToRadians(360.0)))*
      sin(fmod((double) (0.5*beta),DegreesToRadians(360.0)))/
      sin(fmod((double) beta,DegreesToRadians(360.0)));
    points[0].x=(double) (center.x+cos(fmod((double) (alpha+(double) i*theta/
      arc_segments),DegreesToRadians(360.0)))-gamma*sin(fmod((double) (alpha+
      (double) i*theta/arc_segments),DegreesToRadians(360.0))));
    points[0].y=(double) (center.y+sin(fmod((double) (alpha+(double) i*theta/
      arc_segments),DegreesToRadians(360.0)))+gamma*cos(fmod((double) (alpha+
      (double) i*theta/arc_segments),DegreesToRadians(360.0))));
    points[2].x=(double) (center.x+cos(fmod((double) (alpha+(double) (i+1)*
      theta/arc_segments),DegreesToRadians(360.0))));
    points[2].y=(double) (center.y+sin(fmod((double) (alpha+(double) (i+1)*
      theta/arc_segments),DegreesToRadians(360.0))));
    points[1].x=(double) (points[2].x+gamma*sin(fmod((double) (alpha+(double)
      (i+1)*theta/arc_segments),DegreesToRadians(360.0))));
    points[1].y=(double) (points[2].y-gamma*cos(fmod((double) (alpha+(double)
      (i+1)*theta/arc_segments),DegreesToRadians(360.0))));
    p->point.x=(p == primitive_info) ? start.x : (p-1)->point.x;
    p->point.y=(p == primitive_info) ? start.y : (p-1)->point.y;
    (p+1)->point.x=(double) (cosine*radii.x*points[0].x-sine*radii.y*
      points[0].y);
    (p+1)->point.y=(double) (sine*radii.x*points[0].x+cosine*radii.y*
      points[0].y);
    (p+2)->point.x=(double) (cosine*radii.x*points[1].x-sine*radii.y*
      points[1].y);
    (p+2)->point.y=(double) (sine*radii.x*points[1].x+cosine*radii.y*
      points[1].y);
    (p+3)->point.x=(double) (cosine*radii.x*points[2].x-sine*radii.y*
      points[2].y);
    (p+3)->point.y=(double) (sine*radii.x*points[2].x+cosine*radii.y*
      points[2].y);
    if (i == (ssize_t) (arc_segments-1))
      (p+3)->point=end;
    status&=TraceBezier(mvg_info,4);
    if (status == 0)
      break;
    p=(*mvg_info->primitive_info)+mvg_info->offset;
    mvg_info->offset+=p->coordinates;
    p+=p->coordinates;
  }
  if (status == 0)
    return(MagickFalse);
  mvg_info->offset=offset;
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  primitive_info->coordinates=(size_t) (p-primitive_info);
  primitive_info->closed_subpath=MagickFalse;
  for (i=0; i < (ssize_t) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  return(MagickTrue);
}