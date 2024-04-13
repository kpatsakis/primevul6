HandlePlainAuth(rfbClient *client)
{
  uint32_t ulen, ulensw;
  uint32_t plen, plensw;
  rfbCredential *cred;

  if (!client->GetCredential)
  {
    rfbClientLog("GetCredential callback is not set.\n");
    return FALSE;
  }
  cred = client->GetCredential(client, rfbCredentialTypeUser);
  if (!cred)
  {
    rfbClientLog("Reading credential failed\n");
    return FALSE;
  }

  ulen = (cred->userCredential.username ? strlen(cred->userCredential.username) : 0);
  ulensw = rfbClientSwap32IfLE(ulen);
  plen = (cred->userCredential.password ? strlen(cred->userCredential.password) : 0);
  plensw = rfbClientSwap32IfLE(plen);
  if (!WriteToRFBServer(client, (char *)&ulensw, 4) ||
      !WriteToRFBServer(client, (char *)&plensw, 4))
  {
    FreeUserCredential(cred);
    return FALSE;
  }
  if (ulen > 0)
  {
    if (!WriteToRFBServer(client, cred->userCredential.username, ulen))
    {
      FreeUserCredential(cred);
      return FALSE;
    }
  }
  if (plen > 0)
  {
    if (!WriteToRFBServer(client, cred->userCredential.password, plen))
    {
      FreeUserCredential(cred);
      return FALSE;
    }
  }

  FreeUserCredential(cred);

  /* Handle the SecurityResult message */
  if (!rfbHandleAuthResult(client)) return FALSE;

  return TRUE;
}