TIFFPrintDirectory(TIFF* tif, FILE* fd, long flags)
{
	TIFFDirectory *td = &tif->tif_dir;
	char *sep;
	long l, n;

#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
	fprintf(fd, "TIFF Directory at offset 0x%I64x (%I64u)\n",
		(unsigned __int64) tif->tif_diroff,
		(unsigned __int64) tif->tif_diroff);
#else
	fprintf(fd, "TIFF Directory at offset 0x%llx (%llu)\n",
		(unsigned long long) tif->tif_diroff,
		(unsigned long long) tif->tif_diroff);
#endif
	if (TIFFFieldSet(tif,FIELD_SUBFILETYPE)) {
		fprintf(fd, "  Subfile Type:");
		sep = " ";
		if (td->td_subfiletype & FILETYPE_REDUCEDIMAGE) {
			fprintf(fd, "%sreduced-resolution image", sep);
			sep = "/";
		}
		if (td->td_subfiletype & FILETYPE_PAGE) {
			fprintf(fd, "%smulti-page document", sep);
			sep = "/";
		}
		if (td->td_subfiletype & FILETYPE_MASK)
			fprintf(fd, "%stransparency mask", sep);
		fprintf(fd, " (%lu = 0x%lx)\n",
		    (unsigned long) td->td_subfiletype, (long) td->td_subfiletype);
	}
	if (TIFFFieldSet(tif,FIELD_IMAGEDIMENSIONS)) {
		fprintf(fd, "  Image Width: %lu Image Length: %lu",
		    (unsigned long) td->td_imagewidth, (unsigned long) td->td_imagelength);
		if (TIFFFieldSet(tif,FIELD_IMAGEDEPTH))
			fprintf(fd, " Image Depth: %lu",
			    (unsigned long) td->td_imagedepth);
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_TILEDIMENSIONS)) {
		fprintf(fd, "  Tile Width: %lu Tile Length: %lu",
		    (unsigned long) td->td_tilewidth, (unsigned long) td->td_tilelength);
		if (TIFFFieldSet(tif,FIELD_TILEDEPTH))
			fprintf(fd, " Tile Depth: %lu",
			    (unsigned long) td->td_tiledepth);
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_RESOLUTION)) {
		fprintf(fd, "  Resolution: %g, %g",
		    td->td_xresolution, td->td_yresolution);
		if (TIFFFieldSet(tif,FIELD_RESOLUTIONUNIT)) {
			switch (td->td_resolutionunit) {
			case RESUNIT_NONE:
				fprintf(fd, " (unitless)");
				break;
			case RESUNIT_INCH:
				fprintf(fd, " pixels/inch");
				break;
			case RESUNIT_CENTIMETER:
				fprintf(fd, " pixels/cm");
				break;
			default:
				fprintf(fd, " (unit %u = 0x%x)",
				    td->td_resolutionunit,
				    td->td_resolutionunit);
				break;
			}
		}
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_POSITION))
		fprintf(fd, "  Position: %g, %g\n",
		    td->td_xposition, td->td_yposition);
	if (TIFFFieldSet(tif,FIELD_BITSPERSAMPLE))
		fprintf(fd, "  Bits/Sample: %u\n", td->td_bitspersample);
	if (TIFFFieldSet(tif,FIELD_SAMPLEFORMAT)) {
		fprintf(fd, "  Sample Format: ");
		switch (td->td_sampleformat) {
		case SAMPLEFORMAT_VOID:
			fprintf(fd, "void\n");
			break;
		case SAMPLEFORMAT_INT:
			fprintf(fd, "signed integer\n");
			break;
		case SAMPLEFORMAT_UINT:
			fprintf(fd, "unsigned integer\n");
			break;
		case SAMPLEFORMAT_IEEEFP:
			fprintf(fd, "IEEE floating point\n");
			break;
		case SAMPLEFORMAT_COMPLEXINT:
			fprintf(fd, "complex signed integer\n");
			break;
		case SAMPLEFORMAT_COMPLEXIEEEFP:
			fprintf(fd, "complex IEEE floating point\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_sampleformat, td->td_sampleformat);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_COMPRESSION)) {
		const TIFFCodec* c = TIFFFindCODEC(td->td_compression);
		fprintf(fd, "  Compression Scheme: ");
		if (c)
			fprintf(fd, "%s\n", c->name);
		else
			fprintf(fd, "%u (0x%x)\n",
			    td->td_compression, td->td_compression);
	}
	if (TIFFFieldSet(tif,FIELD_PHOTOMETRIC)) {
		fprintf(fd, "  Photometric Interpretation: ");
		if (td->td_photometric < NPHOTONAMES)
			fprintf(fd, "%s\n", photoNames[td->td_photometric]);
		else {
			switch (td->td_photometric) {
			case PHOTOMETRIC_LOGL:
				fprintf(fd, "CIE Log2(L)\n");
				break;
			case PHOTOMETRIC_LOGLUV:
				fprintf(fd, "CIE Log2(L) (u',v')\n");
				break;
			default:
				fprintf(fd, "%u (0x%x)\n",
				    td->td_photometric, td->td_photometric);
				break;
			}
		}
	}
	if (TIFFFieldSet(tif,FIELD_EXTRASAMPLES) && td->td_extrasamples) {
		uint16 i;
		fprintf(fd, "  Extra Samples: %u<", td->td_extrasamples);
		sep = "";
		for (i = 0; i < td->td_extrasamples; i++) {
			switch (td->td_sampleinfo[i]) {
			case EXTRASAMPLE_UNSPECIFIED:
				fprintf(fd, "%sunspecified", sep);
				break;
			case EXTRASAMPLE_ASSOCALPHA:
				fprintf(fd, "%sassoc-alpha", sep);
				break;
			case EXTRASAMPLE_UNASSALPHA:
				fprintf(fd, "%sunassoc-alpha", sep);
				break;
			default:
				fprintf(fd, "%s%u (0x%x)", sep,
				    td->td_sampleinfo[i], td->td_sampleinfo[i]);
				break;
			}
			sep = ", ";
		}
		fprintf(fd, ">\n");
	}
	if (TIFFFieldSet(tif,FIELD_INKNAMES)) {
		char* cp;
		uint16 i;
		fprintf(fd, "  Ink Names: ");
		i = td->td_samplesperpixel;
		sep = "";
		for (cp = td->td_inknames; 
		     i > 0 && cp < td->td_inknames + td->td_inknameslen; 
		     cp = strchr(cp,'\0')+1, i--) {
			size_t max_chars = 
				td->td_inknameslen - (cp - td->td_inknames);
			fputs(sep, fd);
			_TIFFprintAsciiBounded(fd, cp, max_chars);
			sep = ", ";
		}
                fputs("\n", fd);
	}
	if (TIFFFieldSet(tif,FIELD_THRESHHOLDING)) {
		fprintf(fd, "  Thresholding: ");
		switch (td->td_threshholding) {
		case THRESHHOLD_BILEVEL:
			fprintf(fd, "bilevel art scan\n");
			break;
		case THRESHHOLD_HALFTONE:
			fprintf(fd, "halftone or dithered scan\n");
			break;
		case THRESHHOLD_ERRORDIFFUSE:
			fprintf(fd, "error diffused\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_threshholding, td->td_threshholding);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_FILLORDER)) {
		fprintf(fd, "  FillOrder: ");
		switch (td->td_fillorder) {
		case FILLORDER_MSB2LSB:
			fprintf(fd, "msb-to-lsb\n");
			break;
		case FILLORDER_LSB2MSB:
			fprintf(fd, "lsb-to-msb\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_fillorder, td->td_fillorder);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_YCBCRSUBSAMPLING))
        {
		fprintf(fd, "  YCbCr Subsampling: %u, %u\n",
			td->td_ycbcrsubsampling[0], td->td_ycbcrsubsampling[1] );
	}
	if (TIFFFieldSet(tif,FIELD_YCBCRPOSITIONING)) {
		fprintf(fd, "  YCbCr Positioning: ");
		switch (td->td_ycbcrpositioning) {
		case YCBCRPOSITION_CENTERED:
			fprintf(fd, "centered\n");
			break;
		case YCBCRPOSITION_COSITED:
			fprintf(fd, "cosited\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_ycbcrpositioning, td->td_ycbcrpositioning);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_HALFTONEHINTS))
		fprintf(fd, "  Halftone Hints: light %u dark %u\n",
		    td->td_halftonehints[0], td->td_halftonehints[1]);
	if (TIFFFieldSet(tif,FIELD_ORIENTATION)) {
		fprintf(fd, "  Orientation: ");
		if (td->td_orientation < NORIENTNAMES)
			fprintf(fd, "%s\n", orientNames[td->td_orientation]);
		else
			fprintf(fd, "%u (0x%x)\n",
			    td->td_orientation, td->td_orientation);
	}
	if (TIFFFieldSet(tif,FIELD_SAMPLESPERPIXEL))
		fprintf(fd, "  Samples/Pixel: %u\n", td->td_samplesperpixel);
	if (TIFFFieldSet(tif,FIELD_ROWSPERSTRIP)) {
		fprintf(fd, "  Rows/Strip: ");
		if (td->td_rowsperstrip == (uint32) -1)
			fprintf(fd, "(infinite)\n");
		else
			fprintf(fd, "%lu\n", (unsigned long) td->td_rowsperstrip);
	}
	if (TIFFFieldSet(tif,FIELD_MINSAMPLEVALUE))
		fprintf(fd, "  Min Sample Value: %u\n", td->td_minsamplevalue);
	if (TIFFFieldSet(tif,FIELD_MAXSAMPLEVALUE))
		fprintf(fd, "  Max Sample Value: %u\n", td->td_maxsamplevalue);
	if (TIFFFieldSet(tif,FIELD_SMINSAMPLEVALUE)) {
		int i;
		int count = (tif->tif_flags & TIFF_PERSAMPLE) ? td->td_samplesperpixel : 1;
		fprintf(fd, "  SMin Sample Value:");
		for (i = 0; i < count; ++i)
			fprintf(fd, " %g", td->td_sminsamplevalue[i]);
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_SMAXSAMPLEVALUE)) {
		int i;
		int count = (tif->tif_flags & TIFF_PERSAMPLE) ? td->td_samplesperpixel : 1;
		fprintf(fd, "  SMax Sample Value:");
		for (i = 0; i < count; ++i)
			fprintf(fd, " %g", td->td_smaxsamplevalue[i]);
		fprintf(fd, "\n");
	}
	if (TIFFFieldSet(tif,FIELD_PLANARCONFIG)) {
		fprintf(fd, "  Planar Configuration: ");
		switch (td->td_planarconfig) {
		case PLANARCONFIG_CONTIG:
			fprintf(fd, "single image plane\n");
			break;
		case PLANARCONFIG_SEPARATE:
			fprintf(fd, "separate image planes\n");
			break;
		default:
			fprintf(fd, "%u (0x%x)\n",
			    td->td_planarconfig, td->td_planarconfig);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PAGENUMBER))
		fprintf(fd, "  Page Number: %u-%u\n",
		    td->td_pagenumber[0], td->td_pagenumber[1]);
	if (TIFFFieldSet(tif,FIELD_COLORMAP)) {
		fprintf(fd, "  Color Map: ");
		if (flags & TIFFPRINT_COLORMAP) {
			fprintf(fd, "\n");
			n = 1L<<td->td_bitspersample;
			for (l = 0; l < n; l++)
				fprintf(fd, "   %5ld: %5u %5u %5u\n",
				    l,
				    td->td_colormap[0][l],
				    td->td_colormap[1][l],
				    td->td_colormap[2][l]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_REFBLACKWHITE)) {
		int i;
		fprintf(fd, "  Reference Black/White:\n");
		for (i = 0; i < 3; i++)
		fprintf(fd, "    %2d: %5g %5g\n", i,
			td->td_refblackwhite[2*i+0],
			td->td_refblackwhite[2*i+1]);
	}
	if (TIFFFieldSet(tif,FIELD_TRANSFERFUNCTION)) {
		fprintf(fd, "  Transfer Function: ");
		if (flags & TIFFPRINT_CURVES) {
			fprintf(fd, "\n");
			n = 1L<<td->td_bitspersample;
			for (l = 0; l < n; l++) {
				uint16 i;
				fprintf(fd, "    %2ld: %5u",
				    l, td->td_transferfunction[0][l]);
				for (i = 1; i < td->td_samplesperpixel - td->td_extrasamples && i < 3; i++)
					fprintf(fd, " %5u",
					    td->td_transferfunction[i][l]);
				fputc('\n', fd);
			}
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif, FIELD_SUBIFD) && (td->td_subifd)) {
		uint16 i;
		fprintf(fd, "  SubIFD Offsets:");
		for (i = 0; i < td->td_nsubifd; i++)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, " %5I64u",
				(unsigned __int64) td->td_subifd[i]);
#else
			fprintf(fd, " %5llu",
				(unsigned long long) td->td_subifd[i]);
#endif
		fputc('\n', fd);
	}

	/*
	** Custom tag support.
	*/
	{
		int  i;
		short count;

		count = (short) TIFFGetTagListCount(tif);
		for(i = 0; i < count; i++) {
			uint32 tag = TIFFGetTagListEntry(tif, i);
			const TIFFField *fip;
			uint32 value_count;
			int mem_alloc = 0;
			void *raw_data;

			fip = TIFFFieldWithTag(tif, tag);
			if(fip == NULL)
				continue;

			if(fip->field_passcount) {
				if (fip->field_readcount == TIFF_VARIABLE2 ) {
					if(TIFFGetField(tif, tag, &value_count, &raw_data) != 1)
						continue;
				} else if (fip->field_readcount == TIFF_VARIABLE ) {
					uint16 small_value_count;
					if(TIFFGetField(tif, tag, &small_value_count, &raw_data) != 1)
						continue;
					value_count = small_value_count;
				} else {
					assert (fip->field_readcount == TIFF_VARIABLE
						|| fip->field_readcount == TIFF_VARIABLE2);
					continue;
				} 
			} else {
				if (fip->field_readcount == TIFF_VARIABLE
				    || fip->field_readcount == TIFF_VARIABLE2)
					value_count = 1;
				else if (fip->field_readcount == TIFF_SPP)
					value_count = td->td_samplesperpixel;
				else
					value_count = fip->field_readcount;
				if (fip->field_tag == TIFFTAG_DOTRANGE
				    && strcmp(fip->field_name,"DotRange") == 0) {
					/* TODO: This is an evil exception and should not have been
					   handled this way ... likely best if we move it into
					   the directory structure with an explicit field in 
					   libtiff 4.1 and assign it a FIELD_ value */
					static uint16 dotrange[2];
					raw_data = dotrange;
					TIFFGetField(tif, tag, dotrange+0, dotrange+1);
				} else if (fip->field_type == TIFF_ASCII
					   || fip->field_readcount == TIFF_VARIABLE
					   || fip->field_readcount == TIFF_VARIABLE2
					   || fip->field_readcount == TIFF_SPP
					   || value_count > 1) {
					if(TIFFGetField(tif, tag, &raw_data) != 1)
						continue;
				} else {
					raw_data = _TIFFmalloc(
					    _TIFFDataSize(fip->field_type)
					    * value_count);
					mem_alloc = 1;
					if(TIFFGetField(tif, tag, raw_data) != 1) {
						_TIFFfree(raw_data);
						continue;
					}
				}
			}

			/*
			 * Catch the tags which needs to be specially handled
			 * and pretty print them. If tag not handled in
			 * _TIFFPrettyPrintField() fall down and print it as
			 * any other tag.
			 */
			if (!_TIFFPrettyPrintField(tif, fip, fd, tag, value_count, raw_data))
				_TIFFPrintField(fd, fip, value_count, raw_data);

			if(mem_alloc)
				_TIFFfree(raw_data);
		}
	}
        
	if (tif->tif_tagmethods.printdir)
		(*tif->tif_tagmethods.printdir)(tif, fd, flags);

        _TIFFFillStriles( tif );
        
	if ((flags & TIFFPRINT_STRIPS) &&
	    TIFFFieldSet(tif,FIELD_STRIPOFFSETS)) {
		uint32 s;

		fprintf(fd, "  %lu %s:\n",
		    (unsigned long) td->td_nstrips,
		    isTiled(tif) ? "Tiles" : "Strips");
		for (s = 0; s < td->td_nstrips; s++)
#if defined(__WIN32__) && (defined(_MSC_VER) || defined(__MINGW32__))
			fprintf(fd, "    %3lu: [%8I64u, %8I64u]\n",
			    (unsigned long) s,
			    td->td_stripoffset ? (unsigned __int64) td->td_stripoffset[s] : 0,
			    td->td_stripbytecount ? (unsigned __int64) td->td_stripbytecount[s] : 0);
#else
			fprintf(fd, "    %3lu: [%8llu, %8llu]\n",
			    (unsigned long) s,
			    td->td_stripoffset ? (unsigned long long) td->td_stripoffset[s] : 0,
			    td->td_stripbytecount ? (unsigned long long) td->td_stripbytecount[s] : 0);
#endif
	}
}