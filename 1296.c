static void XDrawBeveledButton(Display *display,const XWindowInfo *window_info,
  const XWidgetInfo *button_info)
{
  int
    x,
    y;

  unsigned int
    width;

  XFontStruct
    *font_info;

  XRectangle
    crop_info;

  /*
    Draw matte.
  */
  XDrawBevel(display,window_info,button_info);
  XSetMatteColor(display,window_info,button_info->raised);
  (void) XFillRectangle(display,window_info->id,window_info->widget_context,
    button_info->x,button_info->y,button_info->width,button_info->height);
  x=button_info->x-button_info->bevel_width-1;
  y=button_info->y-button_info->bevel_width-1;
  (void) XSetForeground(display,window_info->widget_context,
    window_info->pixel_info->trough_color.pixel);
  if (button_info->raised || (window_info->depth == 1))
    (void) XDrawRectangle(display,window_info->id,window_info->widget_context,
      x,y,button_info->width+(button_info->bevel_width << 1)+1,
      button_info->height+(button_info->bevel_width << 1)+1);
  if (button_info->text == (char *) NULL)
    return;
  /*
    Set cropping region.
  */
  crop_info.width=(unsigned short) button_info->width;
  crop_info.height=(unsigned short) button_info->height;
  crop_info.x=button_info->x;
  crop_info.y=button_info->y;
  /*
    Draw text.
  */
  font_info=window_info->font_info;
  width=WidgetTextWidth(font_info,button_info->text);
  x=button_info->x+(QuantumMargin >> 1);
  if (button_info->center)
    x=button_info->x+(button_info->width >> 1)-(width >> 1);
  y=button_info->y+((button_info->height-
    (font_info->ascent+font_info->descent)) >> 1)+font_info->ascent;
  if ((int) button_info->width == (QuantumMargin >> 1))
    {
      /*
        Option button-- write label to right of button.
      */
      XSetTextColor(display,window_info,MagickTrue);
      x=button_info->x+button_info->width+button_info->bevel_width+
        (QuantumMargin >> 1);
      (void) XDrawString(display,window_info->id,window_info->widget_context,
        x,y,button_info->text,Extent(button_info->text));
      return;
    }
  (void) XSetClipRectangles(display,window_info->widget_context,0,0,&crop_info,
    1,Unsorted);
  XSetTextColor(display,window_info,button_info->raised);
  (void) XDrawString(display,window_info->id,window_info->widget_context,x,y,
    button_info->text,Extent(button_info->text));
  (void) XSetClipMask(display,window_info->widget_context,None);
  if (button_info->raised == MagickFalse)
    XDelay(display,SuspendTime << 2);
}