checked_write_file (HANDLE f, gconstpointer buffer, guint32 numbytes)
{
	guint32 dummy;
	if (!WriteFile (f, buffer, numbytes, &dummy, NULL))
		g_error ("WriteFile returned %d\n", GetLastError ());
}