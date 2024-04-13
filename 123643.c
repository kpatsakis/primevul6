static void ReversePoints(PointInfo *points,const size_t number_points)
{
  PointInfo
    point;

  ssize_t
    i;

  for (i=0; i < (ssize_t) (number_points >> 1); i++)
  {
    point=points[i];
    points[i]=points[number_points-(i+1)];
    points[number_points-(i+1)]=point;
  }
}