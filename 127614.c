void SWFShape_useVersion(SWFShape shape, int version)
{
	if(shape->useVersion >= version)
		return;
	if(version > SWF_SHAPE4)
		return;
	shape->useVersion = version;
}