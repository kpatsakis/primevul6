static ZIPARCHIVE_METHOD(unchangeArchive)
{
	struct zip *intern;
	zval *this = getThis();

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zip_unchange_archive(intern) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}