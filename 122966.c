void rad_free(RADIUS_PACKET **radius_packet_ptr)
{
	RADIUS_PACKET *radius_packet;

	if (!radius_packet_ptr) return;
	radius_packet = *radius_packet_ptr;

	if (radius_packet->data) free(radius_packet->data);
	if (radius_packet->vps) pairfree(&radius_packet->vps);

	free(radius_packet);

	*radius_packet_ptr = NULL;
}