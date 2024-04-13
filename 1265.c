MagickExport MagickBooleanType XPreferencesWidget(Display *display,
  XResourceInfo *resource_info,XWindows *windows)
{
#define ApplyButtonText  "Apply"
#define CacheButtonText  "%lu mega-bytes of memory in the undo edit cache   "
#define CancelButtonText  "Cancel"
#define NumberPreferences  8

  static const char
    *Preferences[] =
    {
      "display image centered on a backdrop",
      "confirm on program exit",
      "confirm on image edits",
      "correct image for display gamma",
      "display warning messages",
      "apply Floyd/Steinberg error diffusion to image",
      "use a shared colormap for colormapped X visuals",
      "display images as an X server pixmap"
    };

  char
    cache[MaxTextExtent];

  int
    x,
    y;

  int
    i;

  Status
    status;

  unsigned int
    height,
    text_width,
    width;

  size_t
    state;

  XEvent
    event;

  XFontStruct
    *font_info;

  XTextProperty
    window_name;

  XWidgetInfo
    apply_info,
    cache_info,
    cancel_info,
    preferences_info[NumberPreferences];

  XWindowChanges
    window_changes;

  /*
    Determine Preferences widget attributes.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(windows != (XWindows *) NULL);
  XCheckRefreshWindows(display,windows);
  font_info=windows->widget.font_info;
  text_width=WidgetTextWidth(font_info,CacheButtonText);
  for (i=0; i < NumberPreferences; i++)
    if (WidgetTextWidth(font_info,(char *) Preferences[i]) > text_width)
      text_width=WidgetTextWidth(font_info,(char *) Preferences[i]);
  width=WidgetTextWidth(font_info,ApplyButtonText);
  if (WidgetTextWidth(font_info,CancelButtonText) > width)
    width=WidgetTextWidth(font_info,CancelButtonText);
  width+=(unsigned int) QuantumMargin;
  height=(unsigned int) (font_info->ascent+font_info->descent);
  /*
    Position Preferences widget.
  */
  windows->widget.width=(unsigned int) (MagickMax((int) (width << 1),
    (int) text_width)+6*QuantumMargin);
  windows->widget.min_width=(width << 1)+QuantumMargin;
  if (windows->widget.width < windows->widget.min_width)
    windows->widget.width=windows->widget.min_width;
  windows->widget.height=(unsigned int)
    (7*height+NumberPreferences*(height+(QuantumMargin >> 1)));
  windows->widget.min_height=(unsigned int)
    (7*height+NumberPreferences*(height+(QuantumMargin >> 1)));
  if (windows->widget.height < windows->widget.min_height)
    windows->widget.height=windows->widget.min_height;
  XConstrainWindowPosition(display,&windows->widget);
  /*
    Map Preferences widget.
  */
  (void) CopyMagickString(windows->widget.name,"Preferences",MaxTextExtent);
  status=XStringListToTextProperty(&windows->widget.name,1,&window_name);
  if (status != False)
    {
      XSetWMName(display,windows->widget.id,&window_name);
      XSetWMIconName(display,windows->widget.id,&window_name);
      (void) XFree((void *) window_name.value);
    }
  window_changes.width=(int) windows->widget.width;
  window_changes.height=(int) windows->widget.height;
  window_changes.x=windows->widget.x;
  window_changes.y=windows->widget.y;
  (void) XReconfigureWMWindow(display,windows->widget.id,windows->widget.screen,
    (unsigned int) (CWWidth | CWHeight | CWX | CWY),&window_changes);
  (void) XMapRaised(display,windows->widget.id);
  windows->widget.mapped=MagickFalse;
  /*
    Respond to X events.
  */
  state=UpdateConfigurationState;
  XSetCursorState(display,windows,MagickTrue);
  do
  {
    if (state & UpdateConfigurationState)
      {
        /*
          Initialize button information.
        */
        XGetWidgetInfo(CancelButtonText,&cancel_info);
        cancel_info.width=width;
        cancel_info.height=(unsigned int) (3*height) >> 1;
        cancel_info.x=(int) windows->widget.width-cancel_info.width-
          (QuantumMargin << 1);
        cancel_info.y=(int) windows->widget.height-
          cancel_info.height-QuantumMargin;
        XGetWidgetInfo(ApplyButtonText,&apply_info);
        apply_info.width=width;
        apply_info.height=(unsigned int) (3*height) >> 1;
        apply_info.x=QuantumMargin << 1;
        apply_info.y=cancel_info.y;
        y=(int) (height << 1);
        for (i=0; i < NumberPreferences; i++)
        {
          XGetWidgetInfo(Preferences[i],&preferences_info[i]);
          preferences_info[i].bevel_width--;
          preferences_info[i].width=(unsigned int) QuantumMargin >> 1;
          preferences_info[i].height=(unsigned int) QuantumMargin >> 1;
          preferences_info[i].x=QuantumMargin << 1;
          preferences_info[i].y=y;
          y+=height+(QuantumMargin >> 1);
        }
        preferences_info[0].raised=resource_info->backdrop ==
          MagickFalse ? MagickTrue : MagickFalse;
        preferences_info[1].raised=resource_info->confirm_exit ==
          MagickFalse ? MagickTrue : MagickFalse;
        preferences_info[2].raised=resource_info->confirm_edit ==
          MagickFalse ? MagickTrue : MagickFalse;
        preferences_info[3].raised=resource_info->gamma_correct ==
          MagickFalse ? MagickTrue : MagickFalse;
        preferences_info[4].raised=resource_info->display_warnings ==
          MagickFalse ? MagickTrue : MagickFalse;
        preferences_info[5].raised=resource_info->quantize_info->dither ==
          MagickFalse ? MagickTrue : MagickFalse;
        preferences_info[6].raised=resource_info->colormap !=
          SharedColormap ? MagickTrue : MagickFalse;
        preferences_info[7].raised=resource_info->use_pixmap ==
          MagickFalse ? MagickTrue : MagickFalse;
        (void) FormatLocaleString(cache,MaxTextExtent,CacheButtonText,
          (unsigned long) resource_info->undo_cache);
        XGetWidgetInfo(cache,&cache_info);
        cache_info.bevel_width--;
        cache_info.width=(unsigned int) QuantumMargin >> 1;
        cache_info.height=(unsigned int) QuantumMargin >> 1;
        cache_info.x=QuantumMargin << 1;
        cache_info.y=y;
        state&=(~UpdateConfigurationState);
      }
    if (state & RedrawWidgetState)
      {
        /*
          Redraw Preferences widget.
        */
        XDrawBeveledButton(display,&windows->widget,&apply_info);
        XDrawBeveledButton(display,&windows->widget,&cancel_info);
        for (i=0; i < NumberPreferences; i++)
          XDrawBeveledButton(display,&windows->widget,&preferences_info[i]);
        XDrawTriangleEast(display,&windows->widget,&cache_info);
        XHighlightWidget(display,&windows->widget,BorderOffset,BorderOffset);
        state&=(~RedrawWidgetState);
      }
    /*
      Wait for next event.
    */
    (void) XIfEvent(display,&event,XScreenEvent,(char *) windows);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (MatteIsActive(apply_info,event.xbutton))
          {
            /*
              User pressed Apply button.
            */
            apply_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&apply_info);
            break;
          }
        if (MatteIsActive(cancel_info,event.xbutton))
          {
            /*
              User pressed Cancel button.
            */
            cancel_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
            break;
          }
        for (i=0; i < NumberPreferences; i++)
          if (MatteIsActive(preferences_info[i],event.xbutton))
            {
              /*
                User pressed a Preferences button.
              */
              preferences_info[i].raised=preferences_info[i].raised ==
                MagickFalse ? MagickTrue : MagickFalse;
              XDrawBeveledButton(display,&windows->widget,&preferences_info[i]);
              break;
            }
        if (MatteIsActive(cache_info,event.xbutton))
          {
            /*
              User pressed Cache button.
            */
            x=cache_info.x+cache_info.width+cache_info.bevel_width+
              (QuantumMargin >> 1);
            y=cache_info.y+((cache_info.height-height) >> 1);
            width=WidgetTextWidth(font_info,cache);
            (void) XClearArea(display,windows->widget.id,x,y,width,height,
              False);
            resource_info->undo_cache<<=1;
            if (resource_info->undo_cache > 256)
              resource_info->undo_cache=1;
            (void) FormatLocaleString(cache,MaxTextExtent,CacheButtonText,
              (unsigned long) resource_info->undo_cache);
            cache_info.raised=MagickFalse;
            XDrawTriangleEast(display,&windows->widget,&cache_info);
            break;
          }
        break;
      }
      case ButtonRelease:
      {
        if (windows->widget.mapped == MagickFalse)
          break;
        if (apply_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(apply_info,event.xbutton))
                state|=ExitState;
            apply_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&apply_info);
            apply_info.raised=MagickFalse;
          }
        if (cancel_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(cancel_info,event.xbutton))
                state|=ExitState;
            cancel_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
          }
        if (cache_info.raised == MagickFalse)
          {
            cache_info.raised=MagickTrue;
            XDrawTriangleEast(display,&windows->widget,&cache_info);
          }
        break;
      }
      case ClientMessage:
      {
        /*
          If client window delete message, exit.
        */
        if (event.xclient.message_type != windows->wm_protocols)
          break;
        if (*event.xclient.data.l == (int) windows->wm_take_focus)
          {
            (void) XSetInputFocus(display,event.xclient.window,RevertToParent,
              (Time) event.xclient.data.l[1]);
            break;
          }
        if (*event.xclient.data.l != (int) windows->wm_delete_window)
          break;
        if (event.xclient.window == windows->widget.id)
          {
            state|=ExitState;
            break;
          }
        break;
      }
      case ConfigureNotify:
      {
        /*
          Update widget configuration.
        */
        if (event.xconfigure.window != windows->widget.id)
          break;
        if ((event.xconfigure.width == (int) windows->widget.width) &&
            (event.xconfigure.height == (int) windows->widget.height))
          break;
        windows->widget.width=(unsigned int)
          MagickMax(event.xconfigure.width,(int) windows->widget.min_width);
        windows->widget.height=(unsigned int)
          MagickMax(event.xconfigure.height,(int) windows->widget.min_height);
        state|=UpdateConfigurationState;
        break;
      }
      case EnterNotify:
      {
        if (event.xcrossing.window != windows->widget.id)
          break;
        state&=(~InactiveWidgetState);
        break;
      }
      case Expose:
      {
        if (event.xexpose.window != windows->widget.id)
          break;
        if (event.xexpose.count != 0)
          break;
        state|=RedrawWidgetState;
        break;
      }
      case KeyPress:
      {
        static char
          command[MaxTextExtent];

        static KeySym
          key_symbol;

        /*
          Respond to a user key press.
        */
        if (event.xkey.window != windows->widget.id)
          break;
        (void) XLookupString((XKeyEvent *) &event.xkey,command,
          (int) sizeof(command),&key_symbol,(XComposeStatus *) NULL);
        if ((key_symbol == XK_Return) || (key_symbol == XK_KP_Enter))
          {
            apply_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&apply_info);
            state|=ExitState;
            break;
          }
        break;
      }
      case LeaveNotify:
      {
        if (event.xcrossing.window != windows->widget.id)
          break;
        state|=InactiveWidgetState;
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,&event)) ;
        if (state & InactiveWidgetState)
          break;
        if (apply_info.raised == MatteIsActive(apply_info,event.xmotion))
          {
            /*
              Apply button status changed.
            */
            apply_info.raised=
              apply_info.raised == MagickFalse ? MagickTrue : MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&apply_info);
            break;
          }
        if (cancel_info.raised == MatteIsActive(cancel_info,event.xmotion))
          {
            /*
              Cancel button status changed.
            */
            cancel_info.raised=
              cancel_info.raised == MagickFalse ? MagickTrue : MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
            break;
          }
        break;
      }
      default:
        break;
    }
  } while ((state & ExitState) == 0);
  XSetCursorState(display,windows,MagickFalse);
  (void) XWithdrawWindow(display,windows->widget.id,windows->widget.screen);
  XCheckRefreshWindows(display,windows);
  if (apply_info.raised)
    return(MagickFalse);
  /*
    Save user preferences to the client configuration file.
  */
  resource_info->backdrop=
    preferences_info[0].raised == MagickFalse ? MagickTrue : MagickFalse;
  resource_info->confirm_exit=
    preferences_info[1].raised == MagickFalse ? MagickTrue : MagickFalse;
  resource_info->confirm_edit=
    preferences_info[2].raised == MagickFalse ? MagickTrue : MagickFalse;
  resource_info->gamma_correct=
    preferences_info[3].raised == MagickFalse ? MagickTrue : MagickFalse;
  resource_info->display_warnings=
     preferences_info[4].raised == MagickFalse ? MagickTrue : MagickFalse;
  resource_info->quantize_info->dither=
    preferences_info[5].raised == MagickFalse ? MagickTrue : MagickFalse;
  resource_info->colormap=SharedColormap;
  if (preferences_info[6].raised)
    resource_info->colormap=PrivateColormap;
  resource_info->use_pixmap=
    preferences_info[7].raised == MagickFalse ? MagickTrue : MagickFalse;
  XUserPreferences(resource_info);
  return(MagickTrue);
}