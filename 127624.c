static void oprintf(struct out *op, const char *fmt, ...) 	 
{
	va_list ap; 	 
	char buf[256]; 	 
	int d, l; 	 
	  	 
	va_start(ap, fmt); 	 
	l = vsprintf(buf, fmt, ap); 	 
	while((d = op->ptr - op->buf) + l >= op->len-1) 	 
	{
		op->buf = (char *) realloc(op->buf, op->len += 100); 	 
		op->ptr = op->buf + d; 	 
	} 	 
	for(d = 0 ; d < l ; d++) 	 
		*op->ptr++ = buf[d]; 	 
}