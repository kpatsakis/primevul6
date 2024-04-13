static int getArray(struct MYSOFA_ARRAY *array, struct DATAOBJECT *dataobject) {
	float *p1;
	double *p2;
	int i;

	struct MYSOFA_ATTRIBUTE *attr = dataobject->attributes;
	while (attr) {
		log(" %s=%s\n",attr->name,attr->value);

		attr = attr->next;
	}

	if (dataobject->dt.u.f.bit_precision != 64)
		return MYSOFA_UNSUPPORTED_FORMAT;

	array->attributes = dataobject->attributes;
	dataobject->attributes = NULL;
	array->elements = dataobject->data_len / 8;

	p1 = dataobject->data;
	p2 = dataobject->data;
	for (i = 0; i < array->elements; i++)
		*p1++ = *p2++;
	array->values = realloc(dataobject->data, array->elements * sizeof(float));

	dataobject->data = NULL;

	return MYSOFA_OK;
}