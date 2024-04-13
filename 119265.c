TEST(DenseOpticalFlow_DIS, InvalidImgSize_CoarsestLevelLessThanFinestLevel)
{
    cv::Ptr<cv::DISOpticalFlow> of = cv::DISOpticalFlow::create();
    const int mat_size = 80;

    cv::Mat x(mat_size, mat_size, CV_8UC1, 42);
    cv::Mat y(mat_size, mat_size, CV_8UC1, 42);
    cv::Mat flow;

    of->calc(x, y, flow);

    ASSERT_EQ(flow.rows, mat_size);
    ASSERT_EQ(flow.cols, mat_size);
}