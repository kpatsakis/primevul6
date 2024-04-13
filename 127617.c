int SWFShape_getFlags(SWFShape shape)
{
	if(shape->useVersion == SWF_SHAPE4)
		return shape->flags;
	else
		return 0;
}