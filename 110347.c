_cairo_image_surface_create_from_jpeg (GInputStream  *istream,
				       GthFileData   *file_data,
				       int            requested_size,
				       int           *original_width_p,
				       int           *original_height_p,
				       gboolean      *loaded_original_p,
				       gpointer       user_data,
				       GCancellable  *cancellable,
				       GError       **error)
{
	GthImage                      *image;
	JpegInfoFlags		       info_flags;
	gboolean                       load_scaled;
	GthTransform                   orientation;
	int                            output_width;
	int                            output_height;
	int                            destination_width;
	int                            destination_height;
	int                            line_start;
	int                            line_step;
	int                            pixel_step;
	void                          *in_buffer;
	gsize                          in_buffer_size;
	JpegInfoData                   jpeg_info;
	struct error_handler_data      jsrcerr;
	struct jpeg_decompress_struct  srcinfo;
	cairo_surface_t               *surface;
	cairo_surface_metadata_t      *metadata;
	unsigned char                 *surface_data;
	unsigned char                 *surface_row;
	JSAMPARRAY                     buffer;
	int                            buffer_stride;
	JDIMENSION                     n_lines;
	JSAMPARRAY                     buffer_row;
	int                            l;
	unsigned char                 *p_surface;
	guchar                         r, g, b;
	guint32                        pixel;
	unsigned char                 *p_buffer;
	int                            x;
	gboolean                       read_all_scanlines = FALSE;
	gboolean                       finished = FALSE;

	image = gth_image_new ();
	surface = NULL;

	if (! _g_input_stream_read_all (istream,
			      	        &in_buffer,
			      	        &in_buffer_size,
			      	        cancellable,
			      	        error))
	{
		return image;
	}

	_jpeg_info_data_init (&jpeg_info);
	info_flags = _JPEG_INFO_EXIF_ORIENTATION;
#if HAVE_LCMS2
	info_flags |= _JPEG_INFO_EXIF_COLOR_SPACE | _JPEG_INFO_ICC_PROFILE;
#endif
	_jpeg_info_get_from_buffer (in_buffer, in_buffer_size, info_flags, &jpeg_info);
	if (jpeg_info.valid & _JPEG_INFO_EXIF_ORIENTATION)
		orientation = jpeg_info.orientation;
	else
		orientation = GTH_TRANSFORM_NONE;
#if HAVE_LCMS2
	{
		GthICCProfile *profile = NULL;

		if (jpeg_info.valid & _JPEG_INFO_ICC_PROFILE) {
			profile = gth_icc_profile_new (GTH_ICC_PROFILE_ID_UNKNOWN, cmsOpenProfileFromMem (jpeg_info.icc_data, jpeg_info.icc_data_size));
		}
		else if (jpeg_info.valid & _JPEG_INFO_EXIF_COLOR_SPACE) {
			if (jpeg_info.color_space == GTH_COLOR_SPACE_SRGB)
				profile = gth_icc_profile_new_srgb ();
		}

		if (profile != NULL) {
			gth_image_set_icc_profile (image, profile);
			g_object_unref (profile);
		}
	}
#endif
	_jpeg_info_data_dispose (&jpeg_info);

	srcinfo.err = jpeg_std_error (&(jsrcerr.pub));
	jsrcerr.pub.error_exit = fatal_error_handler;
	jsrcerr.pub.output_message = output_message_handler;
	jsrcerr.error = error;

	jpeg_create_decompress (&srcinfo);

	if (sigsetjmp (jsrcerr.setjmp_buffer, 1))
		goto stop_loading;

	_jpeg_memory_src (&srcinfo, in_buffer, in_buffer_size);

	jpeg_read_header (&srcinfo, TRUE);

	srcinfo.out_color_space = srcinfo.jpeg_color_space; /* make all the color space conversions manually */

	load_scaled = (requested_size > 0) && (requested_size < srcinfo.image_width) && (requested_size < srcinfo.image_height);
	if (load_scaled) {
		for (srcinfo.scale_denom = 1; srcinfo.scale_denom <= 16; srcinfo.scale_denom++) {
			jpeg_calc_output_dimensions (&srcinfo);
			if ((srcinfo.output_width < requested_size) || (srcinfo.output_height < requested_size)) {
				srcinfo.scale_denom -= 1;
				break;
			}
		}

		if (srcinfo.scale_denom <= 0) {
			srcinfo.scale_denom = srcinfo.scale_num;
			load_scaled = FALSE;
		}
	}

	jpeg_calc_output_dimensions (&srcinfo);

	buffer_stride = srcinfo.output_width * srcinfo.output_components;
	buffer = (*srcinfo.mem->alloc_sarray) ((j_common_ptr) &srcinfo, JPOOL_IMAGE, buffer_stride, srcinfo.rec_outbuf_height);

	jpeg_start_decompress (&srcinfo);

	output_width = MIN (srcinfo.output_width, CAIRO_MAX_IMAGE_SIZE);
	output_height = MIN (srcinfo.output_height, CAIRO_MAX_IMAGE_SIZE);
	_cairo_image_surface_transform_get_steps (CAIRO_FORMAT_ARGB32,
						  output_width,
						  output_height,
						  orientation,
						  &destination_width,
						  &destination_height,
						  &line_start,
						  &line_step,
						  &pixel_step);

#if 0
	g_print ("requested: %d, original [%d, %d] ==> load at [%d, %d]\n",
			requested_size,
			srcinfo.image_width,
			srcinfo.image_height,
			destination_width,
			destination_height);
#endif

	surface = _cairo_image_surface_create (CAIRO_FORMAT_ARGB32, destination_width, destination_height);
	if (surface == NULL) {
		jpeg_destroy_decompress (&srcinfo);
		g_free (in_buffer);

		return image;
	}

	metadata = _cairo_image_surface_get_metadata (surface);
	_cairo_metadata_set_has_alpha (metadata, FALSE);
	surface_data = _cairo_image_surface_flush_and_get_data (surface);
	surface_row = surface_data + line_start;

	switch (srcinfo.out_color_space) {
	case JCS_CMYK:
		{
			register unsigned char *cmyk_tab;
			int c, m, y, k, ki;

			CMYK_table_init ();
			cmyk_tab = CMYK_Tab;

			while (srcinfo.output_scanline < output_height) {
				if (g_cancellable_is_cancelled (cancellable))
					goto stop_loading;

				n_lines = jpeg_read_scanlines (&srcinfo, buffer, srcinfo.rec_outbuf_height);

				buffer_row = buffer;
				for (l = 0; l < n_lines; l++) {
					p_surface = surface_row;
					p_buffer = buffer_row[l];

					if (g_cancellable_is_cancelled (cancellable))
						goto stop_loading;

					for (x = 0; x < output_width; x++) {
						if (srcinfo.saw_Adobe_marker) {
							c = p_buffer[0];
							m = p_buffer[1];
							y = p_buffer[2];
							k = p_buffer[3];
						}
						else {
							c = 255 - p_buffer[0];
							m = 255 - p_buffer[1];
							y = 255 - p_buffer[2];
							k = 255 - p_buffer[3];
						}

						ki = k << 8; /* ki = k * 256 */
						r = cmyk_tab[ki + c];
						g = cmyk_tab[ki + m];
						b = cmyk_tab[ki + y];
						pixel = CAIRO_RGBA_TO_UINT32 (r, g, b, 0xff);
						memcpy (p_surface, &pixel, sizeof (guint32));

						p_surface += pixel_step;
						p_buffer += 4 /*srcinfo.output_components*/;
					}

					surface_row += line_step;
					buffer_row += buffer_stride;
				}
			}
		}
		break;

	case JCS_GRAYSCALE:
		{
			while (srcinfo.output_scanline < output_height) {
				if (g_cancellable_is_cancelled (cancellable))
					goto stop_loading;

				n_lines = jpeg_read_scanlines (&srcinfo, buffer, srcinfo.rec_outbuf_height);

				buffer_row = buffer;
				for (l = 0; l < n_lines; l++) {
					p_surface = surface_row;
					p_buffer = buffer_row[l];

					if (g_cancellable_is_cancelled (cancellable))
						goto stop_loading;

					for (x = 0; x < output_width; x++) {
						r = g = b = p_buffer[0];
						pixel = CAIRO_RGBA_TO_UINT32 (r, g, b, 0xff);
						memcpy (p_surface, &pixel, sizeof (guint32));

						p_surface += pixel_step;
						p_buffer += 1 /*srcinfo.output_components*/;
					}

					surface_row += line_step;
					buffer_row += buffer_stride;
				}
			}
		}
		break;

	case JCS_RGB:
		{
			while (srcinfo.output_scanline < output_height) {
				if (g_cancellable_is_cancelled (cancellable))
					goto stop_loading;

				n_lines = jpeg_read_scanlines (&srcinfo, buffer, srcinfo.rec_outbuf_height);

				buffer_row = buffer;
				for (l = 0; l < n_lines; l++) {
					p_surface = surface_row;
					p_buffer = buffer_row[l];

					if (g_cancellable_is_cancelled (cancellable))
						goto stop_loading;

					for (x = 0; x < output_width; x++) {
						r = p_buffer[0];
						g = p_buffer[1];
						b = p_buffer[2];
						pixel = CAIRO_RGBA_TO_UINT32 (r, g, b, 0xff);
						memcpy (p_surface, &pixel, sizeof (guint32));

						p_surface += pixel_step;
						p_buffer += 3 /*srcinfo.output_components*/;
					}

					surface_row += line_step;
					buffer_row += buffer_stride;
				}
			}
		}
		break;

	case JCS_YCbCr:
		{
			register JSAMPLE *range_limit = srcinfo.sample_range_limit;
			register int     *r_cr_tab;
			register int     *g_cb_tab;
			register int     *g_cr_tab;
			register int     *b_cb_tab;
			int               Y, Cb, Cr;

			YCbCr_tables_init ();
			r_cr_tab = YCbCr_R_Cr_Tab;
			g_cb_tab = YCbCr_G_Cb_Tab;
			g_cr_tab = YCbCr_G_Cr_Tab;
			b_cb_tab = YCbCr_B_Cb_Tab;

			while (srcinfo.output_scanline < output_height) {
				if (g_cancellable_is_cancelled (cancellable))
					goto stop_loading;

				n_lines = jpeg_read_scanlines (&srcinfo, buffer, srcinfo.rec_outbuf_height);

				buffer_row = buffer;
				for (l = 0; l < n_lines; l++) {
					p_surface = surface_row;
					p_buffer = buffer_row[l];

					if (g_cancellable_is_cancelled (cancellable))
						goto stop_loading;

					for (x = 0; x < output_width; x++) {
						Y = p_buffer[0];
						Cb = p_buffer[1];
						Cr = p_buffer[2];

						r = range_limit[Y + r_cr_tab[Cr]];
						g = range_limit[Y + SCALE_DOWN (g_cb_tab[Cb] + g_cr_tab[Cr])];
						b = range_limit[Y + b_cb_tab[Cb]];
						pixel = CAIRO_RGBA_TO_UINT32 (r, g, b, 0xff);
						memcpy (p_surface, &pixel, sizeof (guint32));

						p_surface += pixel_step;
						p_buffer += 3 /*srcinfo.output_components*/;
					}

					surface_row += line_step;
					buffer_row += buffer_stride;
				}
			}
		}
		break;

	case JCS_YCCK:
		{
			register JSAMPLE *range_limit = srcinfo.sample_range_limit;
			register int     *r_cr_tab;
			register int     *g_cb_tab;
			register int     *g_cr_tab;
			register int     *b_cb_tab;
			register guchar  *cmyk_tab;
			int               Y, Cb, Cr, K, Ki, c, m , y;

			YCbCr_tables_init ();
			r_cr_tab = YCbCr_R_Cr_Tab;
			g_cb_tab = YCbCr_G_Cb_Tab;
			g_cr_tab = YCbCr_G_Cr_Tab;
			b_cb_tab = YCbCr_B_Cb_Tab;

			CMYK_table_init ();
			cmyk_tab = CMYK_Tab;

			while (srcinfo.output_scanline < output_height) {
				if (g_cancellable_is_cancelled (cancellable))
					goto stop_loading;

				n_lines = jpeg_read_scanlines (&srcinfo, buffer, srcinfo.rec_outbuf_height);

				buffer_row = buffer;
				for (l = 0; l < n_lines; l++) {
					p_surface = surface_row;
					p_buffer = buffer_row[l];

					if (g_cancellable_is_cancelled (cancellable))
						goto stop_loading;

					for (x = 0; x < output_width; x++) {
						Y = p_buffer[0];
						Cb = p_buffer[1];
						Cr = p_buffer[2];
						K = p_buffer[3];

						c = range_limit[255 - (Y + r_cr_tab[Cr])];
						m = range_limit[255 - (Y + SCALE_DOWN (g_cb_tab[Cb] + g_cr_tab[Cr]))];
						y = range_limit[255 - (Y + b_cb_tab[Cb])];

						Ki = K << 8; /* ki = K * 256 */

						r = cmyk_tab[Ki + c];
						g = cmyk_tab[Ki + m];
						b = cmyk_tab[Ki + y];
						pixel = CAIRO_RGBA_TO_UINT32 (r, g, b, 0xff);
						memcpy (p_surface, &pixel, sizeof (guint32));

						p_surface += pixel_step;
						p_buffer += 4 /*srcinfo.output_components*/;
					}

					surface_row += line_step;
					buffer_row += buffer_stride;
				}
			}
		}
		break;

	case JCS_UNKNOWN:
	default:
		g_set_error (error,
                             GDK_PIXBUF_ERROR,
                             GDK_PIXBUF_ERROR_UNKNOWN_TYPE,
                             _("Unknown JPEG color space (%d)"),
                             srcinfo.out_color_space);
		break;
	}

	read_all_scanlines = TRUE;

	stop_loading:

	if (surface != NULL) {
		cairo_surface_mark_dirty (surface);

		if (! g_cancellable_is_cancelled (cancellable)) {
			int original_width;
			int original_height;

			/* Set the original dimensions */

			if ((orientation == GTH_TRANSFORM_ROTATE_90)
			     ||	(orientation == GTH_TRANSFORM_ROTATE_270)
			     ||	(orientation == GTH_TRANSFORM_TRANSPOSE)
			     ||	(orientation == GTH_TRANSFORM_TRANSVERSE))
			{
				original_width = srcinfo.image_height;
				original_height = srcinfo.image_width;
			}
			else {
				original_width = srcinfo.image_width;
				original_height = srcinfo.image_height;
			}

			_cairo_metadata_set_original_size (metadata, original_width, original_height);

			if (original_width_p != NULL)
				*original_width_p = original_width;
			if (original_height_p != NULL)
				*original_height_p = original_height;
			if (loaded_original_p != NULL)
				*loaded_original_p = ! load_scaled;

			/*_cairo_image_surface_set_attribute_int (surface, "Image::Rotation", rotation); FIXME*/
			/* FIXME _cairo_image_surface_set_attribute (surface, "Jpeg::ColorSpace", jpeg_color_space_name (srcinfo.jpeg_color_space)); */

			gth_image_set_cairo_surface (image, surface);
		}
		else
			g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_CANCELLED, "");

		cairo_surface_destroy (surface);
		surface = NULL; /* ignore other jpeg errors */
	}

	if (! finished) {
		finished = TRUE;
		if (read_all_scanlines)
			jpeg_finish_decompress (&srcinfo);
		else
			jpeg_abort_decompress (&srcinfo);
		jpeg_destroy_decompress (&srcinfo);
		g_free (in_buffer);
	}

	return image;
}