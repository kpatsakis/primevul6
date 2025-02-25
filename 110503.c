initialise_banding(fz_context *ctx, render_details *render, int color)
{
	size_t min_band_mem;
	int bpp, h, w, reps;

	render->colorspace = output_cs;
	render->format = output_format;
#if GREY_FALLBACK != 0
	if (color == 0)
	{
		if (render->colorspace == CS_RGB)
		{
			/* Fallback from PPM to PGM */
			render->colorspace = CS_GRAY;
			render->format = OUT_PGM;
		}
		else if (render->colorspace == CS_CMYK)
		{
			render->colorspace = CS_GRAY;
			if (render->format == OUT_PKM)
				render->format = OUT_PBM;
			else
				render->format = OUT_PGM;
		}
	}
#endif

	switch (render->colorspace)
	{
	case CS_GRAY:
		bpp = 1;
		break;
	case CS_RGB:
		bpp = 2;
		break;
	default:
	case CS_CMYK:
		bpp = 3;
		break;
	}

	w = render->ibounds.x1 - render->ibounds.x0;
	min_band_mem = (size_t)bpp * w * min_band_height;
	reps = (int)(max_band_memory / min_band_mem);
	if (reps < 1)
		reps = 1;

	/* Adjust reps to even out the work between threads */
	if (render->num_workers > 0)
	{
		int runs, num_bands;
		h = render->ibounds.y1 - render->ibounds.y0;
		num_bands = (h + min_band_height - 1) / min_band_height;
		/* num_bands = number of min_band_height bands */
		runs = (num_bands + reps-1) / reps;
		/* runs = number of worker runs of reps min_band_height bands */
		runs = ((runs + render->num_workers - 1) / render->num_workers) * render->num_workers;
		/* runs = number of worker runs rounded up to make use of all our threads */
		reps = (num_bands + runs - 1) / runs;
	}

	render->band_height_multiple = reps;
	render->bands_rendered = 0;

	if (output_format == OUT_PGM || output_format == OUT_PPM)
	{
		render->bander = fz_new_pnm_band_writer(ctx, out);
		render->n = output_format == OUT_PGM ? 1 : 3;
	}
	else if (output_format == OUT_PAM)
	{
		render->bander = fz_new_pam_band_writer(ctx, out);
		render->n = 4;
	}
	else if (output_format == OUT_PBM)
	{
		render->bander = fz_new_pbm_band_writer(ctx, out);
		render->n = 1;
	}
	else if (output_format == OUT_PKM)
	{
		render->bander = fz_new_pkm_band_writer(ctx, out);
		render->n = 4;
	}
}