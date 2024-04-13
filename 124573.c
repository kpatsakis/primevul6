        void do_write_static()
        {
            is_writing = true;
            asio::write(adaptor_.socket(), buffers_);

            if (res.file_info.statResult == 0)
            {
                std::ifstream is(res.file_info.path.c_str(), std::ios::in | std::ios::binary);
                std::vector<asio::const_buffer> buffers{1};
                char buf[16384];
                is.read(buf, sizeof(buf));
                while (is.gcount() > 0)
                {
                    buffers[0] = asio::buffer(buf, is.gcount());
                    do_write_sync(buffers);
                    is.read(buf, sizeof(buf));
                }
            }
            is_writing = false;
            if (close_connection_)
            {
                adaptor_.shutdown_readwrite();
                adaptor_.close();
                CROW_LOG_DEBUG << this << " from write (static)";
                check_destroy();
            }

            res.end();
            res.clear();
            buffers_.clear();
        }