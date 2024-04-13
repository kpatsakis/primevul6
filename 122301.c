QPDFWriter::initializeSpecialStreams()
{
    // Mark all page content streams in case we are filtering or
    // normalizing.
    std::vector<QPDFObjectHandle> pages = pdf.getAllPages();
    int num = 0;
    for (std::vector<QPDFObjectHandle>::iterator iter = pages.begin();
	 iter != pages.end(); ++iter)
    {
	QPDFObjectHandle& page = *iter;
	this->page_object_to_seq[page.getObjGen()] = ++num;
	QPDFObjectHandle contents = page.getKey("/Contents");
	std::vector<QPDFObjGen> contents_objects;
	if (contents.isArray())
	{
	    int n = contents.getArrayNItems();
	    for (int i = 0; i < n; ++i)
	    {
		contents_objects.push_back(
		    contents.getArrayItem(i).getObjGen());
	    }
	}
	else if (contents.isStream())
	{
	    contents_objects.push_back(contents.getObjGen());
	}

	for (std::vector<QPDFObjGen>::iterator iter = contents_objects.begin();
	     iter != contents_objects.end(); ++iter)
	{
	    this->contents_to_page_seq[*iter] = num;
	    this->normalized_streams.insert(*iter);
	}
    }
}