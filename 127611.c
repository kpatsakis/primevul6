SWFShape_addSolidFillStyle(SWFShape shape, byte r, byte g, byte b, byte a)
{
	int  ret;

	SWFFillStyle fill = newSWFSolidFillStyle(r, g, b, a);
	
	ret = addFillStyle(shape, fill);
	if(ret < 0) /* error */
	{
		destroySWFFillStyle(fill);
		return NULL;
	}
	else if(ret == shape->nFills)  /* new fill */
	{
		return fill;
	}
	else /* fill is known */ 
	{
		destroySWFFillStyle(fill);
		return shape->fills[ret];
	}
}