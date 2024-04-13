static void XDrawTriangleNorth(Display *display,const XWindowInfo *window_info,
  const XWidgetInfo *triangle_info)
{
  int
    x1,
    x2,
    x3,
    y1,
    y2,
    y3;

  unsigned int
    bevel_width;

  XPoint
    points[4];

  /*
    Draw triangle matte.
  */
  x1=triangle_info->x;
  y1=triangle_info->y+triangle_info->height;
  x2=triangle_info->x+(triangle_info->width >> 1);
  y2=triangle_info->y;
  x3=triangle_info->x+triangle_info->width;
  y3=triangle_info->y+triangle_info->height;
  bevel_width=triangle_info->bevel_width;
  points[0].x=x1;
  points[0].y=y1;
  points[1].x=x2;
  points[1].y=y2;
  points[2].x=x3;
  points[2].y=y3;
  XSetMatteColor(display,window_info,triangle_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,3,Complex,CoordModeOrigin);
  /*
    Draw left bevel.
  */
  points[0].x=x1;
  points[0].y=y1;
  points[1].x=x2;
  points[1].y=y2;
  points[2].x=x2;
  points[2].y=y2-bevel_width-2;
  points[3].x=x1-bevel_width-1;
  points[3].y=y1+bevel_width;
  XSetBevelColor(display,window_info,triangle_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,4,Complex,CoordModeOrigin);
  /*
    Draw right bevel.
  */
  points[0].x=x2;
  points[0].y=y2;
  points[1].x=x3;
  points[1].y=y3;
  points[2].x=x3+bevel_width;
  points[2].y=y3+bevel_width;
  points[3].x=x2;
  points[3].y=y2-bevel_width;
  XSetBevelColor(display,window_info,!triangle_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,4,Complex,CoordModeOrigin);
  /*
    Draw lower bevel.
  */
  points[0].x=x3;
  points[0].y=y3;
  points[1].x=x1;
  points[1].y=y1;
  points[2].x=x1-bevel_width;
  points[2].y=y1+bevel_width;
  points[3].x=x3+bevel_width;
  points[3].y=y3+bevel_width;
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,4,Complex,CoordModeOrigin);
  (void) XSetFillStyle(display,window_info->widget_context,FillSolid);
}