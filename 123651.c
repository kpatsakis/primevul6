static double GetFillAlpha(PolygonInfo *polygon_info,const double mid,
  const MagickBooleanType fill,const FillRule fill_rule,const ssize_t x,
  const ssize_t y,double *stroke_alpha)
{
  double
    alpha,
    beta,
    distance,
    subpath_alpha;

  PointInfo
    delta;

  const PointInfo
    *q;

  EdgeInfo
    *p;

  ssize_t
    i;

  ssize_t
    j,
    winding_number;

  /*
    Compute fill & stroke opacity for this (x,y) point.
  */
  *stroke_alpha=0.0;
  subpath_alpha=0.0;
  p=polygon_info->edges;
  for (j=0; j < (ssize_t) polygon_info->number_edges; j++, p++)
  {
    if ((double) y <= (p->bounds.y1-mid-0.5))
      break;
    if ((double) y > (p->bounds.y2+mid+0.5))
      {
        (void) DestroyEdge(polygon_info,j);
        continue;
      }
    if (((double) x <= (p->bounds.x1-mid-0.5)) ||
        ((double) x > (p->bounds.x2+mid+0.5)))
      continue;
    i=(ssize_t) MagickMax((double) p->highwater,1.0);
    for ( ; i < (ssize_t) p->number_points; i++)
    {
      if ((double) y <= (p->points[i-1].y-mid-0.5))
        break;
      if ((double) y > (p->points[i].y+mid+0.5))
        continue;
      if (p->scanline != (double) y)
        {
          p->scanline=(double) y;
          p->highwater=(size_t) i;
        }
      /*
        Compute distance between a point and an edge.
      */
      q=p->points+i-1;
      delta.x=(q+1)->x-q->x;
      delta.y=(q+1)->y-q->y;
      beta=delta.x*(x-q->x)+delta.y*(y-q->y);
      if (beta <= 0.0)
        {
          delta.x=(double) x-q->x;
          delta.y=(double) y-q->y;
          distance=delta.x*delta.x+delta.y*delta.y;
        }
      else
        {
          alpha=delta.x*delta.x+delta.y*delta.y;
          if (beta >= alpha)
            {
              delta.x=(double) x-(q+1)->x;
              delta.y=(double) y-(q+1)->y;
              distance=delta.x*delta.x+delta.y*delta.y;
            }
          else
            {
              alpha=PerceptibleReciprocal(alpha);
              beta=delta.x*(y-q->y)-delta.y*(x-q->x)+MagickEpsilon;
              distance=alpha*beta*beta;
            }
        }
      /*
        Compute stroke & subpath opacity.
      */
      beta=0.0;
      if (p->ghostline == MagickFalse)
        {
          alpha=mid+0.5;
          if ((*stroke_alpha < 1.0) &&
              (distance <= ((alpha+0.25)*(alpha+0.25))))
            {
              alpha=mid-0.5;
              if (distance <= ((alpha+0.25)*(alpha+0.25)))
                *stroke_alpha=1.0;
              else
                {
                  beta=1.0;
                  if (fabs(distance-1.0) >= MagickEpsilon)
                    beta=sqrt((double) distance);
                  alpha=beta-mid-0.5;
                  if (*stroke_alpha < ((alpha-0.25)*(alpha-0.25)))
                    *stroke_alpha=(alpha-0.25)*(alpha-0.25);
                }
            }
        }
      if ((fill == MagickFalse) || (distance > 1.0) || (subpath_alpha >= 1.0))
        continue;
      if (distance <= 0.0)
        {
          subpath_alpha=1.0;
          continue;
        }
      if (distance > 1.0)
        continue;
      if (fabs(beta) < MagickEpsilon)
        {
          beta=1.0;
          if (fabs(distance-1.0) >= MagickEpsilon)
            beta=sqrt(distance);
        }
      alpha=beta-1.0;
      if (subpath_alpha < (alpha*alpha))
        subpath_alpha=alpha*alpha;
    }
  }
  /*
    Compute fill opacity.
  */
  if (fill == MagickFalse)
    return(0.0);
  if (subpath_alpha >= 1.0)
    return(1.0);
  /*
    Determine winding number.
  */
  winding_number=0;
  p=polygon_info->edges;
  for (j=0; j < (ssize_t) polygon_info->number_edges; j++, p++)
  {
    if ((double) y <= p->bounds.y1)
      break;
    if (((double) y > p->bounds.y2) || ((double) x <= p->bounds.x1))
      continue;
    if ((double) x > p->bounds.x2)
      {
        winding_number+=p->direction ? 1 : -1;
        continue;
      }
    i=(ssize_t) MagickMax((double) p->highwater,1.0);
    for ( ; i < (ssize_t) (p->number_points-1); i++)
      if ((double) y <= p->points[i].y)
        break;
    q=p->points+i-1;
    if ((((q+1)->x-q->x)*(y-q->y)) <= (((q+1)->y-q->y)*(x-q->x)))
      winding_number+=p->direction ? 1 : -1;
  }
  if (fill_rule != NonZeroRule)
    {
      if ((MagickAbsoluteValue(winding_number) & 0x01) != 0)
        return(1.0);
    }
  else
    if (MagickAbsoluteValue(winding_number) != 0)
      return(1.0);
  return(subpath_alpha);
}