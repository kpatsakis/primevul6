DeepScanLineInputFile::DeepScanLineInputFile
    (OPENEXR_IMF_INTERNAL_NAMESPACE::IStream &is, int numThreads)
:
     _data (new Data (numThreads))
{
    _data->_deleteStream = false;
    _data->_streamData = nullptr;

    try
    {
        readMagicNumberAndVersionField(is, _data->version);
        //
        // Backward compatibility to read multpart file.
        // multiPartInitialize will create _streamData
        if (isMultiPart(_data->version))
        {
            compatibilityInitialize(is);
            return;
        }
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        if (_data)       delete _data;

        REPLACE_EXC (e, "Cannot read image file "
                     "\"" << is.fileName() << "\". " << e.what());
        throw;
    }

    //
    // not multiPart - allocate stream data and intialise as normal
    //
    try
    {
        _data->_streamData = new InputStreamMutex();
        _data->_streamData->is = &is;
        _data->memoryMapped = is.isMemoryMapped();
        _data->header.readFrom (*_data->_streamData->is, _data->version);
        _data->header.sanityCheck (isTiled (_data->version));

        initialize(_data->header);

        readLineOffsets (*_data->_streamData->is,
                         _data->lineOrder,
                         _data->lineOffsets,
                         _data->fileIsComplete);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        if (_data && _data->_streamData)
        {
            delete _data->_streamData;
        }
        if (_data)       delete _data;

        REPLACE_EXC (e, "Cannot read image file "
                     "\"" << is.fileName() << "\". " << e.what());
        throw;
    }
    catch (...)
    {
        if (_data && _data->_streamData)
        {
            delete _data->_streamData;
        }
        if (_data)       delete _data;

        throw;
    }
}