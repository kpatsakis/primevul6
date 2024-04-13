QPDFWriter::preserveObjectStreams()
{
    // Our object_to_object_stream map has to map ObjGen -> ObjGen
    // since we may be generating object streams out of old objects
    // that have generation numbers greater than zero.  However in an
    // existing PDF, all object stream objects and all objects in them
    // must have generation 0 because the PDF spec does not provide
    // any way to do otherwise.
    std::map<int, int> omap;
    QPDF::Writer::getObjectStreamData(this->pdf, omap);
    for (std::map<int, int>::iterator iter = omap.begin();
         iter != omap.end(); ++iter)
    {
        this->object_to_object_stream[QPDFObjGen((*iter).first, 0)] =
            (*iter).second;
    }
}