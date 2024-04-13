static u32 rt6_peer_genid(void)
{
	return atomic_read(&__rt6_peer_genid);
}