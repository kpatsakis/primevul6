SWFShape_getFills(SWFShape shape, SWFFillStyle** fills, int* nFills)
{
	*fills = shape->fills;
	*nFills = shape->nFills;
}