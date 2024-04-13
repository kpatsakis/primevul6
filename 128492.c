SendIncrementalFramebufferUpdateRequest(rfbClient* client)
{
	return SendFramebufferUpdateRequest(client,
			client->updateRect.x, client->updateRect.y,
			client->updateRect.w, client->updateRect.h, TRUE);
}