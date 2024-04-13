static PolygonInfo **AcquirePolygonThreadSet(
  const PrimitiveInfo *primitive_info,ExceptionInfo *exception)
{
  PathInfo
    *magick_restrict path_info;

  PolygonInfo
    **polygon_info;

  ssize_t
    i;

  size_t
    number_threads;

  number_threads=(size_t) GetMagickResourceLimit(ThreadResource);
  polygon_info=(PolygonInfo **) AcquireQuantumMemory(number_threads,
    sizeof(*polygon_info));
  if (polygon_info == (PolygonInfo **) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return((PolygonInfo **) NULL);
    }
  (void) memset(polygon_info,0,number_threads*sizeof(*polygon_info));
  path_info=ConvertPrimitiveToPath(primitive_info,exception);
  if (path_info == (PathInfo *) NULL)
    return(DestroyPolygonThreadSet(polygon_info));
  polygon_info[0]=ConvertPathToPolygon(path_info,exception);
  if (polygon_info[0] == (PolygonInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'","");
      return(DestroyPolygonThreadSet(polygon_info));
    }
  for (i=1; i < (ssize_t) number_threads; i++)
  {
    EdgeInfo
      *edge_info;

    ssize_t
      j;

    polygon_info[i]=(PolygonInfo *) AcquireMagickMemory(
      sizeof(*polygon_info[i]));
    if (polygon_info[i] == (PolygonInfo *) NULL)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'","");
        return(DestroyPolygonThreadSet(polygon_info));
      }
    polygon_info[i]->number_edges=0;
    edge_info=polygon_info[0]->edges;
    polygon_info[i]->edges=(EdgeInfo *) AcquireQuantumMemory(
      polygon_info[0]->number_edges,sizeof(*edge_info));
    if (polygon_info[i]->edges == (EdgeInfo *) NULL)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'","");
        return(DestroyPolygonThreadSet(polygon_info));
      }
    (void) memcpy(polygon_info[i]->edges,edge_info,
      polygon_info[0]->number_edges*sizeof(*edge_info));
    for (j=0; j < (ssize_t) polygon_info[i]->number_edges; j++)
      polygon_info[i]->edges[j].points=(PointInfo *) NULL;
    polygon_info[i]->number_edges=polygon_info[0]->number_edges;
    for (j=0; j < (ssize_t) polygon_info[i]->number_edges; j++)
    {
      edge_info=polygon_info[0]->edges+j;
      polygon_info[i]->edges[j].points=(PointInfo *) AcquireQuantumMemory(
        edge_info->number_points,sizeof(*edge_info));
      if (polygon_info[i]->edges[j].points == (PointInfo *) NULL)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'","");
          return(DestroyPolygonThreadSet(polygon_info));
        }
      (void) memcpy(polygon_info[i]->edges[j].points,edge_info->points,
        edge_info->number_points*sizeof(*edge_info->points));
    }
  }
  path_info=(PathInfo *) RelinquishMagickMemory(path_info);
  return(polygon_info);
}