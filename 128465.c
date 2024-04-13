void rfbClientRegisterExtension(rfbClientProtocolExtension* e)
{
	e->next = rfbClientExtensions;
	rfbClientExtensions = e;
}