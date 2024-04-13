    void PngImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Exiv2::PngImage::readMetadata: Reading PNG file " << io_->path() << std::endl;
#endif
        if (io_->open() != 0)
        {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isPngType(*io_, true))
        {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "PNG");
        }
        clearMetadata();

        const long imgSize = (long) io_->size();
        DataBuf cheaderBuf(8);       // Chunk header size : 4 bytes (data size) + 4 bytes (chunk type).

        while(!io_->eof())
        {
            // Read chunk header.

#ifdef DEBUG
            std::cout << "Exiv2::PngImage::readMetadata: Position: " << io_->tell() << std::endl;
#endif
            std::memset(cheaderBuf.pData_, 0x0, cheaderBuf.size_);
            long bufRead = io_->read(cheaderBuf.pData_, cheaderBuf.size_);
            if (io_->error()) {
                throw Error(kerFailedToReadImageData);
            }
            if (bufRead != cheaderBuf.size_) {
                throw Error(kerInputDataReadFailed);
            }

            // Decode chunk data length.
            uint32_t dataOffset = Exiv2::getULong(cheaderBuf.pData_, Exiv2::bigEndian);
            long pos = io_->tell();
            if (pos == -1 ||
                dataOffset > uint32_t(0x7FFFFFFF) ||
                static_cast<long>(dataOffset) > imgSize - pos) {
                throw Exiv2::Error(kerFailedToReadImageData);
            }

#ifdef DEBUG
            std::string chunkName(reinterpret_cast<char *>(cheaderBuf.pData_) + 4, 4);
            std::cout << "Exiv2::PngImage::readMetadata: chunk name: " << chunkName << std::endl;
#endif

            // Perform a chunk triage for item that we need.
            if (!memcmp(cheaderBuf.pData_ + 4, "IEND", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "IHDR", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "tEXt", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "zTXt", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "iTXt", 4) ||
                !memcmp(cheaderBuf.pData_ + 4, "iCCP", 4))
            {
                // Extract chunk data.

                DataBuf cdataBuf(dataOffset);
                bufRead = io_->read(cdataBuf.pData_, dataOffset);
                if (io_->error()) {
                    throw Error(kerFailedToReadImageData);
                }
                if (bufRead != (long)dataOffset) {
                    throw Error(kerInputDataReadFailed);
                }

                if (!memcmp(cheaderBuf.pData_ + 4, "IEND", 4))
                {
                    // Last chunk found: we stop parsing.
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found IEND chunk with length: " << dataOffset << std::endl;
#endif
                    return;
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "IHDR", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found IHDR chunk with length: " << dataOffset << std::endl;
#endif
                    if (cdataBuf.size_ >= 8) {
                        PngChunk::decodeIHDRChunk(cdataBuf, &pixelWidth_, &pixelHeight_);
                    }
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "tEXt", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found tEXt chunk with length: " << dataOffset << std::endl;
#endif
                    PngChunk::decodeTXTChunk(this, cdataBuf, PngChunk::tEXt_Chunk);
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "zTXt", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found zTXt chunk with length: " << dataOffset << std::endl;
#endif
                    PngChunk::decodeTXTChunk(this, cdataBuf, PngChunk::zTXt_Chunk);
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "iTXt", 4))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found iTXt chunk with length: " << dataOffset << std::endl;
#endif
                    PngChunk::decodeTXTChunk(this, cdataBuf, PngChunk::iTXt_Chunk);
                }
                else if (!memcmp(cheaderBuf.pData_ + 4, "iCCP", 4))
                {
                    // The ICC profile name can vary from 1-79 characters.
                    uint32_t iccOffset = 0;
                    while (iccOffset < 80 && iccOffset < dataOffset) {

                        const byte* profileName = cdataBuf.pData_ + iccOffset;
                        ++iccOffset;

                        if (*profileName == 0x00)
                            break;
                    }

                    ++iccOffset; // +1 = 'compressed' flag

                    zlibToDataBuf(cdataBuf.pData_ +iccOffset,dataOffset-iccOffset,iccProfile_);
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::readMetadata: Found iCCP chunk length: " << dataOffset  << std::endl;
                    std::cout << "Exiv2::PngImage::readMetadata: iccProfile.size_ : " << iccProfile_.size_ << std::endl;
#endif
                }

                // Set dataOffset to null like chunk data have been extracted previously.
                dataOffset = 0;
            }

            // Move to the next chunk: chunk data size + 4 CRC bytes.
#ifdef DEBUG
            std::cout << "Exiv2::PngImage::readMetadata: Seek to offset: " << dataOffset + 4 << std::endl;
#endif
            io_->seek(dataOffset + 4 , BasicIo::cur);
            if (io_->error() || io_->eof()) {
                throw Error(kerFailedToReadImageData);
            }
        }

    } // PngImage::readMetadata