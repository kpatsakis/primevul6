ReadReason(rfbClient* client)
{
    uint32_t reasonLen;
    char *reason;

    if (!ReadFromRFBServer(client, (char *)&reasonLen, 4)) return;
    reasonLen = rfbClientSwap32IfLE(reasonLen);
    if(reasonLen > 1<<20) {
      rfbClientLog("VNC connection failed, but sent reason length of %u exceeds limit of 1MB",(unsigned int)reasonLen);
      return;
    }
    reason = malloc(reasonLen+1);
    if (!ReadFromRFBServer(client, reason, reasonLen)) { free(reason); return; }
    reason[reasonLen]=0;
    rfbClientLog("VNC connection failed: %s\n",reason);
    free(reason);
}