writeSWFShapeBlockToMethod(SWFBlock block, 
                           SWFByteOutputMethod method, void* data)
{
	SWFOutput out = ((SWFShape)block)->out;
	SWFOutput_writeToMethod(out, method, data);
}