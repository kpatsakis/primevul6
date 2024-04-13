static int checkAttribute(struct MYSOFA_ATTRIBUTE *attribute, char *name,
		char *value) {
	while (attribute) {
		if (!mystrcmp(attribute->name, name)
				&& !mystrcmp(attribute->value, value))
			return MYSOFA_OK;
		attribute = attribute->next;
	}

	return MYSOFA_INVALID_FORMAT;
}