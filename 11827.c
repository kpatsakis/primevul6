name_of_type(int type)
{
    const char *result = "unknown";
    switch (type) {
    case BOOLEAN:
	result = "boolean";
	break;
    case NUMBER:
	result = "number";
	break;
    case STRING:
	result = "string";
	break;
    }
    return result;
}