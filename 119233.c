Ptr<DISOpticalFlow> DISOpticalFlow::create(int preset)
{
    Ptr<DISOpticalFlow> dis = makePtr<DISOpticalFlowImpl>();
    dis->setPatchSize(8);
    if (preset == DISOpticalFlow::PRESET_ULTRAFAST)
    {
        dis->setFinestScale(2);
        dis->setPatchStride(4);
        dis->setGradientDescentIterations(12);
        dis->setVariationalRefinementIterations(0);
    }
    else if (preset == DISOpticalFlow::PRESET_FAST)
    {
        dis->setFinestScale(2);
        dis->setPatchStride(4);
        dis->setGradientDescentIterations(16);
        dis->setVariationalRefinementIterations(5);
    }
    else if (preset == DISOpticalFlow::PRESET_MEDIUM)
    {
        dis->setFinestScale(1);
        dis->setPatchStride(3);
        dis->setGradientDescentIterations(25);
        dis->setVariationalRefinementIterations(5);
    }

    return dis;
}