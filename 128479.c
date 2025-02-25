SendClientCutText(rfbClient* client, char *str, int len)
{
  rfbClientCutTextMsg cct;

  if (!SupportsClient2Server(client, rfbClientCutText)) return TRUE;

  memset(&cct, 0, sizeof(cct));
  cct.type = rfbClientCutText;
  cct.length = rfbClientSwap32IfLE(len);
  return  (WriteToRFBServer(client, (char *)&cct, sz_rfbClientCutTextMsg) &&
	   WriteToRFBServer(client, str, len));
}