 */
static ssize_t bql_show(char *buf, unsigned int value)
{
	return sprintf(buf, "%u\n", value);