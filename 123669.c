static PolygonInfo *ConvertPathToPolygon(const PathInfo *path_info,
  ExceptionInfo *exception)
{
  long
    direction,
    next_direction;

  PointInfo
    point,
    *points;

  PolygonInfo
    *polygon_info;

  SegmentInfo
    bounds;

  ssize_t
    i,
    n;

  MagickBooleanType
    ghostline;

  size_t
    edge,
    number_edges,
    number_points;

  /*
    Convert a path to the more efficient sorted rendering form.
  */
  polygon_info=(PolygonInfo *) AcquireMagickMemory(sizeof(*polygon_info));
  if (polygon_info == (PolygonInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return((PolygonInfo *) NULL);
    }
  number_edges=16;
  polygon_info->edges=(EdgeInfo *) AcquireQuantumMemory(number_edges,
    sizeof(*polygon_info->edges));
  if (polygon_info->edges == (EdgeInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return(DestroyPolygonInfo(polygon_info));
    }
  (void) memset(polygon_info->edges,0,number_edges*
    sizeof(*polygon_info->edges));
  direction=0;
  edge=0;
  ghostline=MagickFalse;
  n=0;
  number_points=0;
  points=(PointInfo *) NULL;
  (void) memset(&point,0,sizeof(point));
  (void) memset(&bounds,0,sizeof(bounds));
  polygon_info->edges[edge].number_points=(size_t) n;
  polygon_info->edges[edge].scanline=0.0;
  polygon_info->edges[edge].highwater=0;
  polygon_info->edges[edge].ghostline=ghostline;
  polygon_info->edges[edge].direction=(ssize_t) direction;
  polygon_info->edges[edge].points=points;
  polygon_info->edges[edge].bounds=bounds;
  polygon_info->number_edges=0;
  for (i=0; path_info[i].code != EndCode; i++)
  {
    if ((path_info[i].code == MoveToCode) || (path_info[i].code == OpenCode) ||
        (path_info[i].code == GhostlineCode))
      {
        /*
          Move to.
        */
        if ((points != (PointInfo *) NULL) && (n >= 2))
          {
            if (edge == number_edges)
              {
                number_edges<<=1;
                polygon_info->edges=(EdgeInfo *) ResizeQuantumMemory(
                  polygon_info->edges,(size_t) number_edges,
                  sizeof(*polygon_info->edges));
                if (polygon_info->edges == (EdgeInfo *) NULL)
                  {
                    (void) ThrowMagickException(exception,GetMagickModule(),
                      ResourceLimitError,"MemoryAllocationFailed","`%s'","");
                    points=(PointInfo *) RelinquishMagickMemory(points);
                    return(DestroyPolygonInfo(polygon_info));
                  }
              }
            polygon_info->edges[edge].number_points=(size_t) n;
            polygon_info->edges[edge].scanline=(-1.0);
            polygon_info->edges[edge].highwater=0;
            polygon_info->edges[edge].ghostline=ghostline;
            polygon_info->edges[edge].direction=(ssize_t) (direction > 0);
            if (direction < 0)
              ReversePoints(points,(size_t) n);
            polygon_info->edges[edge].points=points;
            polygon_info->edges[edge].bounds=bounds;
            polygon_info->edges[edge].bounds.y1=points[0].y;
            polygon_info->edges[edge].bounds.y2=points[n-1].y;
            points=(PointInfo *) NULL;
            ghostline=MagickFalse;
            edge++;
            polygon_info->number_edges=edge;
          }
        if (points == (PointInfo *) NULL)
          {
            number_points=16;
            points=(PointInfo *) AcquireQuantumMemory((size_t) number_points,
              sizeof(*points));
            if (points == (PointInfo *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed","`%s'","");
                return(DestroyPolygonInfo(polygon_info));
              }
          }
        ghostline=path_info[i].code == GhostlineCode ? MagickTrue : MagickFalse;
        point=path_info[i].point;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        direction=0;
        n=1;
        continue;
      }
    /*
      Line to.
    */
    next_direction=((path_info[i].point.y > point.y) ||
      ((fabs(path_info[i].point.y-point.y) < MagickEpsilon) &&
       (path_info[i].point.x > point.x))) ? 1 : -1;
    if ((points != (PointInfo *) NULL) && (direction != 0) &&
        (direction != next_direction))
      {
        /*
          New edge.
        */
        point=points[n-1];
        if (edge == number_edges)
          {
            number_edges<<=1;
            polygon_info->edges=(EdgeInfo *) ResizeQuantumMemory(
              polygon_info->edges,(size_t) number_edges,
              sizeof(*polygon_info->edges));
            if (polygon_info->edges == (EdgeInfo *) NULL)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed","`%s'","");
                points=(PointInfo *) RelinquishMagickMemory(points);
                return(DestroyPolygonInfo(polygon_info));
              }
          }
        polygon_info->edges[edge].number_points=(size_t) n;
        polygon_info->edges[edge].scanline=(-1.0);
        polygon_info->edges[edge].highwater=0;
        polygon_info->edges[edge].ghostline=ghostline;
        polygon_info->edges[edge].direction=(ssize_t) (direction > 0);
        if (direction < 0)
          ReversePoints(points,(size_t) n);
        polygon_info->edges[edge].points=points;
        polygon_info->edges[edge].bounds=bounds;
        polygon_info->edges[edge].bounds.y1=points[0].y;
        polygon_info->edges[edge].bounds.y2=points[n-1].y;
        polygon_info->number_edges=edge+1;
        points=(PointInfo *) NULL;
        number_points=16;
        points=(PointInfo *) AcquireQuantumMemory((size_t) number_points,
          sizeof(*points));
        if (points == (PointInfo *) NULL)
          {
            (void) ThrowMagickException(exception,GetMagickModule(),
              ResourceLimitError,"MemoryAllocationFailed","`%s'","");
            return(DestroyPolygonInfo(polygon_info));
          }
        n=1;
        ghostline=MagickFalse;
        points[0]=point;
        bounds.x1=point.x;
        bounds.x2=point.x;
        edge++;
      }
    direction=next_direction;
    if (points == (PointInfo *) NULL)
      continue;
    if (n == (ssize_t) number_points)
      {
        number_points<<=1;
        points=(PointInfo *) ResizeQuantumMemory(points,(size_t) number_points,
          sizeof(*points));
        if (points == (PointInfo *) NULL)
          {
            (void) ThrowMagickException(exception,GetMagickModule(),
              ResourceLimitError,"MemoryAllocationFailed","`%s'","");
            return(DestroyPolygonInfo(polygon_info));
          }
      }
    point=path_info[i].point;
    points[n]=point;
    if (point.x < bounds.x1)
      bounds.x1=point.x;
    if (point.x > bounds.x2)
      bounds.x2=point.x;
    n++;
  }
  if (points != (PointInfo *) NULL)
    {
      if (n < 2)
        points=(PointInfo *) RelinquishMagickMemory(points);
      else
        {
          if (edge == number_edges)
            {
              number_edges<<=1;
              polygon_info->edges=(EdgeInfo *) ResizeQuantumMemory(
                polygon_info->edges,(size_t) number_edges,
                sizeof(*polygon_info->edges));
              if (polygon_info->edges == (EdgeInfo *) NULL)
                {
                  (void) ThrowMagickException(exception,GetMagickModule(),
                    ResourceLimitError,"MemoryAllocationFailed","`%s'","");
                  return(DestroyPolygonInfo(polygon_info));
                }
            }
          polygon_info->edges[edge].number_points=(size_t) n;
          polygon_info->edges[edge].scanline=(-1.0);
          polygon_info->edges[edge].highwater=0;
          polygon_info->edges[edge].ghostline=ghostline;
          polygon_info->edges[edge].direction=(ssize_t) (direction > 0);
          if (direction < 0)
            ReversePoints(points,(size_t) n);
          polygon_info->edges[edge].points=points;
          polygon_info->edges[edge].bounds=bounds;
          polygon_info->edges[edge].bounds.y1=points[0].y;
          polygon_info->edges[edge].bounds.y2=points[n-1].y;
          points=(PointInfo *) NULL;
          ghostline=MagickFalse;
          edge++;
          polygon_info->number_edges=edge;
        }
    }
  polygon_info->number_edges=edge;
  polygon_info->edges=(EdgeInfo *) ResizeQuantumMemory(polygon_info->edges,
    polygon_info->number_edges,sizeof(*polygon_info->edges));
  if (polygon_info->edges == (EdgeInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return(DestroyPolygonInfo(polygon_info));
    }
  for (i=0; i < (ssize_t) polygon_info->number_edges; i++)
  {
    EdgeInfo
      *edge_info;

    edge_info=polygon_info->edges+i;
    edge_info->points=(PointInfo *) ResizeQuantumMemory(edge_info->points,
      edge_info->number_points,sizeof(*edge_info->points));
    if (edge_info->points == (PointInfo *) NULL)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'","");
        return(DestroyPolygonInfo(polygon_info));
      }
  }
  qsort(polygon_info->edges,(size_t) polygon_info->number_edges,
    sizeof(*polygon_info->edges),DrawCompareEdges);
  if (IsEventLogging() != MagickFalse)
    LogPolygonInfo(polygon_info);
  return(polygon_info);
}