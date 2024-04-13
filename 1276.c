static void XSetBevelColor(Display *display,const XWindowInfo *window_info,
  const MagickStatusType raised)
{
  if (window_info->depth == 1)
    {
      Pixmap
        stipple;

      /*
        Monochrome window.
      */
      (void) XSetBackground(display,window_info->widget_context,
        XBlackPixel(display,window_info->screen));
      (void) XSetForeground(display,window_info->widget_context,
        XWhitePixel(display,window_info->screen));
      (void) XSetFillStyle(display,window_info->widget_context,
        FillOpaqueStippled);
      stipple=window_info->highlight_stipple;
      if (raised == MagickFalse)
        stipple=window_info->shadow_stipple;
      (void) XSetStipple(display,window_info->widget_context,stipple);
    }
  else
    if (raised)
      (void) XSetForeground(display,window_info->widget_context,
        window_info->pixel_info->highlight_color.pixel);
    else
      (void) XSetForeground(display,window_info->widget_context,
        window_info->pixel_info->shadow_color.pixel);
}