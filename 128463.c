void* rfbClientGetClientData(rfbClient* client, void* tag)
{
	rfbClientData* clientData = client->clientData;

	while(clientData) {
		if(clientData->tag == tag)
			return clientData->data;
		clientData = clientData->next;
	}

	return NULL;
}