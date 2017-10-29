//Performs detecion and tracing of wand by tracking the bright wand-tip blob 
//and saving the most recently detected keypoints to a deque
//which are then joined together to make a free-form trace
//Also contains handwriting recognition & training code to recognize wand traces

#include "stdafx.h"
#include "config.h"
#include"ImageProcessor.h" 

void ImageProcessor::init(int width, int height)
{
	_frameWidth = width;
	_frameHeight = height;

	wandMoveTracingFrame = Mat(_frameHeight, _frameWidth, CV_8U);
	cameraFrame = Mat(_frameHeight, _frameWidth, CV_8U);

	_pMOG2 = createBackgroundSubtractorMOG2(BGS_HISTORY_FRAMES);

	// Change thresholds
	_params.minThreshold = 150;
	_params.maxThreshold = 255;

	// Fliter by color
	_params.filterByColor = true;
	_params.blobColor = 255;

	// Filter by Area.
	_params.filterByArea = true;
	_params.minArea = 0.05f;
	_params.maxArea = 20;

	// Filter by Circularity
	_params.filterByCircularity = true;
	_params.minCircularity = 0.5;

	// Filter by Convexity
	_params.filterByConvexity = true;
	_params.minConvexity = 0.5;

	// Filter by Inertia
	_params.filterByInertia = false;
	//params.minInertiaRatio = 0.01;

	_blobDetector = SimpleBlobDetector::create(_params);


	hog = HOGDescriptor(
		Size(64, 64), //winSize  50 x 50
		Size(32, 32), //blocksize 32 x 32
		Size(16, 16), //blockStride, 8 x 8
		Size(16, 16), //cellSize, 16 x 16
		9, //nbins,
		1, //derivAper,
		-1, //winSigma,
		0, //histogramNormType,
		0.2, //L2HysThresh,
		0,//gammal correction,
		64,//nlevels=64
		1);

	return;
}

std::vector<KeyPoint> ImageProcessor::wandDetect(ushort frameData[], int numPixels)
{
	// Storage for blobs
	std::vector<KeyPoint> keypoints;
	
	//Copy frameData array to Mat
	for (uint i = 0; i < numPixels; i++)
	{
		uint8_t thisPixelIntensity = (uint8_t)(frameData[i] >> 8);
		cameraFrame.at<unsigned char>(i) = thisPixelIntensity;
		//Clear frame
		//wandMoveTracingFrame.at<unsigned char>(i) = 0;
	}
	if (cameraFrame.empty())
		return keypoints;

	//Background Elimination
	_pMOG2->apply(cameraFrame, _fgMaskMOG2);
	Mat bgSubtractedFrame;
	cameraFrame.copyTo(bgSubtractedFrame, _fgMaskMOG2);

	// Detect blobs
	_blobDetector->detect(bgSubtractedFrame, keypoints);
	
	return keypoints;
}

Mat ImageProcessor::getWandTrace(ushort frameData[], int numpixels)
{
	blobKeypoints = wandDetect(frameData, numpixels);

	//Add keypoints to deque. For now, take only the first found keypoint
	if( blobKeypoints.size() != 0 )
	{
		// ----------->> _lastKeyPointTime = now();
		auto currentKeypointTime = std::chrono::high_resolution_clock::now();

		if (tracePoints.size() != 0)
		{
			std::chrono::duration<double> elapsed = currentKeypointTime - _lastKeypointTime;
			Point pt1(tracePoints[tracePoints.size() - 1].pt.x, tracePoints[tracePoints.size() - 1].pt.y);
			Point pt2(blobKeypoints[0].pt.x, blobKeypoints[0].pt.y);

			if (_distance(pt1,pt2) / elapsed.count() >= MAX_TRACE_SPEED)
			{
				return wandMoveTracingFrame;
			}

			if (tracePoints.size() >= DEQUE_BUFFER_SIZE)
				tracePoints.pop_front();

			tracePoints.push_back(blobKeypoints[0]);
			//Point pt2(tracePoints[tracePoints.size() - 1].pt.x, tracePoints[tracePoints.size() - 1].pt.y);
			line(wandMoveTracingFrame, pt1, pt2, Scalar(255), TRACE_THICKNESS);
		}
		else
		{
			_lastKeypointTime = currentKeypointTime;
			tracePoints.push_back(blobKeypoints[0]);
		}
	}
	
	return wandMoveTracingFrame;
}

