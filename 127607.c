void SWFShape_setRenderHintingFlags(SWFShape shape, int flags)
{
	flags &= (SWF_SHAPE_USESCALINGSTROKES | SWF_SHAPE_USENONSCALINGSTROKES);
	shape->flags = flags;
	SWFShape_useVersion(shape, SWF_SHAPE4);
}