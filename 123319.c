isc_httpd_setfinishhook(void (*fn)(void)) {
#if ENABLE_AFL
	finishhook = fn;
#else  /* ENABLE_AFL */
	UNUSED(fn);
#endif /* ENABLE_AFL */
}