double ImageProcessor::_distance(Point& p, Point& q) {
	Point diff = p - q;
	return cv::sqrt(diff.x*diff.x + diff.y*diff.y);
}

bool ImageProcessor::wandVisible()
{
	if(blobKeypoints.size() == 0)
		return false;
	return true;
}

bool ImageProcessor::checkTraceValidity()
{
	if (blobKeypoints.size() == 0)
	{
		auto currentKeypointTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = currentKeypointTime - _lastKeypointTime;

		if (elapsed.count() < 4.0)
		{
			return false;
		}
	
		if (tracePoints.size() > DEQUE_BUFFER_SIZE - 5)
		{
			traceUpperCorner = Point(_frameWidth, _frameHeight);
			traceLowerCorner = Point(0, 0);

			//Draw a trace by connecting all the keypoints stored in the deque
			//Also update lower and upper bounds of the trace
			for (int i = 1; i < tracePoints.size(); i++)
			{
				if (tracePoints[i].size == -99.0)
					continue;
				Point pt1(tracePoints[i - 1].pt.x, tracePoints[i - 1].pt.y);
				Point pt2(tracePoints[i].pt.x, tracePoints[i].pt.y);

				//Min x,y = traceUpperCorner points
				//Max x,y = traceLowerCorner points
				if (pt1.x < traceUpperCorner.x)
					traceUpperCorner.x = pt1.x;
				if (pt1.x > traceLowerCorner.x)
					traceLowerCorner.x = pt1.x;
				if (pt1.y < traceUpperCorner.y)
					traceUpperCorner.y = pt1.y;
				if (pt1.y > traceLowerCorner.y)
					traceLowerCorner.y = pt1.y;
			}

			long traceArea = (traceLowerCorner.x - traceUpperCorner.x) * (traceLowerCorner.y - traceUpperCorner.y);
			cout << "Trace area:" << traceArea << endl;
			
			if (traceArea > MIN_0_TRACE_AREA)
				return true;
		}
		//It's been over four seconds since the last keypoint and trace isn't valid
		eraseTrace();
	}
	return false;
}

void ImageProcessor::eraseTrace()
{
	//Erase existing trace
	for (uint i = 0; i < (_frameHeight*_frameWidth); i++)
	{
		wandMoveTracingFrame.at<unsigned char>(i) = 0;
	}
	//Empty corresponding tracePoints
	while (tracePoints.size() != 0)
	{
		tracePoints.pop_front();
	}
}

