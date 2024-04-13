QPDFWriter::unparseObject(QPDFObjectHandle object, int level,
			  unsigned int flags, size_t stream_length,
                          bool compress)
{
    QPDFObjGen old_og = object.getObjGen();
    unsigned int child_flags = flags & ~f_stream;

    std::string indent;
    for (int i = 0; i < level; ++i)
    {
	indent += "  ";
    }

    if (object.isArray())
    {
	// Note: PDF spec 1.4 implementation note 121 states that
	// Acrobat requires a space after the [ in the /H key of the
	// linearization parameter dictionary.  We'll do this
	// unconditionally for all arrays because it looks nicer and
	// doesn't make the files that much bigger.
	writeString("[");
	writeStringQDF("\n");
	int n = object.getArrayNItems();
	for (int i = 0; i < n; ++i)
	{
	    writeStringQDF(indent);
	    writeStringQDF("  ");
	    writeStringNoQDF(" ");
	    unparseChild(object.getArrayItem(i), level + 1, child_flags);
	    writeStringQDF("\n");
	}
	writeStringQDF(indent);
	writeStringNoQDF(" ");
	writeString("]");
    }
    else if (object.isDictionary())
    {
        // Make a shallow copy of this object so we can modify it
        // safely without affecting the original.  This code makes
        // assumptions about things that are made true in
        // prepareFileForWrite, such as that certain things are direct
        // objects so that replacing them doesn't leave unreferenced
        // objects in the output.
        object = object.shallowCopy();

        // Handle special cases for specific dictionaries.

        // Extensions dictionaries.

        // We have one of several cases:
        //
        // * We need ADBE
        //    - We already have Extensions
        //       - If it has the right ADBE, preserve it
        //       - Otherwise, replace ADBE
        //    - We don't have Extensions: create one from scratch
        // * We don't want ADBE
        //    - We already have Extensions
        //       - If it only has ADBE, remove it
        //       - If it has other things, keep those and remove ADBE
        //    - We have no extensions: no action required
        //
        // Before writing, we guarantee that /Extensions, if present,
        // is direct through the ADBE dictionary, so we can modify in
        // place.

        bool is_root = false;
        bool have_extensions_other = false;
        bool have_extensions_adbe = false;

        QPDFObjectHandle extensions;
        if (old_og == pdf.getRoot().getObjGen())
        {
            is_root = true;
            if (object.hasKey("/Extensions") &&
                object.getKey("/Extensions").isDictionary())
            {
                extensions = object.getKey("/Extensions");
            }
        }

        if (extensions.isInitialized())
        {
            std::set<std::string> keys = extensions.getKeys();
            if (keys.count("/ADBE") > 0)
            {
                have_extensions_adbe = true;
                keys.erase("/ADBE");
            }
            if (keys.size() > 0)
            {
                have_extensions_other = true;
            }
        }

        bool need_extensions_adbe = (this->final_extension_level > 0);

        if (is_root)
        {
            if (need_extensions_adbe)
            {
                if (! (have_extensions_other || have_extensions_adbe))
                {
                    // We need Extensions and don't have it.  Create
                    // it here.
                    QTC::TC("qpdf", "QPDFWriter create Extensions",
                            this->qdf_mode ? 0 : 1);
                    extensions = QPDFObjectHandle::newDictionary();
                    object.replaceKey("/Extensions", extensions);
                }
            }
            else if (! have_extensions_other)
            {
                // We have Extensions dictionary and don't want one.
                if (have_extensions_adbe)
                {
                    QTC::TC("qpdf", "QPDFWriter remove existing Extensions");
                    object.removeKey("/Extensions");
                    extensions = QPDFObjectHandle(); // uninitialized
                }
            }
        }

        if (extensions.isInitialized())
        {
            QTC::TC("qpdf", "QPDFWriter preserve Extensions");
            QPDFObjectHandle adbe = extensions.getKey("/ADBE");
            if (adbe.isDictionary() &&
                adbe.hasKey("/BaseVersion") &&
                adbe.getKey("/BaseVersion").isName() &&
                (adbe.getKey("/BaseVersion").getName() ==
                 "/" + this->final_pdf_version) &&
                adbe.hasKey("/ExtensionLevel") &&
                adbe.getKey("/ExtensionLevel").isInteger() &&
                (adbe.getKey("/ExtensionLevel").getIntValue() ==
                 this->final_extension_level))
            {
                QTC::TC("qpdf", "QPDFWriter preserve ADBE");
            }
            else
            {
                if (need_extensions_adbe)
                {
                    extensions.replaceKey(
                        "/ADBE",
                        QPDFObjectHandle::parse(
                            "<< /BaseVersion /" + this->final_pdf_version +
                            " /ExtensionLevel " +
                            QUtil::int_to_string(this->final_extension_level) +
                            " >>"));
                }
                else
                {
                    QTC::TC("qpdf", "QPDFWriter remove ADBE");
                    extensions.removeKey("/ADBE");
                }
            }
        }

        // Stream dictionaries.

        if (flags & f_stream)
        {
            // Suppress /Length since we will write it manually
            object.removeKey("/Length");

	    if (flags & f_filtered)
            {
                // We will supply our own filter and decode
                // parameters.
                object.removeKey("/Filter");
                object.removeKey("/DecodeParms");
            }
            else
            {
                // Make sure, no matter what else we have, that we
                // don't have /Crypt in the output filters.
                QPDFObjectHandle filter = object.getKey("/Filter");
                QPDFObjectHandle decode_parms = object.getKey("/DecodeParms");
                if (filter.isOrHasName("/Crypt"))
                {
                    if (filter.isName())
                    {
                        object.removeKey("/Filter");
                        object.removeKey("/DecodeParms");
                    }
                    else
                    {
                        int idx = -1;
                        for (int i = 0; i < filter.getArrayNItems(); ++i)
                        {
                            QPDFObjectHandle item = filter.getArrayItem(i);
                            if (item.isName() && item.getName() == "/Crypt")
                            {
                                idx = i;
                                break;
                            }
                        }
                        if (idx >= 0)
                        {
                            // If filter is an array, then the code in
                            // QPDF_Stream has already verified that
                            // DecodeParms and Filters are arrays of
                            // the same length, but if they weren't
                            // for some reason, eraseItem does type
                            // and bounds checking.
                            QTC::TC("qpdf", "QPDFWriter remove Crypt");
                            filter.eraseItem(idx);
                            decode_parms.eraseItem(idx);
                        }
                    }
                }
            }
        }

	writeString("<<");
	writeStringQDF("\n");

	std::set<std::string> keys = object.getKeys();
	for (std::set<std::string>::iterator iter = keys.begin();
	     iter != keys.end(); ++iter)
	{
	    std::string const& key = *iter;

	    writeStringQDF(indent);
	    writeStringQDF("  ");
	    writeStringNoQDF(" ");
	    writeString(QPDF_Name::normalizeName(key));
	    writeString(" ");
            unparseChild(object.getKey(key), level + 1, child_flags);
	    writeStringQDF("\n");
	}

	if (flags & f_stream)
	{
	    writeStringQDF(indent);
	    writeStringQDF(" ");
	    writeString(" /Length ");

	    if (this->direct_stream_lengths)
	    {
		writeString(QUtil::int_to_string(stream_length));
	    }
	    else
	    {
		writeString(
		    QUtil::int_to_string(this->cur_stream_length_id));
		writeString(" 0 R");
	    }
	    writeStringQDF("\n");
	    if (compress && (flags & f_filtered))
	    {
		writeStringQDF(indent);
		writeStringQDF(" ");
		writeString(" /Filter /FlateDecode");
		writeStringQDF("\n");
	    }
	}

	writeStringQDF(indent);
	writeStringNoQDF(" ");
	writeString(">>");
    }
    else if (object.isStream())
    {
	// Write stream data to a buffer.
	int new_id = obj_renumber[old_og];
	if (! this->direct_stream_lengths)
	{
	    this->cur_stream_length_id = new_id + 1;
	}
	QPDFObjectHandle stream_dict = object.getDict();

	bool is_metadata = false;
	if (stream_dict.getKey("/Type").isName() &&
	    (stream_dict.getKey("/Type").getName() == "/Metadata"))
	{
	    is_metadata = true;
	}
	bool filter = (this->stream_data_mode != qpdf_s_preserve);
	if (this->stream_data_mode == qpdf_s_compress)
	{
	    // Don't filter if the stream is already compressed with
	    // FlateDecode.  We don't want to make it worse by getting
	    // rid of a predictor or otherwise messing with it.  We
	    // should also avoid messing with anything that's
	    // compressed with a lossy compression scheme, but we
	    // don't support any of those right now.
	    QPDFObjectHandle filter_obj = stream_dict.getKey("/Filter");
	    if (filter_obj.isName() &&
		((filter_obj.getName() == "/FlateDecode") ||
		 (filter_obj.getName() == "/Fl")))
	    {
		QTC::TC("qpdf", "QPDFWriter not recompressing /FlateDecode");
		filter = false;
	    }
	}
	bool normalize = false;
	bool compress = false;
	if (is_metadata &&
	    ((! this->encrypted) || (this->encrypt_metadata == false)))
	{
	    QTC::TC("qpdf", "QPDFWriter not compressing metadata");
	    filter = true;
	    compress = false;
	}
	else if (this->normalize_content && normalized_streams.count(old_og))
	{
	    normalize = true;
	    filter = true;
	}
	else if (filter && (this->stream_data_mode == qpdf_s_compress))
	{
	    compress = true;
	    QTC::TC("qpdf", "QPDFWriter compressing uncompressed stream");
	}

	flags |= f_stream;

        if (filter && this->precheck_streams)
        {
            try
            {
                QTC::TC("qpdf", "QPDFWriter precheck stream");
                Pl_Discard discard;
                filter = object.pipeStreamData(
                    &discard, true, false, false, true);
            }
            catch (std::exception&)
            {
                filter = false;
            }
        }

	pushPipeline(new Pl_Buffer("stream data"));
	activatePipelineStack();
	bool filtered =
	    object.pipeStreamData(this->pipeline, filter, normalize, compress);
	PointerHolder<Buffer> stream_data;
	popPipelineStack(&stream_data);
	if (filtered)
	{
	    flags |= f_filtered;
	}
	else
	{
	    compress = false;
	}

	this->cur_stream_length = stream_data->getSize();
	if (is_metadata && this->encrypted && (! this->encrypt_metadata))
	{
	    // Don't encrypt stream data for the metadata stream
	    this->cur_data_key.clear();
	}
	adjustAESStreamLength(this->cur_stream_length);
	unparseObject(stream_dict, 0, flags, this->cur_stream_length, compress);
	writeString("\nstream\n");
	pushEncryptionFilter();
	writeBuffer(stream_data);
	char last_char = this->pipeline->getLastChar();
	popPipelineStack();

        if (this->newline_before_endstream ||
            (this->qdf_mode && (last_char != '\n')))
        {
            writeString("\n");
            this->added_newline = true;
        }
        else
        {
            this->added_newline = false;
        }
	writeString("endstream");
    }
    else if (object.isString())
    {
	std::string val;
	if (this->encrypted &&
	    (! (flags & f_in_ostream)) &&
	    (! this->cur_data_key.empty()))
	{
	    val = object.getStringValue();
	    if (this->encrypt_use_aes)
	    {
		Pl_Buffer bufpl("encrypted string");
		Pl_AES_PDF pl(
                    "aes encrypt string", &bufpl, true,
                    QUtil::unsigned_char_pointer(this->cur_data_key),
                    this->cur_data_key.length());
		pl.write(QUtil::unsigned_char_pointer(val), val.length());
		pl.finish();
		Buffer* buf = bufpl.getBuffer();
		val = QPDF_String(
		    std::string(reinterpret_cast<char*>(buf->getBuffer()),
				buf->getSize())).unparse(true);
		delete buf;
	    }
	    else
	    {
		char* tmp = QUtil::copy_string(val);
		size_t vlen = val.length();
		RC4 rc4(QUtil::unsigned_char_pointer(this->cur_data_key),
			this->cur_data_key.length());
		rc4.process(QUtil::unsigned_char_pointer(tmp), vlen);
		val = QPDF_String(std::string(tmp, vlen)).unparse();
		delete [] tmp;
	    }
	}
	else
	{
	    val = object.unparseResolved();
	}
	writeString(val);
    }
    else
    {
	writeString(object.unparseResolved());
    }
}