OurModifiers(XtermWidget xw)
{
    return (ShiftMask
	    | ControlMask
	    | MetaMask(xw));
}