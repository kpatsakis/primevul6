SWFRect SWFShape_getEdgeBounds(SWFShape shape)
{
	if(shape->useVersion == SWF_SHAPE4)
		return shape->edgeBounds;
	else
		return NULL;
}