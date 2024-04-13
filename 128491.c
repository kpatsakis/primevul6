rfbBool TextChatSend(rfbClient* client, char *text)
{
    rfbTextChatMsg chat;
    int count = strlen(text);

    if (!SupportsClient2Server(client, rfbTextChat)) return TRUE;
    chat.type = rfbTextChat;
    chat.pad1 = 0;
    chat.pad2 = 0;
    chat.length = (uint32_t)count;
    chat.length = rfbClientSwap32IfLE(chat.length);

    if (!WriteToRFBServer(client, (char *)&chat, sz_rfbTextChatMsg))
        return FALSE;

    if (count>0) {
        if (!WriteToRFBServer(client, text, count))
            return FALSE;
    }
    return TRUE;
}