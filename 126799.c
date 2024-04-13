static void key_gc_timer_func(unsigned long data)
{
	kenter("");
	key_gc_next_run = LONG_MAX;
	key_schedule_gc_links();
}