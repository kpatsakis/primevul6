rfbBool TextChatOpen(rfbClient* client)
{
    rfbTextChatMsg chat;

    if (!SupportsClient2Server(client, rfbTextChat)) return TRUE;
    chat.type = rfbTextChat;
    chat.pad1 = 0;
    chat.pad2 = 0;
    chat.length = rfbClientSwap32IfLE(rfbTextChatOpen);
    return  (WriteToRFBServer(client, (char *)&chat, sz_rfbTextChatMsg) ? TRUE : FALSE);
}