SendFocusButton(XtermWidget xw, XFocusChangeEvent *event)
{
    if (okSendFocusPos(xw)) {
	ANSI reply;

	memset(&reply, 0, sizeof(reply));
	reply.a_type = ANSI_CSI;

#if OPT_SCO_FUNC_KEYS
	if (xw->keyboard.type == keyboardIsSCO) {
	    reply.a_pintro = '>';
	}
#endif
	reply.a_final = CharOf((event->type == FocusIn) ? 'I' : 'O');
	unparseseq(xw, &reply);
    }
    return;
}