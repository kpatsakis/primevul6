static ZIPARCHIVE_METHOD(unchangeAll)
{
	struct zip *intern;
	zval *this = getThis();

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zip_unchange_all(intern) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}