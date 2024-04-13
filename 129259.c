exif_entry_fix (ExifEntry *e)
{
	unsigned int i, newsize;
	unsigned char *newdata;
	ExifByteOrder o;
	ExifRational r;
	ExifSRational sr;

	if (!e || !e->priv) return;

	switch (e->tag) {
	
	/* These tags all need to be of format SHORT. */
	case EXIF_TAG_YCBCR_SUB_SAMPLING:
	case EXIF_TAG_SUBJECT_AREA:
	case EXIF_TAG_COLOR_SPACE:
	case EXIF_TAG_PLANAR_CONFIGURATION:
	case EXIF_TAG_SENSING_METHOD:
	case EXIF_TAG_ORIENTATION:
	case EXIF_TAG_YCBCR_POSITIONING:
	case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
	case EXIF_TAG_CUSTOM_RENDERED:
	case EXIF_TAG_EXPOSURE_MODE:
	case EXIF_TAG_WHITE_BALANCE:
	case EXIF_TAG_SCENE_CAPTURE_TYPE:
	case EXIF_TAG_GAIN_CONTROL:
	case EXIF_TAG_SATURATION:
	case EXIF_TAG_CONTRAST:
	case EXIF_TAG_SHARPNESS:
	case EXIF_TAG_ISO_SPEED_RATINGS:
		switch (e->format) {
		case EXIF_FORMAT_LONG:
		case EXIF_FORMAT_SLONG:
		case EXIF_FORMAT_BYTE:
		case EXIF_FORMAT_SBYTE:
		case EXIF_FORMAT_SSHORT:
			if (!e->parent || !e->parent->parent) break;
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag '%s' was of format '%s' (which is "
				"against specification) and has been "
				"changed to format '%s'."),
				exif_tag_get_name_in_ifd(e->tag,
							exif_entry_get_ifd(e)),
				exif_format_get_name (e->format),
				exif_format_get_name (EXIF_FORMAT_SHORT));

			o = exif_data_get_byte_order (e->parent->parent);
			newsize = e->components * exif_format_get_size (EXIF_FORMAT_SHORT);
			newdata = exif_entry_alloc (e, newsize);
			if (!newdata) {
				exif_entry_log (e, EXIF_LOG_CODE_NO_MEMORY,
					"Could not allocate %lu byte(s).", (unsigned long)newsize);
				break;
			}

			for (i = 0; i < e->components; i++)
				exif_set_short (
					newdata + i *
					exif_format_get_size (
					 EXIF_FORMAT_SHORT), o,
					 exif_get_short_convert (
					  e->data + i *
					  exif_format_get_size (e->format),
					  e->format, o));

			exif_mem_free (e->priv->mem, e->data);
			e->data = newdata;
			e->size = newsize;
			e->format = EXIF_FORMAT_SHORT;
			break;
		case EXIF_FORMAT_SHORT:
			/* No conversion necessary */
			break;
		default:
			exif_entry_log (e, EXIF_LOG_CODE_CORRUPT_DATA,
				_("Tag '%s' is of format '%s' (which is "
				"against specification) but cannot be changed "
				"to format '%s'."),
				exif_tag_get_name_in_ifd(e->tag,
							exif_entry_get_ifd(e)),
				exif_format_get_name (e->format),
				exif_format_get_name (EXIF_FORMAT_SHORT));
			break;
		}
		break;

	/* All these tags need to be of format 'Rational'. */
	case EXIF_TAG_FNUMBER:
	case EXIF_TAG_APERTURE_VALUE:
	case EXIF_TAG_EXPOSURE_TIME:
	case EXIF_TAG_FOCAL_LENGTH:
		switch (e->format) {
		case EXIF_FORMAT_SRATIONAL:
			if (!e->parent || !e->parent->parent) break;
			o = exif_data_get_byte_order (e->parent->parent);
			for (i = 0; i < e->components; i++) {
				sr = exif_get_srational (e->data + i * 
					exif_format_get_size (
						EXIF_FORMAT_SRATIONAL), o);
				r.numerator = (ExifLong) sr.numerator;
				r.denominator = (ExifLong) sr.denominator;
				exif_set_rational (e->data + i *
					exif_format_get_size (
						EXIF_FORMAT_RATIONAL), o, r);
			}
			e->format = EXIF_FORMAT_RATIONAL;
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag '%s' was of format '%s' (which is "
				"against specification) and has been "
				"changed to format '%s'."),
				exif_tag_get_name_in_ifd(e->tag,
							exif_entry_get_ifd(e)),
				exif_format_get_name (EXIF_FORMAT_SRATIONAL),
				exif_format_get_name (EXIF_FORMAT_RATIONAL));
			break;
		default:
			break;
		}
		break;

	/* All these tags need to be of format 'SRational'. */
	case EXIF_TAG_EXPOSURE_BIAS_VALUE:
	case EXIF_TAG_BRIGHTNESS_VALUE:
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		switch (e->format) {
		case EXIF_FORMAT_RATIONAL:
			if (!e->parent || !e->parent->parent) break;
			o = exif_data_get_byte_order (e->parent->parent);
			for (i = 0; i < e->components; i++) {
				r = exif_get_rational (e->data + i * 
					exif_format_get_size (
						EXIF_FORMAT_RATIONAL), o);
				sr.numerator = (ExifLong) r.numerator;
				sr.denominator = (ExifLong) r.denominator;
				exif_set_srational (e->data + i *
					exif_format_get_size (
						EXIF_FORMAT_SRATIONAL), o, sr);
			}
			e->format = EXIF_FORMAT_SRATIONAL;
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag '%s' was of format '%s' (which is "
				"against specification) and has been "
				"changed to format '%s'."),
				exif_tag_get_name_in_ifd(e->tag,
							exif_entry_get_ifd(e)),
				exif_format_get_name (EXIF_FORMAT_RATIONAL),
				exif_format_get_name (EXIF_FORMAT_SRATIONAL));
			break;
		default:
			break;
		}
		break;

	case EXIF_TAG_USER_COMMENT:

		/* Format needs to be UNDEFINED. */
		if (e->format != EXIF_FORMAT_UNDEFINED) {
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag 'UserComment' had invalid format '%s'. "
				"Format has been set to 'undefined'."),
				exif_format_get_name (e->format));
			e->format = EXIF_FORMAT_UNDEFINED;
		}

		/* Some packages like Canon ZoomBrowser EX 4.5 store
		   only one zero byte followed by 7 bytes of rubbish */
		if ((e->size >= 8) && (e->data[0] == 0)) {
			memcpy(e->data, "\0\0\0\0\0\0\0\0", 8);
		}

		/* There need to be at least 8 bytes. */
		if (e->size < 8) {
			e->data = exif_entry_realloc (e, e->data, 8 + e->size);
			if (!e->data) {
				clear_entry(e);
				return;
			}

			/* Assume ASCII */
			memmove (e->data + 8, e->data, e->size);
			memcpy (e->data, "ASCII\0\0\0", 8);
			e->size += 8;
			e->components += 8;
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag 'UserComment' has been expanded to at "
				"least 8 bytes in order to follow the "
				"specification."));
			break;
		}

		/*
		 * If the first 8 bytes are empty and real data starts
		 * afterwards, let's assume ASCII and claim the 8 first
		 * bytes for the format specifyer.
		 */
		for (i = 0; (i < e->size) && !e->data[i]; i++);
		if (!i) for ( ; (i < e->size) && (e->data[i] == ' '); i++);
		if ((i >= 8) && (i < e->size)) {
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag 'UserComment' is not empty but does not "
				"start with a format identifier. "
				"This has been fixed."));
			memcpy (e->data, "ASCII\0\0\0", 8);
			break;
		}

		/* 
		 * First 8 bytes need to follow the specification. If they don't, 
		 * assume ASCII.
		 */
		if (memcmp (e->data, "ASCII\0\0\0"     , 8) &&
		    memcmp (e->data, "UNICODE\0"       , 8) &&
		    memcmp (e->data, "JIS\0\0\0\0\0"   , 8) &&
		    memcmp (e->data, "\0\0\0\0\0\0\0\0", 8)) {
			e->data = exif_entry_realloc (e, e->data, 8 + e->size);
			if (!e->data) {
				clear_entry(e);
				break;
			}

			/* Assume ASCII */
			memmove (e->data + 8, e->data, e->size);
			memcpy (e->data, "ASCII\0\0\0", 8);
			e->size += 8;
			e->components += 8;
			exif_entry_log (e, EXIF_LOG_CODE_DEBUG,
				_("Tag 'UserComment' did not start with a "
				"format identifier. This has been fixed."));
			break;
		}

		break;
	default:
		break;
	}
}