show_fkey_name(NAME_VALUE * data)
{
    if (data->keycode > 0) {
	fprintf(stderr, " %s", keyname(data->keycode));
	fprintf(stderr, " (capability \"%s\")", data->name);
    } else {
	fprintf(stderr, " capability \"%s\"", data->name);
    }
}