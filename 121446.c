KeyState(XtermWidget xw, unsigned x)
{
    return ((((x) & (ShiftMask | ControlMask)))
	    + (((x) & MetaMask(xw)) ? 2 : 0));
}