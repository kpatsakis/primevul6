DefaultSupportedMessagesTightVNC(rfbClient* client)
{
    DefaultSupportedMessages(client);
    SetClient2Server(client, rfbFileTransfer);
    SetClient2Server(client, rfbSetServerInput);
    SetClient2Server(client, rfbSetSW);
    /* SetClient2Server(client, rfbTextChat); */
    /* technically, we only care what we can *send* to the server */
    SetServer2Client(client, rfbFileTransfer);
    SetServer2Client(client, rfbTextChat);
}