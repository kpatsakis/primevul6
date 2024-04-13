static void TIFFReadDirEntryOutputErr(TIFF* tif, enum TIFFReadDirEntryErr err, const char* module, const char* tagname, int recover)
{
	if (!recover) {
		switch (err) {
			case TIFFReadDirEntryErrCount:
				TIFFErrorExt(tif->tif_clientdata, module,
					     "Incorrect count for \"%s\"",
					     tagname);
				break;
			case TIFFReadDirEntryErrType:
				TIFFErrorExt(tif->tif_clientdata, module,
					     "Incompatible type for \"%s\"",
					     tagname);
				break;
			case TIFFReadDirEntryErrIo:
				TIFFErrorExt(tif->tif_clientdata, module,
					     "IO error during reading of \"%s\"",
					     tagname);
				break;
			case TIFFReadDirEntryErrRange:
				TIFFErrorExt(tif->tif_clientdata, module,
					     "Incorrect value for \"%s\"",
					     tagname);
				break;
			case TIFFReadDirEntryErrPsdif:
				TIFFErrorExt(tif->tif_clientdata, module,
			"Cannot handle different values per sample for \"%s\"",
					     tagname);
				break;
			case TIFFReadDirEntryErrSizesan:
				TIFFErrorExt(tif->tif_clientdata, module,
				"Sanity check on size of \"%s\" value failed",
					     tagname);
				break;
			case TIFFReadDirEntryErrAlloc:
				TIFFErrorExt(tif->tif_clientdata, module,
					     "Out of memory reading of \"%s\"",
					     tagname);
				break;
			default:
				assert(0);   /* we should never get here */
				break;
		}
	} else {
		switch (err) {
			case TIFFReadDirEntryErrCount:
				TIFFWarningExt(tif->tif_clientdata, module,
				"Incorrect count for \"%s\"; tag ignored",
					     tagname);
				break;
			case TIFFReadDirEntryErrType:
				TIFFWarningExt(tif->tif_clientdata, module,
				"Incompatible type for \"%s\"; tag ignored",
					       tagname);
				break;
			case TIFFReadDirEntryErrIo:
				TIFFWarningExt(tif->tif_clientdata, module,
			"IO error during reading of \"%s\"; tag ignored",
					       tagname);
				break;
			case TIFFReadDirEntryErrRange:
				TIFFWarningExt(tif->tif_clientdata, module,
				"Incorrect value for \"%s\"; tag ignored",
					       tagname);
				break;
			case TIFFReadDirEntryErrPsdif:
				TIFFWarningExt(tif->tif_clientdata, module,
	"Cannot handle different values per sample for \"%s\"; tag ignored",
					       tagname);
				break;
			case TIFFReadDirEntryErrSizesan:
				TIFFWarningExt(tif->tif_clientdata, module,
		"Sanity check on size of \"%s\" value failed; tag ignored",
					       tagname);
				break;
			case TIFFReadDirEntryErrAlloc:
				TIFFWarningExt(tif->tif_clientdata, module,
				"Out of memory reading of \"%s\"; tag ignored",
					       tagname);
				break;
			default:
				assert(0);   /* we should never get here */
				break;
		}
	}
}