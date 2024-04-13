SendPointerEvent(rfbClient* client,int x, int y, int buttonMask)
{
  rfbPointerEventMsg pe;

  if (!SupportsClient2Server(client, rfbPointerEvent)) return TRUE;

  pe.type = rfbPointerEvent;
  pe.buttonMask = buttonMask;
  if (x < 0) x = 0;
  if (y < 0) y = 0;

  pe.x = rfbClientSwap16IfLE(x);
  pe.y = rfbClientSwap16IfLE(y);
  return WriteToRFBServer(client, (char *)&pe, sz_rfbPointerEventMsg);
}