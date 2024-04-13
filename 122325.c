QPDFWriter::writeObject(QPDFObjectHandle object, int object_stream_index)
{
    QPDFObjGen old_og = object.getObjGen();

    if ((object_stream_index == -1) &&
        (old_og.getGen() == 0) &&
	(this->object_stream_to_objects.count(old_og.getObj())))
    {
	writeObjectStream(object);
	return;
    }

    int new_id = obj_renumber[old_og];
    if (this->qdf_mode)
    {
	if (this->page_object_to_seq.count(old_og))
	{
	    writeString("%% Page ");
	    writeString(
		QUtil::int_to_string(
		    this->page_object_to_seq[old_og]));
	    writeString("\n");
	}
	if (this->contents_to_page_seq.count(old_og))
	{
	    writeString("%% Contents for page ");
	    writeString(
		QUtil::int_to_string(
		    this->contents_to_page_seq[old_og]));
	    writeString("\n");
	}
    }
    if (object_stream_index == -1)
    {
	if (this->qdf_mode && (! this->suppress_original_object_ids))
	{
	    writeString("%% Original object ID: " +
			QUtil::int_to_string(object.getObjectID()) + " " +
			QUtil::int_to_string(object.getGeneration()) + "\n");
	}
	openObject(new_id);
	setDataKey(new_id);
	unparseObject(object, 0, 0);
	this->cur_data_key.clear();
	closeObject(new_id);
    }
    else
    {
	unparseObject(object, 0, f_in_ostream);
	writeString("\n");
    }

    if ((! this->direct_stream_lengths) && object.isStream())
    {
	if (this->qdf_mode)
	{
	    if (this->added_newline)
	    {
		writeString("%QDF: ignore_newline\n");
	    }
	}
	openObject(new_id + 1);
	writeString(QUtil::int_to_string(this->cur_stream_length));
	closeObject(new_id + 1);
    }
}