void gd_stderr_error(int priority, const char *format, va_list args)
{
	switch (priority) {
	case GD_ERROR:
		fputs("GD Error: ", stderr);
		break;
	case GD_WARNING:
		fputs("GD Warning: ", stderr);
		break;
	case GD_NOTICE:
		fputs("GD Notice: ", stderr);
		break;
	case GD_INFO:
		fputs("GD Info: ", stderr);
		break;
	case GD_DEBUG:
		fputs("GD Debug: ", stderr);
		break;
	}
	vfprintf(stderr, format, args);
	fflush(stderr);
}