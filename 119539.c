static int getDimension(unsigned *dim, struct DATAOBJECT *dataobject) {
	int err;
	struct MYSOFA_ATTRIBUTE *attr = dataobject->attributes;

	if (!!(err = checkAttribute(dataobject->attributes, "CLASS",
			"DIMENSION_SCALE")))
		return err;

	while (attr) {
		log(" %s=%s\n",attr->name,attr->value);

		if (!strcmp(attr->name, "NAME")
				&& attr->value && !strncmp(attr->value,
						"This is a netCDF dimension but not a netCDF variable.",
						53)) {
			char *p = attr->value + strlen(attr->value) - 1;
			while (isdigit(*p)) {
				p--;
			}
			p++;
			*dim = atoi(p);
			log("NETCDF DIM %u\n",*dim);
			return MYSOFA_OK;
		}
		attr = attr->next;
	}
	return MYSOFA_INVALID_FORMAT;
}