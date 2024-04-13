exif_entry_log (ExifEntry *e, ExifLogCode code, const char *format, ...)
{
	va_list args;
	ExifLog *l = NULL;

	if (e && e->parent && e->parent->parent)
		l = exif_data_get_log (e->parent->parent);
	va_start (args, format);
	exif_logv (l, code, "ExifEntry", format, args);
	va_end (args);
}