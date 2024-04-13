static void XDrawTriangleEast(Display *display,const XWindowInfo *window_info,
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

  XFontStruct
    *font_info;

  XPoint
    points[4];

  /*
    Draw triangle matte.
  */
  x1=triangle_info->x;
  y1=triangle_info->y;
  x2=triangle_info->x+triangle_info->width;
  y2=triangle_info->y+(triangle_info->height >> 1);
  x3=triangle_info->x;
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
    Draw bottom bevel.
  */
  points[0].x=x2;
  points[0].y=y2;
  points[1].x=x3;
  points[1].y=y3;
  points[2].x=x3-bevel_width;
  points[2].y=y3+bevel_width;
  points[3].x=x2+bevel_width;
  points[3].y=y2;
  XSetBevelColor(display,window_info,!triangle_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,4,Complex,CoordModeOrigin);
  /*
    Draw Left bevel.
  */
  points[0].x=x3;
  points[0].y=y3;
  points[1].x=x1;
  points[1].y=y1;
  points[2].x=x1-bevel_width+1;
  points[2].y=y1-bevel_width;
  points[3].x=x3-bevel_width+1;
  points[3].y=y3+bevel_width;
  XSetBevelColor(display,window_info,triangle_info->raised);
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,4,Complex,CoordModeOrigin);
  /*
    Draw top bevel.
  */
  points[0].x=x1;
  points[0].y=y1;
  points[1].x=x2;
  points[1].y=y2;
  points[2].x=x2+bevel_width;
  points[2].y=y2;
  points[3].x=x1-bevel_width;
  points[3].y=y1-bevel_width;
  (void) XFillPolygon(display,window_info->id,window_info->widget_context,
    points,4,Complex,CoordModeOrigin);
  (void) XSetFillStyle(display,window_info->widget_context,FillSolid);
  if (triangle_info->text == (char *) NULL)
    return;
  /*
    Write label to right of triangle.
  */
  font_info=window_info->font_info;
  XSetTextColor(display,window_info,MagickTrue);
  x1=triangle_info->x+triangle_info->width+triangle_info->bevel_width+
    (QuantumMargin >> 1);
  y1=triangle_info->y+((triangle_info->height-
    (font_info->ascent+font_info->descent)) >> 1)+font_info->ascent;
  (void) XDrawString(display,window_info->id,window_info->widget_context,x1,y1,
    triangle_info->text,Extent(triangle_info->text));
}