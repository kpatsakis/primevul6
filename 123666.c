static size_t DestroyEdge(PolygonInfo *polygon_info,const ssize_t edge)
{
  assert(edge < (ssize_t) polygon_info->number_edges);
  polygon_info->edges[edge].points=(PointInfo *) RelinquishMagickMemory(
    polygon_info->edges[edge].points);
  polygon_info->number_edges--;
  if (edge < (ssize_t) polygon_info->number_edges)
    (void) memmove(polygon_info->edges+edge,polygon_info->edges+edge+1,
      (size_t) (polygon_info->number_edges-edge)*sizeof(*polygon_info->edges));
  return(polygon_info->number_edges);
}