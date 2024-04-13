static void XDrawBevel(Display *display,const XWindowInfo *window_info,
  const XWidgetInfo *bevel_info)
{
  int
    x1,
    x2,
    y1,
    y2;

  unsigned int
    bevel_width;

  XPoint
    points[6];

  /*
    Draw upper and left beveled border.
  */
  x1=bevel_info->x;
  y1=bevel_info->y+bevel_info->height;
  x2=bevel_info->x+bevel_info->width;
  y2=bevel_info->y;
  bevel_width=bevel_info->bevel_width;
  points[0].x=x1;
  points[0].y=y1;
  points[1].x=x1;
  points[1].y=y2;
  points[2].x=x2;
  points[2].y=y2;
  points[3].x=x2+bevel_width;
  points[3].y=y2-bevel_width;
  points[4].x=x1-bevel_width;
  points[4].y=y2-bevel_width;
  points[5].x=x1-bevel_width;
  points[5].y=y1+bevel_width;
  XSetBevelColor(display,window_info,bevel_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,6,Complex,CoordModeOrigin);
  /*
    Draw lower and right beveled border.
  */
  points[0].x=x1;
  points[0].y=y1;
  points[1].x=x2;
  points[1].y=y1;
  points[2].x=x2;
  points[2].y=y2;
  points[3].x=x2+bevel_width;
  points[3].y=y2-bevel_width;
  points[4].x=x2+bevel_width;
  points[4].y=y1+bevel_width;
  points[5].x=x1-bevel_width;
  points[5].y=y1+bevel_width;
  XSetBevelColor(display,window_info,!bevel_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,6,Complex,CoordModeOrigin);
  (void) XSetFillStyle(display,window_info->widget_context,FillSolid);
}