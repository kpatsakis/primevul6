DefaultSupportedMessages(rfbClient* client)
{
    memset((char *)&client->supportedMessages,0,sizeof(client->supportedMessages));

    /* Default client supported messages (universal RFB 3.3 protocol) */
    SetClient2Server(client, rfbSetPixelFormat);
    /* SetClient2Server(client, rfbFixColourMapEntries); Not currently supported */
    SetClient2Server(client, rfbSetEncodings);
    SetClient2Server(client, rfbFramebufferUpdateRequest);
    SetClient2Server(client, rfbKeyEvent);
    SetClient2Server(client, rfbPointerEvent);
    SetClient2Server(client, rfbClientCutText);
    /* technically, we only care what we can *send* to the server
     * but, we set Server2Client Just in case it ever becomes useful
     */
    SetServer2Client(client, rfbFramebufferUpdate);
    SetServer2Client(client, rfbSetColourMapEntries);
    SetServer2Client(client, rfbBell);
    SetServer2Client(client, rfbServerCutText);
}