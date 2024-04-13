static void XDrawMatteText(Display *display,const XWindowInfo *window_info,
  XWidgetInfo *text_info)
{
  const char
    *text;

  int
    n,
    x,
    y;

  int
    i;

  unsigned int
    height,
    width;

  XFontStruct
    *font_info;

  XRectangle
    crop_info;

  /*
    Clear the text area.
  */
  XSetMatteColor(display,window_info,MagickFalse);
  (void) XFillRectangle(display,window_info->id,window_info->widget_context,
    text_info->x,text_info->y,text_info->width,text_info->height);
  if (text_info->text == (char *) NULL)
    return;
  XSetTextColor(display,window_info,text_info->highlight);
  font_info=window_info->font_info;
  x=text_info->x+(QuantumMargin >> 2);
  y=text_info->y+font_info->ascent+(text_info->height >> 2);
  width=text_info->width-(QuantumMargin >> 1);
  height=(unsigned int) (font_info->ascent+font_info->descent);
  if (*text_info->text == '\0')
    {
      /*
        No text-- just draw cursor.
      */
      (void) XDrawLine(display,window_info->id,window_info->annotate_context,
        x,y+3,x,y-height+3);
      return;
    }
  /*
    Set cropping region.
  */
  crop_info.width=(unsigned short) text_info->width;
  crop_info.height=(unsigned short) text_info->height;
  crop_info.x=text_info->x;
  crop_info.y=text_info->y;
  /*
    Determine beginning of the visible text.
  */
  if (text_info->cursor < text_info->marker)
    text_info->marker=text_info->cursor;
  else
    {
      text=text_info->marker;
      if (XTextWidth(font_info,(char *) text,(int) (text_info->cursor-text)) >
          (int) width)
        {
          text=text_info->text;
          for (i=0; i < Extent(text); i++)
          {
            n=XTextWidth(font_info,(char *) text+i,(int)
              (text_info->cursor-text-i));
            if (n <= (int) width)
              break;
          }
          text_info->marker=(char *) text+i;
        }
    }
  /*
    Draw text and cursor.
  */
  if (text_info->highlight == MagickFalse)
    {
      (void) XSetClipRectangles(display,window_info->widget_context,0,0,
        &crop_info,1,Unsorted);
      (void) XDrawString(display,window_info->id,window_info->widget_context,
        x,y,text_info->marker,Extent(text_info->marker));
      (void) XSetClipMask(display,window_info->widget_context,None);
    }
  else
    {
      (void) XSetClipRectangles(display,window_info->annotate_context,0,0,
        &crop_info,1,Unsorted);
      width=WidgetTextWidth(font_info,text_info->marker);
      (void) XFillRectangle(display,window_info->id,
        window_info->annotate_context,x,y-font_info->ascent,width,height);
      (void) XSetClipMask(display,window_info->annotate_context,None);
      (void) XSetClipRectangles(display,window_info->highlight_context,0,0,
        &crop_info,1,Unsorted);
      (void) XDrawString(display,window_info->id,
        window_info->highlight_context,x,y,text_info->marker,
        Extent(text_info->marker));
      (void) XSetClipMask(display,window_info->highlight_context,None);
    }
  x+=XTextWidth(font_info,text_info->marker,(int)
    (text_info->cursor-text_info->marker));
  (void) XDrawLine(display,window_info->id,window_info->annotate_context,x,y+3,
    x,y-height+3);
}