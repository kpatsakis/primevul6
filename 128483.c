rfbBool SendXvpMsg(rfbClient* client, uint8_t version, uint8_t code)
{
    rfbXvpMsg xvp;

    if (!SupportsClient2Server(client, rfbXvp)) return TRUE;
    xvp.type = rfbXvp;
    xvp.pad = 0;
    xvp.version = version;
    xvp.code = code;

    if (!WriteToRFBServer(client, (char *)&xvp, sz_rfbXvpMsg))
        return FALSE;

    return TRUE;
}