Mat ImageProcessor::cropSaveTrace()
{
	if (traceUpperCorner.x > CROPPED_IMG_MARGIN)
		traceUpperCorner.x -= CROPPED_IMG_MARGIN;
	else
		traceUpperCorner.x = 0;

	if (traceUpperCorner.y > CROPPED_IMG_MARGIN)
		traceUpperCorner.y -= CROPPED_IMG_MARGIN;
	else
		traceUpperCorner.y = 0;

	if (traceLowerCorner.x < _frameWidth - CROPPED_IMG_MARGIN)
		traceLowerCorner.x += CROPPED_IMG_MARGIN;
	else
		traceLowerCorner.x = _frameWidth;

	if (traceLowerCorner.y < _frameHeight - CROPPED_IMG_MARGIN)
		traceLowerCorner.y += CROPPED_IMG_MARGIN;
	else
		traceLowerCorner.y = _frameHeight;


	int traceWidth = traceLowerCorner.x - traceUpperCorner.x;
	int traceHeight = traceLowerCorner.y - traceUpperCorner.y;

	Mat resizedCroppedTrace; 
	Size _size;
	

	if (traceHeight > traceWidth)
	{
		_size.height = TRAINER_IMAGE_WIN_SIZE;
		_size.width = traceWidth * TRAINER_IMAGE_WIN_SIZE / traceHeight ; //Since traceHeight & traceWidth are always gonna be > TRAINER_IMAGE_WIN_SIZE
	} 
	else
	{
		_size.width = TRAINER_IMAGE_WIN_SIZE;
		_size.height = traceHeight * TRAINER_IMAGE_WIN_SIZE / traceWidth;
	}
	cout << "Resized width: " << _size.width << endl;
	cout << "Resized height: " << _size.height << endl;

	resize(wandMoveTracingFrame(Rect(traceUpperCorner, traceLowerCorner)).clone(), resizedCroppedTrace, _size);
	cout << "Trace resized!" << endl;
	Mat finalTraceCell = Mat::zeros(TRAINER_IMAGE_WIN_SIZE, TRAINER_IMAGE_WIN_SIZE, CV_8U);
	for (int i = 0; i < resizedCroppedTrace.rows; i++)
	{
		for (int j = 0; j < resizedCroppedTrace.cols;j++)
		{
			
			finalTraceCell.at<unsigned char>(i,j) = resizedCroppedTrace.at<unsigned char>(i,j);
		}
	}
	cout << "Done copying to 64 x 64";
	imshow("Debug window", finalTraceCell);
	imwrite("spellTraceCell.png", finalTraceCell);
	return finalTraceCell;	
}

int ImageProcessor::recognizeSpell()
{
	Mat deskewedTrace = deskew(cropSaveTrace());
	cout << "Trace deskewed" << endl;
	vector<float> descriptors;
	hog.compute(deskewedTrace, descriptors);
	cout << "Hog computing done" << endl;
	Mat descriptorMatrix(1, descriptors.size(), CV_32FC1);
	Mat spellPrediction;
	ConvertVectortoMatrix(descriptors, descriptorMatrix);

	Ptr<SVM> svm = svm->load<SVM>("spellsModel.yml");
	cout << "svm file loaded" << endl;

	/*= SVM::create();
	svm->setGamma(0.50625);
	svm->setC(12.5);
	svm->setKernel(SVM::RBF);
	svm->setType(SVM::C_SVC);*/
	float prediction = svm->predict(descriptorMatrix);
	cout << prediction << endl;
	return (int)prediction;
}

void ImageProcessor::ConvertVectortoMatrix(vector<float> inHOG, Mat &outMat)
{
	int descriptor_size = inHOG.size();

	for (int i = 0;i<inHOG.size();i++) {
			outMat.at<float>(i) = inHOG[i];
		}
}

#if ENABLE_SPELL_TRAINING

void ImageProcessor::spellRecognitionTrainer()
{
	vector<Mat> trainCells;
	vector<int> trainLabels;
	loadTrainLabel(GESTURE_TRAINER_IMAGE, trainCells, trainLabels);

	vector<Mat> deskewedTrainCells;
	CreateDeskewedTrain(deskewedTrainCells, trainCells);

	std::vector<std::vector<float> > trainHOG;
	std::vector<std::vector<float> > testHOG;
	CreateTrainHOG(trainHOG, deskewedTrainCells);

	int descriptor_size = trainHOG[0].size();
	cout << "Descriptor Size : " << descriptor_size << endl;

	Mat trainMat(trainHOG.size(), descriptor_size, CV_32FC1);
	
	ConvertVectortoMatrix(trainHOG, trainMat);

	//Mat testResponse;
	SVMtrain(trainMat, trainLabels);

	//float count = 0;
	//float accuracy = 0;
	//SVMevaluate(testResponse, count, accuracy, testLabels);

	//cout << "the accuracy is :" << accuracy << endl;
}

void ImageProcessor::ConvertVectortoMatrix(vector<vector<float> > &inHOG, Mat &outMat)
{
	int descriptor_size = inHOG[0].size();

	for (int i = 0;i<inHOG.size();i++) {
		for (int j = 0;j<descriptor_size;j++) {
			outMat.at<float>(i, j) = inHOG[i][j];
		}
	}
}

