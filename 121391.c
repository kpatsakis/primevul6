InterpretButton(XtermWidget xw, XButtonEvent *event)
{
    Bool result = False;

    if (ShiftOverride(xw, event->state, (int) event->button)) {
	TRACE(("...shift-button #%d overrides mouse-protocol\n", event->button));
	result = True;
    }
    return result;
}