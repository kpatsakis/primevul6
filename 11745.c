push(void)
/* push onstack on to the stack */
{
    if (stackptr >= MAX_PUSHED)
	_nc_warning("string too complex to convert");
    else
	stack[stackptr++] = onstack;
}