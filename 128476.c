void rfbClientSetClientData(rfbClient* client, void* tag, void* data)
{
	rfbClientData* clientData = client->clientData;

	while(clientData && clientData->tag != tag)
		clientData = clientData->next;
	if(clientData == NULL) {
		clientData = calloc(sizeof(rfbClientData), 1);
		clientData->next = client->clientData;
		client->clientData = clientData;
		clientData->tag = tag;
	}

	clientData->data = data;
}