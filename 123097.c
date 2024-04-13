int radius_event_process(void)
{
	if (!el) return 0;

	return fr_event_loop(el);
}