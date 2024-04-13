SWFShape_addGradientFillStyle(SWFShape shape, SWFGradient gradient, byte flags)
{
	SWFFillStyle fill = newSWFGradientFillStyle(gradient, flags);
	if(addFillStyle(shape, fill) < 0)
	{
		destroySWFFillStyle(fill);
		return NULL;
	}
	return fill;		
}