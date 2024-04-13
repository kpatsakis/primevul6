DefaultSupportedMessagesUltraVNC(rfbClient* client)
{
    DefaultSupportedMessages(client);
    SetClient2Server(client, rfbFileTransfer);
    SetClient2Server(client, rfbSetScale);
    SetClient2Server(client, rfbSetServerInput);
    SetClient2Server(client, rfbSetSW);
    SetClient2Server(client, rfbTextChat);
    SetClient2Server(client, rfbPalmVNCSetScaleFactor);
    /* technically, we only care what we can *send* to the server */
    SetServer2Client(client, rfbResizeFrameBuffer);
    SetServer2Client(client, rfbPalmVNCReSizeFrameBuffer);
    SetServer2Client(client, rfbFileTransfer);
    SetServer2Client(client, rfbTextChat);
}