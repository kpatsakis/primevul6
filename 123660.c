static MagickBooleanType TraceSquareLinecap(PrimitiveInfo *primitive_info,
  const size_t number_vertices,const double offset)
{
  double
    distance;

  double
    dx,
    dy;

  ssize_t
    i;

  ssize_t
    j;

  dx=0.0;
  dy=0.0;
  for (i=1; i < (ssize_t) number_vertices; i++)
  {
    dx=primitive_info[0].point.x-primitive_info[i].point.x;
    dy=primitive_info[0].point.y-primitive_info[i].point.y;
    if ((fabs((double) dx) >= MagickEpsilon) ||
        (fabs((double) dy) >= MagickEpsilon))
      break;
  }
  if (i == (ssize_t) number_vertices)
    i=(ssize_t) number_vertices-1L;
  distance=hypot((double) dx,(double) dy);
  primitive_info[0].point.x=(double) (primitive_info[i].point.x+
    dx*(distance+offset)/distance);
  primitive_info[0].point.y=(double) (primitive_info[i].point.y+
    dy*(distance+offset)/distance);
  for (j=(ssize_t) number_vertices-2; j >= 0;  j--)
  {
    dx=primitive_info[number_vertices-1].point.x-primitive_info[j].point.x;
    dy=primitive_info[number_vertices-1].point.y-primitive_info[j].point.y;
    if ((fabs((double) dx) >= MagickEpsilon) ||
        (fabs((double) dy) >= MagickEpsilon))
      break;
  }
  distance=hypot((double) dx,(double) dy);
  primitive_info[number_vertices-1].point.x=(double) (primitive_info[j].point.x+
    dx*(distance+offset)/distance);
  primitive_info[number_vertices-1].point.y=(double) (primitive_info[j].point.y+
    dy*(distance+offset)/distance);
  return(MagickTrue);
}