void key_schedule_gc_links(void)
{
	set_bit(KEY_GC_KEY_EXPIRED, &key_gc_flags);
	schedule_work(&key_gc_work);
}