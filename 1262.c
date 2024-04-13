static void XHighlightWidget(Display *display,const XWindowInfo *window_info,
  const int x,const int y)
{
  /*
    Draw the widget highlighting rectangle.
  */
  XSetBevelColor(display,window_info,MagickTrue);
  (void) XDrawRectangle(display,window_info->id,window_info->widget_context,x,y,
    window_info->width-(x << 1),window_info->height-(y << 1));
  (void) XDrawRectangle(display,window_info->id,window_info->widget_context,
    x-1,y-1,window_info->width-(x << 1)+1,window_info->height-(y << 1)+1);
  XSetBevelColor(display,window_info,MagickFalse);
  (void) XDrawRectangle(display,window_info->id,window_info->widget_context,
    x-1,y-1,window_info->width-(x << 1),window_info->height-(y << 1));
  (void) XSetFillStyle(display,window_info->widget_context,FillSolid);
}