exif_entry_initialize (ExifEntry *e, ExifTag tag)
{
	ExifRational r;
	ExifByteOrder o;

	/* We need the byte order */
	if (!e || !e->parent || e->data || !e->parent->parent)
		return;
	o = exif_data_get_byte_order (e->parent->parent);

	e->tag = tag;
	switch (tag) {

	/* LONG, 1 component, no default */
	case EXIF_TAG_PIXEL_X_DIMENSION:
	case EXIF_TAG_PIXEL_Y_DIMENSION:
	case EXIF_TAG_EXIF_IFD_POINTER:
	case EXIF_TAG_GPS_INFO_IFD_POINTER:
	case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
	case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
	case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
		e->components = 1;
		e->format = EXIF_FORMAT_LONG;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		break;

	/* SHORT, 1 component, no default */
	case EXIF_TAG_SUBJECT_LOCATION:
	case EXIF_TAG_SENSING_METHOD:
	case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
	case EXIF_TAG_COMPRESSION:
	case EXIF_TAG_EXPOSURE_MODE:
	case EXIF_TAG_WHITE_BALANCE:
	case EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM:
	case EXIF_TAG_GAIN_CONTROL:
	case EXIF_TAG_SUBJECT_DISTANCE_RANGE:
	case EXIF_TAG_FLASH:
	case EXIF_TAG_ISO_SPEED_RATINGS:

	/* SHORT, 1 component, default 0 */
	case EXIF_TAG_IMAGE_WIDTH:
	case EXIF_TAG_IMAGE_LENGTH:
	case EXIF_TAG_EXPOSURE_PROGRAM:
	case EXIF_TAG_LIGHT_SOURCE:
	case EXIF_TAG_METERING_MODE:
	case EXIF_TAG_CUSTOM_RENDERED:
	case EXIF_TAG_SCENE_CAPTURE_TYPE:
	case EXIF_TAG_CONTRAST:
	case EXIF_TAG_SATURATION:
	case EXIF_TAG_SHARPNESS:
		e->components = 1;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 0);
		break;

	/* SHORT, 1 component, default 1 */
	case EXIF_TAG_ORIENTATION:
	case EXIF_TAG_PLANAR_CONFIGURATION:
	case EXIF_TAG_YCBCR_POSITIONING:
		e->components = 1;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 1);
		break;

	/* SHORT, 1 component, default 2 */
	case EXIF_TAG_RESOLUTION_UNIT:
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
		e->components = 1;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 2);
		break;

	/* SHORT, 1 component, default 3 */
	case EXIF_TAG_SAMPLES_PER_PIXEL:
		e->components = 1;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 3);
		break;

	/* SHORT, 1 component, default 0xffff */
	case EXIF_TAG_COLOR_SPACE:
		e->components = 1;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 0xffff);
		break;

	/* SHORT, 3 components, default 8 8 8 */
	case EXIF_TAG_BITS_PER_SAMPLE:
		e->components = 3;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 8);
		exif_set_short (
			e->data + exif_format_get_size (e->format),
			o, 8);
		exif_set_short (
			e->data + 2 * exif_format_get_size (e->format),
			o, 8);
		break;

	/* SHORT, 2 components, default 2 1 */
	case EXIF_TAG_YCBCR_SUB_SAMPLING:
		e->components = 2;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		exif_set_short (e->data, o, 2);
		exif_set_short (
			e->data + exif_format_get_size (e->format),
			o, 1);
		break;

	/* SRATIONAL, 1 component, no default */
	case EXIF_TAG_EXPOSURE_BIAS_VALUE:
	case EXIF_TAG_BRIGHTNESS_VALUE:
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		e->components = 1;
		e->format = EXIF_FORMAT_SRATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		break;

	/* RATIONAL, 1 component, no default */
	case EXIF_TAG_EXPOSURE_TIME:
	case EXIF_TAG_FOCAL_PLANE_X_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION:
	case EXIF_TAG_EXPOSURE_INDEX:
	case EXIF_TAG_FLASH_ENERGY:
	case EXIF_TAG_FNUMBER:
	case EXIF_TAG_FOCAL_LENGTH:
	case EXIF_TAG_SUBJECT_DISTANCE:
	case EXIF_TAG_MAX_APERTURE_VALUE:
	case EXIF_TAG_APERTURE_VALUE:
	case EXIF_TAG_COMPRESSED_BITS_PER_PIXEL:
	case EXIF_TAG_PRIMARY_CHROMATICITIES:
	case EXIF_TAG_DIGITAL_ZOOM_RATIO:
		e->components = 1;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		break;

	/* RATIONAL, 1 component, default 72/1 */
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_Y_RESOLUTION:
		e->components = 1;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		r.numerator = 72;
		r.denominator = 1;
		exif_set_rational (e->data, o, r);
		break;

	/* RATIONAL, 2 components, no default */
	case EXIF_TAG_WHITE_POINT:
		e->components = 2;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		break;

	/* RATIONAL, 6 components */
	case EXIF_TAG_REFERENCE_BLACK_WHITE:
		e->components = 6;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		r.denominator = 1;
		r.numerator = 0;
		exif_set_rational (e->data, o, r);
		r.numerator = 255;
		exif_set_rational (
			e->data + exif_format_get_size (e->format), o, r);
		r.numerator = 0;
		exif_set_rational (
			e->data + 2 * exif_format_get_size (e->format), o, r);
		r.numerator = 255;
		exif_set_rational (
			e->data + 3 * exif_format_get_size (e->format), o, r);
		r.numerator = 0;
		exif_set_rational (
			e->data + 4 * exif_format_get_size (e->format), o, r);
		r.numerator = 255;
		exif_set_rational (
			e->data + 5 * exif_format_get_size (e->format), o, r);
		break;

	/* ASCII, 20 components */
	case EXIF_TAG_DATE_TIME:
	case EXIF_TAG_DATE_TIME_ORIGINAL:
	case EXIF_TAG_DATE_TIME_DIGITIZED:
	{
		time_t t;
#ifdef HAVE_LOCALTIME_R
		struct tm tms;
#endif
		struct tm *tm;

		t = time (NULL);
#ifdef HAVE_LOCALTIME_R
		tm = localtime_r (&t, &tms);
#else
		tm = localtime (&t);
#endif
		e->components = 20;
		e->format = EXIF_FORMAT_ASCII;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		snprintf ((char *) e->data, e->size,
			  "%04i:%02i:%02i %02i:%02i:%02i",
			  tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			  tm->tm_hour, tm->tm_min, tm->tm_sec);
		break;
	}

	/* ASCII, no default */
	case EXIF_TAG_SUB_SEC_TIME:
	case EXIF_TAG_SUB_SEC_TIME_ORIGINAL:
	case EXIF_TAG_SUB_SEC_TIME_DIGITIZED:
		e->components = 0;
		e->format = EXIF_FORMAT_ASCII;
		e->size = 0;
		e->data = NULL;
		break;

	/* ASCII, default "[None]" */
	case EXIF_TAG_IMAGE_DESCRIPTION:
	case EXIF_TAG_MAKE:
	case EXIF_TAG_MODEL:
	case EXIF_TAG_SOFTWARE:
	case EXIF_TAG_ARTIST:
		e->components = strlen (_("[None]")) + 1;
		e->format = EXIF_FORMAT_ASCII;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		strncpy ((char *)e->data, _("[None]"), e->size);
		break;
	/* ASCII, default "[None]\0[None]\0" */
	case EXIF_TAG_COPYRIGHT:
		e->components = (strlen (_("[None]")) + 1) * 2;
		e->format = EXIF_FORMAT_ASCII;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		strcpy (((char *)e->data) + 0, _("[None]"));
		strcpy (((char *)e->data) + strlen (_("[None]")) + 1, _("[None]"));
		break;

	/* UNDEFINED, 1 component, default 1 */
	case EXIF_TAG_SCENE_TYPE:
		e->components = 1;
		e->format = EXIF_FORMAT_UNDEFINED;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		e->data[0] = 0x01;
		break;

	/* UNDEFINED, 1 component, default 3 */
	case EXIF_TAG_FILE_SOURCE:
		e->components = 1;
		e->format = EXIF_FORMAT_UNDEFINED;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = exif_entry_alloc (e, e->size);
		if (!e->data) { clear_entry(e); break; }
		e->data[0] = 0x03;
		break;

	/* UNDEFINED, 4 components, default 48 49 48 48 */
        case EXIF_TAG_FLASH_PIX_VERSION:
                e->components = 4;
                e->format = EXIF_FORMAT_UNDEFINED;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = exif_entry_alloc (e, e->size);
                if (!e->data) { clear_entry(e); break; }
                memcpy (e->data, "0100", 4);
                break;

        /* UNDEFINED, 4 components, default 48 50 49 48 */
        case EXIF_TAG_EXIF_VERSION:
                e->components = 4;
                e->format = EXIF_FORMAT_UNDEFINED;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = exif_entry_alloc (e, e->size);
                if (!e->data) { clear_entry(e); break; }
                memcpy (e->data, "0210", 4);
                break;

        /* UNDEFINED, 4 components, default 1 2 3 0 */
        case EXIF_TAG_COMPONENTS_CONFIGURATION:
                e->components = 4;
                e->format = EXIF_FORMAT_UNDEFINED;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = exif_entry_alloc (e, e->size);
                if (!e->data) { clear_entry(e); break; }
		e->data[0] = 1;
		e->data[1] = 2;
		e->data[2] = 3;
		e->data[3] = 0;
                break;

	/* UNDEFINED, no components, no default */
	/* Use this if the tag is otherwise unsupported */
	case EXIF_TAG_MAKER_NOTE:
	case EXIF_TAG_USER_COMMENT:
	default:
		e->components = 0;
		e->format = EXIF_FORMAT_UNDEFINED;
		e->size = 0;
		e->data = NULL;
		break;
	}
}