QPDFWriter::prepareFileForWrite()
{
    // Do a traversal of the entire PDF file structure replacing all
    // indirect objects that QPDFWriter wants to be direct.  This
    // includes stream lengths, stream filtering parameters, and
    // document extension level information.

    std::list<QPDFObjectHandle> queue;
    queue.push_back(getTrimmedTrailer());
    std::set<int> visited;

    while (! queue.empty())
    {
	QPDFObjectHandle node = queue.front();
	queue.pop_front();
	if (node.isIndirect())
	{
	    if (visited.count(node.getObjectID()) > 0)
	    {
		continue;
	    }
	    visited.insert(node.getObjectID());
	}

	if (node.isArray())
	{
	    int nitems = node.getArrayNItems();
	    for (int i = 0; i < nitems; ++i)
	    {
		QPDFObjectHandle oh = node.getArrayItem(i);
                if (oh.isIndirect() && oh.isNull())
                {
                    QTC::TC("qpdf", "QPDFWriter flatten array null");
                    oh.makeDirect();
                    node.setArrayItem(i, oh);
                }
		else if (! oh.isScalar())
		{
		    queue.push_back(oh);
		}
	    }
	}
	else if (node.isDictionary() || node.isStream())
	{
            bool is_stream = false;
            bool is_root = false;
            bool filterable = false;
	    QPDFObjectHandle dict = node;
	    if (node.isStream())
	    {
                is_stream = true;
		dict = node.getDict();
                // See whether we are able to filter this stream.
                filterable = node.pipeStreamData(0, true, false, false);
	    }
            else if (pdf.getRoot().getObjectID() == node.getObjectID())
            {
                is_root = true;
            }

	    std::set<std::string> keys = dict.getKeys();
	    for (std::set<std::string>::iterator iter = keys.begin();
		 iter != keys.end(); ++iter)
	    {
		std::string const& key = *iter;
		QPDFObjectHandle oh = dict.getKey(key);
                bool add_to_queue = true;
                if (is_stream)
                {
                    if (oh.isIndirect() &&
                        ((key == "/Length") ||
                         (filterable &&
                          ((key == "/Filter") ||
                           (key == "/DecodeParms")))))
                    {
                        QTC::TC("qpdf", "QPDFWriter make stream key direct");
                        add_to_queue = false;
                        oh.makeDirect();
                        dict.replaceKey(key, oh);
                    }
                }
                else if (is_root)
                {
                    if ((key == "/Extensions") && (oh.isDictionary()))
                    {
                        bool extensions_indirect = false;
                        if (oh.isIndirect())
                        {
                            QTC::TC("qpdf", "QPDFWriter make Extensions direct");
                            extensions_indirect = true;
                            add_to_queue = false;
                            oh = oh.shallowCopy();
                            dict.replaceKey(key, oh);
                        }
                        if (oh.hasKey("/ADBE"))
                        {
                            QPDFObjectHandle adbe = oh.getKey("/ADBE");
                            if (adbe.isIndirect())
                            {
                                QTC::TC("qpdf", "QPDFWriter make ADBE direct",
                                        extensions_indirect ? 0 : 1);
                                adbe.makeDirect();
                                oh.replaceKey("/ADBE", adbe);
                            }
                        }
                    }
                }

                if (add_to_queue)
                {
                    queue.push_back(oh);
		}
	    }
	}
    }
}