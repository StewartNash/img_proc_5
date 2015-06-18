/*
Author:			Stewart Nash
File:			img_proc_5.cpp
Date:			June 17, 2015
Description:	Morphological isolation of red objects in video. Output is black and white.
Arguments:		img_proc_5.exe [input filename] [output filename]
*/
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int process_image(Mat & inputImage, Mat & outputImage);

//RANGES represents number of separate hue spectra. In order to change, you must change process_image().
const int RANGES = 2;
//const int FRAME_COUNT = 30000;


/*
Variables
---------
myVideo - holds video which is captured from input file.
myOutput - holds output video.
myImage - holds captured frame as color image.
myMask - holds black and white one channel image from processed function.
myCopy - holds color version of myMask.
myLeft - left half of frame consists of original image.
myRight - right half of frame consists of processed image.
myStitch - image sent to output consisting of stitched myLeft and myRight.
*/
int main(int argc, char * argv[])
{
	VideoCapture myVideo;
	VideoWriter myOutput;
	Mat myImage;
	Mat myMask;
	Mat myCopy;
	int elem_width;
	int elem_height;

	unsigned long long int i, j;
	
	//Open input video file.
	//myVideo.open("F:/Documents/Visual Studio 2010/Projects/img_proc_5/Debug/red_balloon.avi");
	if (argc > 1)
		myVideo.open(argv[1]);
	else
	{
		std::cout << "No filename provided.";
		waitKey(0);
		return 0;
	}
	//Open output video file.
	if (!myVideo.isOpened())
	{
		std::cout << "Unable to open file.";
		waitKey(0);
		return 0;
	}
	//Video frame will be twice the width of the original image.
	elem_width = (int) myVideo.get(CV_CAP_PROP_FRAME_WIDTH);
	elem_height = (int) myVideo.get(CV_CAP_PROP_FRAME_HEIGHT);
	Size mySize = Size(2 * elem_width, elem_height);
	if (argc > 2)
		myOutput.open(argv[2], CV_FOURCC('M','J','P','G'), myVideo.get(CV_CAP_PROP_FPS), mySize);
	else
		myOutput.open("output.avi", CV_FOURCC('M','J','P','G'), myVideo.get(CV_CAP_PROP_FPS), mySize);
	//myOutput.open("F:/Documents/Visual Studio 2010/Projects/img_proc_5/Debug/img_proc_5_out.avi", CV_FOURCC('M','J','P','G'), myVideo.get(CV_CAP_PROP_FPS), mySize);
	if (!myOutput.isOpened())
	{
		std::cout << "Unable to create file.";
		waitKey(0);
		return 0;
	}
	//The left and right regions of interest of the frame are mapped onto myLeft and myRight through their constructors.
	Mat myStitch(elem_height, 2 * elem_width, CV_8UC3);
	Mat myLeft(myStitch, Rect(0, 0, elem_width, elem_height));
	Mat myRight(myStitch, Rect(elem_width, 0, elem_width, elem_height));

	/*
	do
	{
		myVideo >> myImage;
		if (myImage.data)
		{
			process_image(myImage, myMask);
			imshow("Original Image", myImage);
			imshow("Red Object Detection", myMask);
			myOutput << myMask;
		}

	} while (myImage.data);
	*/
	
	/*
	j = FRAME_COUNT;
	if (j > myVideo.get(CV_CAP_PROP_FRAME_COUNT))
		j = (int) myVideo.get(CV_CAP_PROP_FRAME_COUNT);
	*/

	//Process each frame of the input video and save to new output file.
	j = (int) myVideo.get(CV_CAP_PROP_FRAME_COUNT);
	for (i = 0; i < j; i++)
	{
		std::cout << "frame " << i << " of " << j << std::endl;
		myVideo >> myImage;
		process_image(myImage, myMask);
		cvtColor(myMask, myCopy, CV_GRAY2BGR);
		myImage.copyTo(myLeft);
		myCopy.copyTo(myRight);
		myOutput << myStitch;
	}

	myVideo.release();

	return 0;
}

int process_image(Mat & inputImage, Mat & outputImage)
{
	Mat myCopy;
	Mat myMask[RANGES];
	Mat myStructure;

	int lowHue[RANGES], highHue[RANGES], lowSat[RANGES], highSat[RANGES], lowVal[RANGES], highVal[RANGES];
	int elem_size_x;
	int elem_size_y;

	int i;

	elem_size_x = 7;
	elem_size_y = 7;

	lowHue[0] = 165;
	highHue[0] = 179;
	lowSat[0] = 230;
	highSat[0] = 255;
	lowVal[0] = 64;
	highVal[0] = 255;

	lowHue[1] = 0;
	highHue[1] = 30;
	lowSat[1] = 230;
	highSat[1] = 255;
	lowVal[1] = 64;
	highVal[1] = 255;

	myStructure = getStructuringElement(MORPH_RECT, Size(elem_size_x, elem_size_y));
	cvtColor(inputImage, myCopy, COLOR_BGR2HSV);

	for (i = 0; i < RANGES; i++)
		inRange(myCopy, Scalar(lowHue[i], lowSat[i], lowVal[i]), Scalar(highHue[i], highSat[i], highVal[i]), myMask[i]);

	myMask[0].copyTo(outputImage);
	outputImage = Scalar(0);

	//Combine masks with logical or.
	for (i = 0; i < RANGES; i++)
		bitwise_or(outputImage, myMask[i], outputImage);
	//Perform morphological processing.
	erode(outputImage, outputImage, myStructure);
	dilate(outputImage, outputImage, myStructure);

	dilate(outputImage, outputImage, myStructure);
	erode(outputImage, outputImage, myStructure);

	return 0;
}