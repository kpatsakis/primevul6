MagickExport int XDialogWidget(Display *display,XWindows *windows,
  const char *action,const char *query,char *reply)
{
#define CancelButtonText  "Cancel"

  char
    primary_selection[MaxTextExtent];

  int
    x;

  int
    i;

  static MagickBooleanType
    raised = MagickFalse;

  Status
    status;

  unsigned int
    anomaly,
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
    action_info,
    cancel_info,
    reply_info,
    special_info,
    text_info;

  XWindowChanges
    window_changes;

  /*
    Determine Dialog widget attributes.
  */
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  assert(action != (char *) NULL);
  assert(query != (char *) NULL);
  assert(reply != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",action);
  XCheckRefreshWindows(display,windows);
  font_info=windows->widget.font_info;
  width=WidgetTextWidth(font_info,(char *) action);
  if (WidgetTextWidth(font_info,CancelButtonText) > width)
    width=WidgetTextWidth(font_info,CancelButtonText);
  width+=(3*QuantumMargin) >> 1;
  height=(unsigned int) (font_info->ascent+font_info->descent);
  /*
    Position Dialog widget.
  */
  windows->widget.width=(unsigned int) MagickMax((int) (2*width),(int)
    WidgetTextWidth(font_info,(char *) query));
  if (windows->widget.width < WidgetTextWidth(font_info,reply))
    windows->widget.width=WidgetTextWidth(font_info,reply);
  windows->widget.width+=6*QuantumMargin;
  windows->widget.min_width=(unsigned int)
    (width+28*XTextWidth(font_info,"#",1)+4*QuantumMargin);
  if (windows->widget.width < windows->widget.min_width)
    windows->widget.width=windows->widget.min_width;
  windows->widget.height=(unsigned int) (7*height+(QuantumMargin << 1));
  windows->widget.min_height=windows->widget.height;
  if (windows->widget.height < windows->widget.min_height)
    windows->widget.height=windows->widget.min_height;
  XConstrainWindowPosition(display,&windows->widget);
  /*
    Map Dialog widget.
  */
  (void) CopyMagickString(windows->widget.name,"Dialog",MaxTextExtent);
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
  anomaly=(LocaleCompare(action,"Background") == 0) ||
    (LocaleCompare(action,"New") == 0) ||
    (LocaleCompare(action,"Quantize") == 0) ||
    (LocaleCompare(action,"Resize") == 0) ||
    (LocaleCompare(action,"Save") == 0) ||
    (LocaleCompare(action,"Shade") == 0);
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
        cancel_info.height=(unsigned int) ((3*height) >> 1);
        cancel_info.x=(int)
          (windows->widget.width-cancel_info.width-((3*QuantumMargin) >> 1));
        cancel_info.y=(int)
          (windows->widget.height-cancel_info.height-((3*QuantumMargin) >> 1));
        XGetWidgetInfo(action,&action_info);
        action_info.width=width;
        action_info.height=(unsigned int) ((3*height) >> 1);
        action_info.x=cancel_info.x-(cancel_info.width+QuantumMargin+
          (action_info.bevel_width << 1));
        action_info.y=cancel_info.y;
        /*
          Initialize reply information.
        */
        XGetWidgetInfo(reply,&reply_info);
        reply_info.raised=MagickFalse;
        reply_info.bevel_width--;
        reply_info.width=windows->widget.width-(3*QuantumMargin);
        reply_info.height=height << 1;
        reply_info.x=(3*QuantumMargin) >> 1;
        reply_info.y=action_info.y-reply_info.height-QuantumMargin;
        /*
          Initialize option information.
        */
        XGetWidgetInfo("Dither",&special_info);
        special_info.raised=raised;
        special_info.bevel_width--;
        special_info.width=(unsigned int) QuantumMargin >> 1;
        special_info.height=(unsigned int) QuantumMargin >> 1;
        special_info.x=reply_info.x;
        special_info.y=action_info.y+action_info.height-special_info.height;
        if (LocaleCompare(action,"Background") == 0)
          special_info.text=(char *) "Backdrop";
        if (LocaleCompare(action,"New") == 0)
          special_info.text=(char *) "Gradation";
        if (LocaleCompare(action,"Resize") == 0)
          special_info.text=(char *) "Constrain ratio";
        if (LocaleCompare(action,"Save") == 0)
          special_info.text=(char *) "Non-progressive";
        if (LocaleCompare(action,"Shade") == 0)
          special_info.text=(char *) "Color shading";
        /*
          Initialize text information.
        */
        XGetWidgetInfo(query,&text_info);
        text_info.width=reply_info.width;
        text_info.height=height;
        text_info.x=reply_info.x-(QuantumMargin >> 1);
        text_info.y=QuantumMargin;
        text_info.center=MagickFalse;
        state&=(~UpdateConfigurationState);
      }
    if (state & RedrawWidgetState)
      {
        /*
          Redraw Dialog widget.
        */
        XDrawWidgetText(display,&windows->widget,&text_info);
        XDrawBeveledMatte(display,&windows->widget,&reply_info);
        XDrawMatteText(display,&windows->widget,&reply_info);
        if (anomaly)
          XDrawBeveledButton(display,&windows->widget,&special_info);
        XDrawBeveledButton(display,&windows->widget,&action_info);
        XDrawBeveledButton(display,&windows->widget,&cancel_info);
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
        if (anomaly)
          if (MatteIsActive(special_info,event.xbutton))
            {
              /*
                Option button status changed.
              */
              special_info.raised=!special_info.raised;
              XDrawBeveledButton(display,&windows->widget,&special_info);
              break;
            }
        if (MatteIsActive(action_info,event.xbutton))
          {
            /*
              User pressed Action button.
            */
            action_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&action_info);
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
        if (MatteIsActive(reply_info,event.xbutton) == MagickFalse)
          break;
        if (event.xbutton.button != Button2)
          {
            static Time
              click_time;

            /*
              Move text cursor to position of button press.
            */
            x=event.xbutton.x-reply_info.x-(QuantumMargin >> 2);
            for (i=1; i <= Extent(reply_info.marker); i++)
              if (XTextWidth(font_info,reply_info.marker,i) > x)
                break;
            reply_info.cursor=reply_info.marker+i-1;
            if (event.xbutton.time > (click_time+DoubleClick))
              reply_info.highlight=MagickFalse;
            else
              {
                /*
                  Become the XA_PRIMARY selection owner.
                */
                (void) CopyMagickString(primary_selection,reply_info.text,
                  MaxTextExtent);
                (void) XSetSelectionOwner(display,XA_PRIMARY,windows->widget.id,
                  event.xbutton.time);
                reply_info.highlight=XGetSelectionOwner(display,XA_PRIMARY) ==
                  windows->widget.id ? MagickTrue : MagickFalse;
              }
            XDrawMatteText(display,&windows->widget,&reply_info);
            click_time=event.xbutton.time;
            break;
          }
        /*
          Request primary selection.
        */
        (void) XConvertSelection(display,XA_PRIMARY,XA_STRING,XA_STRING,
          windows->widget.id,event.xbutton.time);
        break;
      }
      case ButtonRelease:
      {
        if (windows->widget.mapped == MagickFalse)
          break;
        if (action_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(action_info,event.xbutton))
                state|=ExitState;
            action_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&action_info);
          }
        if (cancel_info.raised == MagickFalse)
          {
            if (event.xbutton.window == windows->widget.id)
              if (MatteIsActive(cancel_info,event.xbutton))
                {
                  *reply_info.text='\0';
                  state|=ExitState;
                }
            cancel_info.raised=MagickTrue;
            XDrawBeveledButton(display,&windows->widget,&cancel_info);
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
            *reply_info.text='\0';
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

        static int
          length;

        static KeySym
          key_symbol;

        /*
          Respond to a user key press.
        */
        if (event.xkey.window != windows->widget.id)
          break;
        length=XLookupString((XKeyEvent *) &event.xkey,command,
          (int) sizeof(command),&key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if ((key_symbol == XK_Return) || (key_symbol == XK_KP_Enter))
          {
            action_info.raised=MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&action_info);
            state|=ExitState;
            break;
          }
        if (key_symbol == XK_Control_L)
          {
            state|=ControlState;
            break;
          }
        if (state & ControlState)
          switch ((int) key_symbol)
          {
            case XK_u:
            case XK_U:
            {
              /*
                Erase the entire line of text.
              */
              *reply_info.text='\0';
              reply_info.cursor=reply_info.text;
              reply_info.marker=reply_info.text;
              reply_info.highlight=MagickFalse;
              break;
            }
            default:
              break;
          }
        XEditText(display,&reply_info,key_symbol,command,state);
        XDrawMatteText(display,&windows->widget,&reply_info);
        break;
      }
      case KeyRelease:
      {
        static char
          command[MaxTextExtent];

        static KeySym
          key_symbol;

        /*
          Respond to a user key release.
        */
        if (event.xkey.window != windows->widget.id)
          break;
        (void) XLookupString((XKeyEvent *) &event.xkey,command,
          (int) sizeof(command),&key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Control_L)
          state&=(~ControlState);
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
        if (action_info.raised == MatteIsActive(action_info,event.xmotion))
          {
            /*
              Action button status changed.
            */
            action_info.raised=action_info.raised == MagickFalse ?
              MagickTrue : MagickFalse;
            XDrawBeveledButton(display,&windows->widget,&action_info);
            break;
          }
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
        break;
      }
      case SelectionClear:
      {
        reply_info.highlight=MagickFalse;
        XDrawMatteText(display,&windows->widget,&reply_info);
        break;
      }
      case SelectionNotify:
      {
        Atom
          type;

        int
          format;

        unsigned char
          *data;

        unsigned long
          after,
          length;

        /*
          Obtain response from primary selection.
        */
        if (event.xselection.property == (Atom) None)
          break;
        status=XGetWindowProperty(display,event.xselection.requestor,
          event.xselection.property,0L,2047L,MagickTrue,XA_STRING,&type,
          &format,&length,&after,&data);
        if ((status != Success) || (type != XA_STRING) || (format == 32) ||
            (length == 0))
          break;
        if ((Extent(reply_info.text)+length) >= (MaxTextExtent-1))
          (void) XBell(display,0);
        else
          {
            /*
              Insert primary selection in reply text.
            */
            *(data+length)='\0';
            XEditText(display,&reply_info,(KeySym) XK_Insert,(char *) data,
              state);
            XDrawMatteText(display,&windows->widget,&reply_info);
          }
        (void) XFree((void *) data);
        break;
      }
      case SelectionRequest:
      {
        XSelectionEvent
          notify;

        XSelectionRequestEvent
          *request;

        if (reply_info.highlight == MagickFalse)
          break;
        /*
          Set primary selection.
        */
        request=(&(event.xselectionrequest));
        (void) XChangeProperty(request->display,request->requestor,
          request->property,request->target,8,PropModeReplace,
          (unsigned char *) primary_selection,Extent(primary_selection));
        notify.type=SelectionNotify;
        notify.display=request->display;
        notify.requestor=request->requestor;
        notify.selection=request->selection;
        notify.target=request->target;
        notify.time=request->time;
        if (request->property == None)
          notify.property=request->target;
        else
          notify.property=request->property;
        (void) XSendEvent(request->display,request->requestor,False,0,
          (XEvent *) &notify);
      }
      default:
        break;
    }
  } while ((state & ExitState) == 0);
  XSetCursorState(display,windows,MagickFalse);
  (void) XWithdrawWindow(display,windows->widget.id,windows->widget.screen);
  XCheckRefreshWindows(display,windows);
  if (anomaly)
    if (special_info.raised)
      if (*reply != '\0')
        raised=MagickTrue;
  return(raised == MagickFalse);
}