void ImageProcessor::loadTrainLabel(String pathName, vector<Mat> &trainCells, vector<int> &trainLabels) {

	Mat img = imread(pathName, CV_LOAD_IMAGE_GRAYSCALE);

	int ImgCount = 0;
	for (int i = 0; i < img.rows; i = i + TRAINER_IMAGE_WIN_SIZE)
	{
		for (int j = 0; j < img.cols; j = j + TRAINER_IMAGE_WIN_SIZE)
		{
			Mat digitImg = (img.colRange(j, j + TRAINER_IMAGE_WIN_SIZE).rowRange(i, i + TRAINER_IMAGE_WIN_SIZE)).clone();
			trainCells.push_back(digitImg);
			ImgCount++;
		}
	}

	cout << "Image Count : " << ImgCount << endl;
	float digitClassNumber = 0;

	for (int z = 0;z<ImgCount;z++) {
		if (z % NO_OF_IMAGES_PER_ELEMENT == 0 && z != 0) {	
			digitClassNumber = digitClassNumber + 1;
		}
		trainLabels.push_back(digitClassNumber);
	}
}

void ImageProcessor::CreateDeskewedTrain(vector<Mat> &deskewedTrainCells, vector<Mat> &trainCells) {

	for (int i = 0;i<trainCells.size();i++) {

		Mat deskewedImg = deskew(trainCells[i]);
		deskewedTrainCells.push_back(deskewedImg);
	}
}

void ImageProcessor::CreateTrainHOG(vector<vector<float> > &trainHOG, vector<Mat> &deskewedtrainCells) {

	for (int y = 0;y<deskewedtrainCells.size();y++) {
		vector<float> descriptors;
		hog.compute(deskewedtrainCells[y], descriptors);
		trainHOG.push_back(descriptors);
	}
}

void ImageProcessor::getSVMParams(SVM *svm)
{
	cout << "Kernel type     : " << svm->getKernelType() << endl;
	cout << "Type            : " << svm->getType() << endl;
	cout << "C               : " << svm->getC() << endl;
	cout << "Degree          : " << svm->getDegree() << endl;
	cout << "Nu              : " << svm->getNu() << endl;
	cout << "Gamma           : " << svm->getGamma() << endl;
}

void  ImageProcessor::SVMtrain(Mat &trainMat, vector<int> &trainLabels) {


	Ptr<SVM> svm = SVM::create();
	svm->setGamma(0.50625);
	svm->setC(12.5);
	svm->setKernel(SVM::RBF);
	svm->setType(SVM::C_SVC);
	Ptr<TrainData> td = TrainData::create(trainMat, ROW_SAMPLE, trainLabels);
	svm->train(td);
	//	svm->trainAuto(td);
	svm->save("model4.yml");
	getSVMParams(svm);
}

//void  ImageProcessor::SVMevaluate(Mat &testResponse, float &count, float &accuracy, vector<int> &testLabels) {
//
//	for (int i = 0;i<testResponse.rows;i++)
//	{
//		cout << testResponse.at<float>(i, 0) << " " << testLabels[i] << endl;
//		if (testResponse.at<float>(i, 0) == testLabels[i]) {
//			count = count + 1;
//		}
//	}
//	accuracy = (count / testResponse.rows) * 100;
//}
#endif // ENABLE_SPELL_TRAINING


Mat ImageProcessor::deskew(Mat& img)
{
	int SZ = 20;
	float affineFlags = WARP_INVERSE_MAP | INTER_LINEAR;

	Moments m = moments(img);
	if (abs(m.mu02) < 1e-2)
	{
		//No deskewing needed
		return img.clone();
	}

	//Calculate skew based on central moments
	float skew = m.mu11 / m.mu02;

	//Calculate affine transform to correct skewness
	Mat warpMat = (Mat_<float>(2, 3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);
	Mat imgOut = Mat::zeros(img.rows, img.cols, img.type());
	warpAffine(img, imgOut, warpMat, imgOut.size(), affineFlags);

	return imgOut;

}



