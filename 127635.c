completeSWFShapeBlock(SWFBlock block)
{
	SWFShape shape = (SWFShape)block;

	SWFShape_end(shape);
	
	return SWFOutput_getLength(shape->out);
}