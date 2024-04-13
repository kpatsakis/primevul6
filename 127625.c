SWFShape_addBitmapFillStyle(SWFShape shape, SWFBitmap bitmap, byte flags)
{
	SWFFillStyle fill;

	if ( bitmap )
	{
		SWFCharacter_addDependency((SWFCharacter)shape,
		                           (SWFCharacter)bitmap);
	}

	fill = newSWFBitmapFillStyle(bitmap, flags);
	if(addFillStyle(shape, fill) < 0)
	{
		destroySWFFillStyle(fill);
		return NULL;
	}
	return fill;
}