TEST(DenseOpticalFlow_DIS, InvalidImgSize_CoarsestLevelLessThanZero)
{
    cv::Ptr<cv::DISOpticalFlow> of = cv::DISOpticalFlow::create();
    const int mat_size = 10;

    cv::Mat x(mat_size, mat_size, CV_8UC1, 42);
    cv::Mat y(mat_size, mat_size, CV_8UC1, 42);
    cv::Mat flow;

    ASSERT_THROW(of->calc(x, y, flow), cv::Exception);
}