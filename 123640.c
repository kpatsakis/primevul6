static MagickBooleanType TraceBezier(MVGInfo *mvg_info,
  const size_t number_coordinates)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  PrimitiveInfo
    *primitive_info;

  PrimitiveInfo
    *p;

  ssize_t
    i,
    j;

  size_t
    control_points,
    quantum;

  /*
    Allocate coefficients.
  */
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  quantum=number_coordinates;
  for (i=0; i < (ssize_t) number_coordinates; i++)
  {
    for (j=i+1; j < (ssize_t) number_coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > (double) MAGICK_SSIZE_MAX)
        {
          (void) ThrowMagickException(mvg_info->exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'","");
          return(MagickFalse);
        }
      if (alpha > (double) quantum)
        quantum=(size_t) alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > (double) MAGICK_SSIZE_MAX)
        {
          (void) ThrowMagickException(mvg_info->exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'","");
          return(MagickFalse);
        }
      if (alpha > (double) quantum)
        quantum=(size_t) alpha;
    }
  }
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  quantum=MagickMin(quantum/number_coordinates,BezierQuantum);
  coefficients=(double *) AcquireQuantumMemory(number_coordinates,
    sizeof(*coefficients));
  points=(PointInfo *) AcquireQuantumMemory(quantum,number_coordinates*
    sizeof(*points));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    {
      if (points != (PointInfo *) NULL)
        points=(PointInfo *) RelinquishMagickMemory(points);
      if (coefficients != (double *) NULL)
        coefficients=(double *) RelinquishMagickMemory(coefficients);
      (void) ThrowMagickException(mvg_info->exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return(MagickFalse);
    }
  control_points=quantum*number_coordinates;
  if (CheckPrimitiveExtent(mvg_info,(double) control_points+1) == MagickFalse)
    {
      points=(PointInfo *) RelinquishMagickMemory(points);
      coefficients=(double *) RelinquishMagickMemory(coefficients);
      return(MagickFalse);
    }
  primitive_info=(*mvg_info->primitive_info)+mvg_info->offset;
  /*
    Compute bezier points.
  */
  end=primitive_info[number_coordinates-1].point;
  for (i=0; i < (ssize_t) number_coordinates; i++)
    coefficients[i]=Permutate((ssize_t) number_coordinates-1,i);
  weight=0.0;
  for (i=0; i < (ssize_t) control_points; i++)
  {
    p=primitive_info;
    point.x=0.0;
    point.y=0.0;
    alpha=pow((double) (1.0-weight),(double) number_coordinates-1.0);
    for (j=0; j < (ssize_t) number_coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/control_points;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (ssize_t) control_points; i++)
  {
    if (TracePoint(p,points[i]) == MagickFalse)
      {
        points=(PointInfo *) RelinquishMagickMemory(points);
        coefficients=(double *) RelinquishMagickMemory(coefficients);
        return(MagickFalse);
      }
    p+=p->coordinates;
  }
  if (TracePoint(p,end) == MagickFalse)
    {
      points=(PointInfo *) RelinquishMagickMemory(points);
      coefficients=(double *) RelinquishMagickMemory(coefficients);
      return(MagickFalse);
    }
  p+=p->coordinates;
  primitive_info->coordinates=(size_t) (p-primitive_info);
  primitive_info->closed_subpath=MagickFalse;
  for (i=0; i < (ssize_t) primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  points=(PointInfo *) RelinquishMagickMemory(points);
  coefficients=(double *) RelinquishMagickMemory(coefficients);
  return(MagickTrue);
}