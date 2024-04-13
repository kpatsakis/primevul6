formatStrlen(char *target, char *source, int freeit)
{
    if (source != 0) {
	sprintf(target, "%u", (unsigned) strlen(source));
	if (freeit) {
	    free(source);
	}
    } else {
	strcpy(target, "0");
    }
    return target;
}