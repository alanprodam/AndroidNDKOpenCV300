#include <com_unicamp_alantavares_monocularopencv300_OpencvNativeClass.h>
#include <android/log.h>

#define  LOG_TAG    "someTag"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

Mat R_f, t_f; //the final rotation and tranlation vectors containing the
double scale = 1.00;
// feature detection, tracking
vector<Point2f> points1, points2;   //vectors to store the coordinates of the feature points
vector<uchar> status;

JNIEXPORT void JNICALL Java_com_unicamp_alantavares_monocularopencv300_OpencvNativeClass_FindFeatures
(JNIEnv *, jclass, jlong addrGray1, jlong addrGray2){
    Mat& imgGray1 = *(Mat*)addrGray1;
    Mat& imgGray2 = *(Mat*)addrGray2;


    int cont = 0;
    featureDetection(imgGray1, points1);    //detect features in imgGray1

    double focal = 720;
    //double focal = 720;
    //double focal = 24.000;
    cv::Point2d pp(240,360);
    //cv::Point2d pp(176,144);
    //3.8cm  6.5cm
    Mat E, R, t, mask;


    if((points1.size()!=NULL) && (points1.size() > 80)){

        featureTracking(imgGray1, imgGray2, points1, points2, status); //track those features to imgGray2

        //for(int i=0;i<points1.size();i++){
          //  circle(imgGray1, Point(points1.at(i).x, points1.at(i).y) ,3, CV_RGB(255,255,255), 1);
        //}

        E = findEssentialMat(points2, points1, focal, pp, RANSAC, 0.999, 1.0, mask);
        recoverPose(E, points2, points1, R, t, focal, pp, mask);
        //LOGD( "Print from JNI - Translacao x: %lf y: %lf z: %lf", t.at<double>(0), t.at<double>(1), t.at<double>(2));
        LOGD( "Numero de Features: %d", points1.size());

        if(cont==0){
            R_f = R.clone();
            t_f = t.clone();
        }


        //if ((cont > 0) && (t.at<double>(2) > t.at<double>(0)) && (t.at<double>(2) > t.at<double>(1))) {
            t_f = t_f + scale*(R_f*t);
            R_f = R*R_f;
            LOGD( "Translacao x: %lf y: %lf z: %lf", t_f.at<double>(0), t_f.at<double>(1), t_f.at<double>(2));
        //}
    }
    else{
        //featureDetection(imgGray1, points1);    //detect features in imgGray1
    }

    //imgGray2 = imgGray1.clone();
    //points2 = points1;

    //__android_log_print(ANDROID_LOG_ERROR, "TRACKERS", "%d", int(X));
    //LOGD( "This is a number from JNI: %d", X);
    //LOGD( "This is a number from JNI: %d", points2.size());
    LOGD( "Valor cont: %d", cont);
    cont=cont+1;

}

//**********************************************************************************************************
//**********************************************************************************************************
void featureDetection(Mat& imgGray1, vector<Point2f>& points1)	{
    //uses FAST as of now, modify parameters as necessary
    vector<KeyPoint> keypoints_1;
    int fast_threshold = 15;
    bool nonmaxSuppression = true;

    FAST(imgGray1, keypoints_1, fast_threshold, nonmaxSuppression);
    KeyPoint::convert(keypoints_1, points1, vector<int>());

}
//**********************************************************************************************************
//**********************************************************************************************************

void featureTracking(Mat& imgGray1, Mat& imgGray2, vector<Point2f>& points1, vector<Point2f>& points2, vector<uchar>& status)	{

//this function automatically gets rid of points for which tracking fails

    vector<float> err;
    Size winSize=Size(19,19);
    TermCriteria termcrit=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);

    calcOpticalFlowPyrLK(imgGray1, imgGray2, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);

    //getting rid of points for which the KLT tracking failed or those who have gone outside the frame
    int indexCorrection = 0;
    for( int i=0; i<status.size(); i++)
    {  Point2f pt = points2.at(i- indexCorrection);
        if ((status.at(i) == 0)||(pt.x<0)||(pt.y<0))	{
            if((pt.x<0)||(pt.y<0))	{
                status.at(i) = 0;
            }
            points1.erase (points1.begin() + (i - indexCorrection));
            points2.erase (points2.begin() + (i - indexCorrection));
            indexCorrection++;
        }

    }

}