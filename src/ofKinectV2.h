#pragma once

#include "ofMain.h"
#include "Kinect.h"
#include "opencv2\core.hpp"

#define JOINT_COUNT JointType::JointType_Count
#define COLOR_WIDTH 1920
#define COLOR_HEIGHT 1080
#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424

using namespace cv;

class ofKinectV2 {

public:
	ofKinectV2();
	virtual ~ofKinectV2();
	void setup();
	void updateColor();
	void updateBody();
	void updateDepth();
	void drawColor(float x, float y);
	void drawBody(float x, float y);
	void drawDepth(float x, float y);
	bool getIsTraking(int count);
	float getDepthAt(float x, float y);

	ofTexture texColor;
	ofTexture texDepth;
	ofFbo fboColor;
	ofFbo fboDepth;
	ofVec2f jointPos[BODY_COUNT][JOINT_COUNT];

private:
	unsigned int cBufferSize;
	unsigned int dBufferSize;
	unsigned char *colPixBuffer;

	UINT16 *depthBuffer;
	UINT16 *depthData;
	Mat depthMat;

	IKinectSensor* pSensor;
	IDepthFrameSource* pDepthSource;
	IBodyFrameSource* pBodySource;
	IColorFrameSource* pColorSource;
	IDepthFrameReader* pDepthReader;
	IBodyFrameReader* pBodyReader;
	IColorFrameReader* pColorReader;
	ICoordinateMapper* pCoordinateMapper;
	DepthSpacePoint *depthSpace;
};