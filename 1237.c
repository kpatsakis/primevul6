static int XScreenEvent(Display *display,XEvent *event,char *data)
{
  XWindows
    *windows;

  windows=(XWindows *) data;
  if (event->xany.window == windows->popup.id)
    {
      if (event->type == MapNotify)
        windows->popup.mapped=MagickTrue;
      if (event->type == UnmapNotify)
        windows->popup.mapped=MagickFalse;
      return(MagickTrue);
    }
  if (event->xany.window == windows->widget.id)
    {
      if (event->type == MapNotify)
        windows->widget.mapped=MagickTrue;
      if (event->type == UnmapNotify)
        windows->widget.mapped=MagickFalse;
      return(MagickTrue);
    }
  switch (event->type)
  {
    case ButtonPress:
    {
      if ((event->xbutton.button == Button3) &&
          (event->xbutton.state & Mod1Mask))
        {
          /*
            Convert Alt-Button3 to Button2.
          */
          event->xbutton.button=Button2;
          event->xbutton.state&=(~Mod1Mask);
        }
      return(MagickTrue);
    }
    case Expose:
    {
      if (event->xexpose.window == windows->image.id)
        {
          XRefreshWindow(display,&windows->image,event);
          break;
        }
      if (event->xexpose.window == windows->magnify.id)
        if (event->xexpose.count == 0)
          if (windows->magnify.mapped)
            {
              XMakeMagnifyImage(display,windows);
              break;
            }
      if (event->xexpose.window == windows->command.id)
        if (event->xexpose.count == 0)
          {
            (void) XCommandWidget(display,windows,(const char *const *) NULL,
              event);
            break;
          }
      break;
    }
    case FocusOut:
    {
      /*
        Set input focus for backdrop window.
      */
      if (event->xfocus.window == windows->image.id)
        (void) XSetInputFocus(display,windows->image.id,RevertToNone,
          CurrentTime);
      return(MagickTrue);
    }
    case ButtonRelease:
    case KeyPress:
    case KeyRelease:
    case MotionNotify:
    case SelectionNotify:
      return(MagickTrue);
    default:
      break;
  }
  return(MagickFalse);
}