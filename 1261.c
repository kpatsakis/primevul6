static void XSetTextColor(Display *display,const XWindowInfo *window_info,
  const MagickStatusType raised)
{
  ssize_t
    foreground,
    matte;

  if (window_info->depth == 1)
    {
      /*
        Monochrome window.
      */
      if (raised)
        (void) XSetForeground(display,window_info->widget_context,
          XBlackPixel(display,window_info->screen));
      else
        (void) XSetForeground(display,window_info->widget_context,
          XWhitePixel(display,window_info->screen));
      return;
    }
  foreground=(ssize_t) XPixelIntensity(
    &window_info->pixel_info->foreground_color);
  matte=(ssize_t) XPixelIntensity(&window_info->pixel_info->matte_color);
  if (MagickAbsoluteValue((int) (foreground-matte)) > (65535L >> 3))
    (void) XSetForeground(display,window_info->widget_context,
      window_info->pixel_info->foreground_color.pixel);
  else
    (void) XSetForeground(display,window_info->widget_context,
      window_info->pixel_info->background_color.pixel);
}