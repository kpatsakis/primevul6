getEventTime(XEvent *event)
{
    Time result;

    if (IsBtnEvent(event)) {
	result = ((XButtonEvent *) event)->time;
    } else if (IsKeyEvent(event)) {
	result = ((XKeyEvent *) event)->time;
    } else {
	result = 0;
    }

    return result;
}