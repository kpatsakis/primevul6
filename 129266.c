exif_get_short_convert (const unsigned char *buf, ExifFormat format,
			ExifByteOrder order)
{
	switch (format) {
	case EXIF_FORMAT_LONG:
		return (ExifShort) exif_get_long (buf, order);
	case EXIF_FORMAT_SLONG:
		return (ExifShort) exif_get_slong (buf, order);
	case EXIF_FORMAT_SHORT:
		return (ExifShort) exif_get_short (buf, order);
	case EXIF_FORMAT_SSHORT:
		return (ExifShort) exif_get_sshort (buf, order);
	case EXIF_FORMAT_BYTE:
	case EXIF_FORMAT_SBYTE:
		return (ExifShort) buf[0];
	default:
		/* Unsupported type */
		return (ExifShort) 0;
	}
}