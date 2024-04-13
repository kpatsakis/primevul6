MagickExport int XConfirmWidget(Display *display,XWindows *windows,
  const char *reason,const char *description)
{
#define CancelButtonText  "Cancel"
#define DismissButtonText  "Dismiss"
#define YesButtonText  "Yes"

  int
    confirm,
    x,
    y;

  Status
    status;

  unsigned int
    height,
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
    cancel_info,
    dismiss_info,
    yes_info;

  XWindowChanges
    window_changes;

  /*
    Determine Confirm widget attributes.
  */
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  assert(reason != (char *) NULL);
  assert(description != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",reason);
  XCheckRefreshWindows(display,windows);
  font_info=windows->widget.font_info;
  width=WidgetTextWidth(font_info,CancelButtonText);
  if (WidgetTextWidth(font_info,DismissButtonText) > width)
    width=WidgetTextWidth(font_info,DismissButtonText);
  if (WidgetTextWidth(font_info,YesButtonText) > width)
    width=WidgetTextWidth(font_info,YesButtonText);
  width<<=1;
  if (description != (char *) NULL)
    if (WidgetTextWidth(font_info,(char *) description) > width)
      width=WidgetTextWidth(font_info,(char *) description);
  height=(unsigned int) (font_info->ascent+font_info->descent);
  /*
    Position Confirm widget.
  */
  windows->widget.width=(unsigned int) (width+9*QuantumMargin);
  windows->widget.min_width=(unsigned int) (9*QuantumMargin+
    WidgetTextWidth(font_info,CancelButtonText)+
    WidgetTextWidth(font_info,DismissButtonText)+
    WidgetTextWidth(font_info,YesButtonText));
  if (windows->widget.width < windows->widget.min_width)
    windows->widget.width=windows->widget.min_width;
  windows->widget.height=(unsigned int) (12*height);
  windows->widget.min_height=(unsigned int) (7*height);
  if (windows->widget.height < windows->widget.min_height)
    windows->widget.height=windows->widget.min_height;
  XConstrainWindowPosition(display,&windows->widget);
  /*
    Map Confirm widget.
  */
  (void) CopyMagickString(windows->widget.name,"Confirm",MaxTextExtent);
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
  confirm=0;
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
        cancel_info.width=(unsigned int) QuantumMargin+
          WidgetTextWidth(font_info,CancelButtonText);
        cancel_info.height=(unsigned int) ((3*height) >> 1);
        cancel_info.x=(int) (windows->widget.width-cancel_info.width-
          QuantumMargin);
        cancel_info.y=(int) (windows->widget.height-(cancel_info.height << 1));
        dismiss_info=cancel_info;
        dismiss_info.text=(char *) DismissButtonText;
        if (LocaleCompare(description,"Do you want to save it") == 0)
          dismiss_info.text=(char *) "Don't Save";
        dismiss_info.width=(unsigned int) QuantumMargin+
          WidgetTextWidth(font_info,dismiss_info.text);
        dismiss_info.x=(int)
          ((windows->widget.width >> 1)-(dismiss_info.width >> 1));
        yes_info=cancel_info;
        yes_info.text=(char *) YesButtonText;
        if (LocaleCompare(description,"Do you want to save it") == 0)
          yes_info.text=(char *) "Save";
        yes_info.width=(unsigned int) QuantumMargin+
          WidgetTextWidth(font_info,yes_info.text);
        if (yes_info.width < cancel_info.width)
          yes_info.width=cancel_info.width;
        yes_info.x=QuantumMargin;
        state&=(~UpdateConfigurationState);
      }
    if (state & RedrawWidgetState)
      {
        /*
          Redraw Confirm widget.
        */
        width=WidgetTextWidth(font_info,(char *) reason);
        x=(int) ((windows->widget.width >> 1)-(width >> 1));
        y=(int) ((windows->widget.height >> 1)-(height << 1));
        (void) XDrawString(display,windows->widget.id,
          windows->widget.annotate_context,x,y,(char *) reason,Extent(reason));
        if (description != (char *) NULL)
          {
            char
              question[MaxTextExtent];

            (void) CopyMagickString(question,description,MaxTextExtent);
            (void) ConcatenateMagickString(question,"?",MaxTextExtent);
            width=WidgetTextWidth(font_info,question);
            x=(int) ((windows->widget.width >> 1)-(width >> 1));
            y+=height;
            (void) XDrawString(display,windows->widget.id,
              windows->widget.annotate_context,x,y,question,Extent(question));
          }
        XDrawBeveledButton(display,&windows->widget,&cancel_info);
        XDrawBeveledButton(display,&windows->widget,&dismiss_info);
        XDrawBeveledButton(display,&windows->widget,&yes_info);
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
        if (MatteIsActive(cancel_info,event.xbutton))
          {
            /*
              User pressed No button.
            */
            cancel_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
            break;
          }
        if (MatteIsActive(dismiss_info,event.xbutton))
          {
            /*
              User pressed Dismiss button.
            */
            dismiss_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&dismiss_info);
            break;
          }
        if (MatteIsActive(yes_info,event.xbutton))
          {
            /*
              User pressed Yes button.
            */
            yes_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&yes_info);
            break;
          }
        break;
      }
      case ButtonRelease:
      {
        if (windows->widget.mapped == MagickFalse)
          break;
        if (cancel_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(cancel_info,event.xbutton))
                {
                  confirm=0;
                  state|=ExitState;
                }
            cancel_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
          }
        if (dismiss_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(dismiss_info,event.xbutton))
                {
                  confirm=(-1);
                  state|=ExitState;
                }
            dismiss_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&dismiss_info);
          }
        if (yes_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(yes_info,event.xbutton))
                {
                  confirm=1;
                  state|=ExitState;
                }
            yes_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&yes_info);
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
            yes_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&yes_info);
            confirm=1;
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
        if (cancel_info.raised == MatteIsActive(cancel_info,event.xmotion))
          {
            /*
              Cancel button status changed.
            */
            cancel_info.raised=cancel_info.raised == MagickFalse ?
              MagickTrue : MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
            break;
          }
        if (dismiss_info.raised == MatteIsActive(dismiss_info,event.xmotion))
          {
            /*
              Dismiss button status changed.
            */
            dismiss_info.raised=dismiss_info.raised == MagickFalse ?
              MagickTrue : MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&dismiss_info);
            break;
          }
        if (yes_info.raised == MatteIsActive(yes_info,event.xmotion))
          {
            /*
              Yes button status changed.
            */
            yes_info.raised=yes_info.raised == MagickFalse ?
              MagickTrue : MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&yes_info);
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
  return(confirm);
}