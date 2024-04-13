static PolygonInfo *DestroyPolygonInfo(PolygonInfo *polygon_info)
{
  ssize_t
    i;

  if (polygon_info->edges != (EdgeInfo *) NULL)
    {
      for (i=0; i < (ssize_t) polygon_info->number_edges; i++)
        if (polygon_info->edges[i].points != (PointInfo *) NULL)
          polygon_info->edges[i].points=(PointInfo *)
            RelinquishMagickMemory(polygon_info->edges[i].points);
      polygon_info->edges=(EdgeInfo *) RelinquishMagickMemory(
        polygon_info->edges);
    }
  return((PolygonInfo *) RelinquishMagickMemory(polygon_info));
}