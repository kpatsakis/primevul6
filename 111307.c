CompositeDeepScanLine::Data::check_valid(const Header & header)
{

    bool has_z=false;
    bool has_alpha=false;
    // check good channel names
    for( ChannelList::ConstIterator i=header.channels().begin();i!=header.channels().end();++i)
    {
        std::string n(i.name()); 
        if(n=="ZBack")
        {
            _zback=true;
        }
        else if(n=="Z")
        {
            has_z=true;
        }
        else if(n=="A")
        {
            has_alpha=true;
        }
    }
    
    if(!has_z)
    {
        throw IEX_NAMESPACE::ArgExc("Deep data provided to CompositeDeepScanLine is missing a Z channel");
    }
    
    if(!has_alpha)
    {
        throw IEX_NAMESPACE::ArgExc("Deep data provided to CompositeDeepScanLine is missing an alpha channel");
    }
    
    
    if(_part.size()==0 && _file.size()==0)
    {
       // first in - update and return

       _dataWindow = header.dataWindow();
       
       return;
    }
    
    
    const Header * const match_header = _part.size()>0 ? &_part[0]->header() : &_file[0]->header();
    
    // check the sizes match
    if(match_header->displayWindow() != header.displayWindow())
    {
        throw IEX_NAMESPACE::ArgExc("Deep data provided to CompositeDeepScanLine has a different displayWindow to previously provided data");
    }
    
    _dataWindow.extendBy(header.dataWindow());
    
}