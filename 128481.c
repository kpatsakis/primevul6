SupportsClient2Server(rfbClient* client, int messageType)
{
    return (client->supportedMessages.client2server[((messageType & 0xFF)/8)] & (1<<(messageType % 8)) ? TRUE : FALSE);
}