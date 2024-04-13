SendFramebufferUpdateRequest(rfbClient* client, int x, int y, int w, int h, rfbBool incremental)
{
  rfbFramebufferUpdateRequestMsg fur;

  if (!SupportsClient2Server(client, rfbFramebufferUpdateRequest)) return TRUE;
  
  fur.type = rfbFramebufferUpdateRequest;
  fur.incremental = incremental ? 1 : 0;
  fur.x = rfbClientSwap16IfLE(x);
  fur.y = rfbClientSwap16IfLE(y);
  fur.w = rfbClientSwap16IfLE(w);
  fur.h = rfbClientSwap16IfLE(h);

  if (!WriteToRFBServer(client, (char *)&fur, sz_rfbFramebufferUpdateRequestMsg))
    return FALSE;

  return TRUE;
}