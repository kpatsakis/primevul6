pq_buffer_has_data(void)
{
	return (PqRecvPointer < PqRecvLength);
}