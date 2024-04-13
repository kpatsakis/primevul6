SupportsServer2Client(rfbClient* client, int messageType)
{
    return (client->supportedMessages.server2client[((messageType & 0xFF)/8)] & (1<<(messageType % 8)) ? TRUE : FALSE);
}