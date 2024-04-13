static void arrayFree(struct MYSOFA_ARRAY *array) {
	while (array->attributes) {
		struct MYSOFA_ATTRIBUTE *next = array->attributes->next;
		free(array->attributes->name);
		free(array->attributes->value);
		free(array->attributes);
		array->attributes = next;
	}
	free(array->values);
}