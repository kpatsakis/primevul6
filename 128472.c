SendScaleSetting(rfbClient* client,int scaleSetting)
{
  rfbSetScaleMsg ssm;

  ssm.scale = scaleSetting;
  ssm.pad = 0;
  
  /* favor UltraVNC SetScale if both are supported */
  if (SupportsClient2Server(client, rfbSetScale)) {
      ssm.type = rfbSetScale;
      if (!WriteToRFBServer(client, (char *)&ssm, sz_rfbSetScaleMsg))
          return FALSE;
  }
  
  if (SupportsClient2Server(client, rfbPalmVNCSetScaleFactor)) {
      ssm.type = rfbPalmVNCSetScaleFactor;
      if (!WriteToRFBServer(client, (char *)&ssm, sz_rfbSetScaleMsg))
          return FALSE;
  }

  return TRUE;
}