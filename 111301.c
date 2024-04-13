CompositeDeepScanLine::readPixels(int start, int end)
{
   size_t parts = _Data->_file.size() + _Data->_part.size(); // total of files+parts
   
   vector<DeepFrameBuffer> framebuffers(parts);
   vector< vector<unsigned int> > counts(parts);
   
   //
   // for each part, a pointer to an array of channels
   //
   vector<vector< vector<float *> > > pointers(parts);
   vector<const Header *> headers(parts);
   
   {
     size_t i;
     for(i=0;i<_Data->_file.size();i++)
     {
         headers[i] = &_Data->_file[i]->header();
     }
     
     for(size_t j=0;j<_Data->_part.size();j++)
     {
        headers[i+j] = &_Data->_part[j]->header();
     }
   }
   
   
   for(size_t i=0;i<parts;i++)
   {
     _Data->handleDeepFrameBuffer(framebuffers[i],counts[i],pointers[i],*headers[i],start,end);
   }
   
   //
   // set frame buffers and read scanlines from all parts
   // TODO what happens if SCANLINE not in data window?
   //
   
   {
       size_t i=0;
       for(i=0;i<_Data->_file.size();i++)
       {
            _Data->_file[i]->setFrameBuffer(framebuffers[i]);
            _Data->_file[i]->readPixelSampleCounts(start,end);
       }
       for(size_t j=0;j<_Data->_part.size();j++)
       {
           _Data->_part[j]->setFrameBuffer(framebuffers[i+j]);
           _Data->_part[j]->readPixelSampleCounts(start,end); 
       }
   }   
   
   
   //
   //  total width
   //
   
   size_t total_width = _Data->_dataWindow.size().x+1;
   size_t total_pixels = total_width*(end-start+1);
   vector<unsigned int> total_sizes(total_pixels);
   vector<unsigned int> num_sources(total_pixels); //number of parts with non-zero sample count
   
   size_t overall_sample_count=0; // sum of all samples in all images between start and end
   
   
   //
   // accumulate pixel counts
   //
   for(size_t ptr=0;ptr<total_pixels;ptr++)
   {
       total_sizes[ptr]=0;
       num_sources[ptr]=0;
       for(size_t j=0;j<parts;j++)
       {
          total_sizes[ptr]+=counts[j][ptr];
          if(counts[j][ptr]>0) num_sources[ptr]++;
       }
       overall_sample_count+=total_sizes[ptr];
       
       
       
   }
   
  
  
   
   //
   // allocate arrays for pixel data
   // samples array accessed as in pixels[channel][sample]
   //
   
   vector<vector<float> > samples( _Data->_channels.size() );
   
   for(size_t channel=0;channel<_Data->_channels.size();channel++)
   {
       if( channel!=1 || _Data->_zback)
       {            
           samples[channel].resize(overall_sample_count);
       }
   }
   
   for(size_t channel=0;channel<samples.size();channel++)
   {
       
       if( channel!=1 || _Data->_zback)
       {
           
           samples[channel].resize(overall_sample_count);
       
       
          //
          // allocate pointers for channel data
          //
          
          size_t offset=0;
       
          for(size_t pixel=0;pixel<total_pixels;pixel++)
          {
              for(size_t part=0 ; part<parts && offset<overall_sample_count ; part++ )
              {
                      pointers[part][channel][pixel]=&samples[channel][offset];           
                      offset+=counts[part][pixel];
              }
          }
       
       }
   }
   
   //
   // read data
   //
   
   for(size_t i=0;i<_Data->_file.size();i++)
   {
       _Data->_file[i]->readPixels(start,end);
   }
   for(size_t j=0;j<_Data->_part.size();j++)
   {
       _Data->_part[j]->readPixels(start,end); 
   }
   
   
   
   
   //
   // composite pixels and write back to framebuffer
  //
   
   
   // turn vector of strings into array of char *
   // and make sure 'ZBack' channel is correct
   vector<const char *> names(_Data->_channels.size());
   for(size_t i=0;i<names.size();i++)
   {
       names[i]=_Data->_channels[i].c_str();
   }
   
   if(!_Data->_zback) names[1]=names[0]; // no zback channel, so make it point to z

   
   
   TaskGroup g;
   for(int y=start;y<=end;y++)
   {
       ThreadPool::addGlobalTask(new LineCompositeTask(&g,_Data,y,start,&names,&pointers,&total_sizes,&num_sources));
   }//next row
}  