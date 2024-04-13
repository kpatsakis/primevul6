QPDFWriter::generateID()
{
    // Generate the ID lazily so that we can handle the user's
    // preference to use static or deterministic ID generation.

    if (! this->id2.empty())
    {
	return;
    }

    QPDFObjectHandle trailer = pdf.getTrailer();

    std::string result;

    if (this->static_id)
    {
	// For test suite use only...
	static unsigned char tmp[] = {0x31, 0x41, 0x59, 0x26,
                                      0x53, 0x58, 0x97, 0x93,
                                      0x23, 0x84, 0x62, 0x64,
                                      0x33, 0x83, 0x27, 0x95,
                                      0x00};
	result = reinterpret_cast<char*>(tmp);
    }
    else
    {
	// The PDF specification has guidelines for creating IDs, but
	// it states clearly that the only thing that's really
	// important is that it is very likely to be unique.  We can't
	// really follow the guidelines in the spec exactly because we
	// haven't written the file yet.  This scheme should be fine
	// though.  The deterministic ID case uses a digest of a
	// sufficient portion of the file's contents such no two
	// non-matching files would match in the subsets used for this
	// computation.  Note that we explicitly omit the filename from
	// the digest calculation for deterministic ID so that the same
	// file converted with qpdf, in that case, would have the same
	// ID regardless of the output file's name.

	std::string seed;
        if (this->deterministic_id)
        {
            if (this->deterministic_id_data.empty())
            {
                QTC::TC("qpdf", "QPDFWriter deterministic with no data");
                throw std::logic_error(
                    "INTERNAL ERROR: QPDFWriter::generateID has no"
                    " data for deterministic ID.  This may happen if"
                    " deterministic ID and file encryption are requested"
                    " together.");
            }
            seed += this->deterministic_id_data;
        }
        else
        {
            seed += QUtil::int_to_string(QUtil::get_current_time());
            seed += this->filename;
            seed += " ";
        }
	seed += " QPDF ";
	if (trailer.hasKey("/Info"))
	{
            QPDFObjectHandle info = trailer.getKey("/Info");
	    std::set<std::string> keys = info.getKeys();
	    for (std::set<std::string>::iterator iter = keys.begin();
		 iter != keys.end(); ++iter)
	    {
		QPDFObjectHandle obj = info.getKey(*iter);
		if (obj.isString())
		{
		    seed += " ";
		    seed += obj.getStringValue();
		}
	    }
	}

	MD5 m;
	m.encodeString(seed.c_str());
	MD5::Digest digest;
	m.digest(digest);
	result = std::string(reinterpret_cast<char*>(digest),
                             sizeof(MD5::Digest));
    }

    // If /ID already exists, follow the spec: use the original first
    // word and generate a new second word.  Otherwise, we'll use the
    // generated ID for both.

    this->id2 = result;
    if (trailer.hasKey("/ID"))
    {
	// Note: keep /ID from old file even if --static-id was given.
	this->id1 = trailer.getKey("/ID").getArrayItem(0).getStringValue();
    }
    else
    {
	this->id1 = this->id2;
    }
}