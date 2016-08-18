#include "ofKinectV2.h"

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

ofKinectV2::ofKinectV2() {

}

void ofKinectV2::setup() {
	colPixBuffer = new unsigned char[COLOR_WIDTH * COLOR_HEIGHT * 4];
	depthBuffer = new UINT16[DEPTH_WIDTH * DEPTH_HEIGHT];
	depthData = new UINT16[COLOR_WIDTH * COLOR_HEIGHT];
	depthSpace = new DepthSpacePoint[COLOR_WIDTH * COLOR_HEIGHT];
	cBufferSize = COLOR_WIDTH * COLOR_HEIGHT * 4 * sizeof(unsigned char);
	dBufferSize = DEPTH_WIDTH * DEPTH_HEIGHT * 4 * sizeof(unsigned short);
	texColor.allocate(COLOR_WIDTH, COLOR_HEIGHT, GL_RGBA);
	texDepth.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, GL_LUMINANCE);
	fboColor.allocate(COLOR_WIDTH, COLOR_HEIGHT);
	fboDepth.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);
	for (int count = 0; count < BODY_COUNT; count++) {
		for (int type = 0; type < JOINT_COUNT; type++) {
			jointPos[count][type] = ofVec2f(-100);
		}
	}


	HRESULT hr = S_OK;
	// Sensor
	hr = GetDefaultKinectSensor(&pSensor);
	if (FAILED(hr)) {
		cerr << "Error : GetDefaultKinectSensor" << endl;
		return;
	}
	hr = pSensor->Open();
	if (FAILED(hr)) {
		cerr << "Error : IKinectSensor::Open()" << endl;
		return;
	}

	// Sorce
	hr = pSensor->get_DepthFrameSource(&pDepthSource);
	if (FAILED(hr)) {
		cerr << "Error : IKinectSensor::get_BodyFrameSource()" << endl;
		return;
	}
	hr = pSensor->get_BodyFrameSource(&pBodySource);
	if (FAILED(hr)) {
		cerr << "Error : IKinectSensor::get_BodyFrameSource()" << endl;
		return;
	}
	hr = pSensor->get_ColorFrameSource(&pColorSource);
	if (FAILED(hr)) {
		cerr << "Error : IKinectSensor::get_BodyFrameSource()" << endl;
		return;
	}

	// Reader
	hr = pDepthSource->OpenReader(&pDepthReader);
	if (FAILED(hr)) {
		cerr << "Error : IBodyFrameSource::OpenReader()" << endl;
		return;
	}
	hr = pBodySource->OpenReader(&pBodyReader);
	if (FAILED(hr)) {
		cerr << "Error : IBodyFrameSource::OpenReader()" << endl;
		return;
	}
	hr = pColorSource->OpenReader(&pColorReader);
	if (FAILED(hr)) {
		cerr << "Error : IBodyFrameSource::OpenReader()" << endl;
		return;
	}

	// Coordinate Mapper
	hr = pSensor->get_CoordinateMapper(&pCoordinateMapper);
	if (FAILED(hr)) {
		cerr << "Error : IKinectSensor::get_CoordinateMapper()" << endl;
		return;
	}
}

void ofKinectV2::updateColor() {
	if (!pColorReader) {
		return;
	}

	HRESULT hr = S_OK;

	// Color Frame
	IColorFrame* pColorFrame = nullptr;
	hr = pColorReader->AcquireLatestFrame(&pColorFrame);
	if (SUCCEEDED(hr)) {
		hr = pColorFrame->CopyConvertedFrameDataToArray(cBufferSize, colPixBuffer, ColorImageFormat_Rgba);
		if (SUCCEEDED(hr)) {
			texColor.loadData(colPixBuffer, COLOR_WIDTH, COLOR_HEIGHT, GL_RGBA);
			fboColor.begin();
			ofSetColor(255);
			texColor.draw(0, 0);
			fboColor.end();
		}
	}
	SafeRelease(pColorFrame);
}

void ofKinectV2::updateBody() {
	if (!pBodyReader) {
		return;
	}

	HRESULT hr = S_OK;

	// Body Frame
	IBodyFrame* pBodyFrame = nullptr;
	hr = pBodyReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr)) {
		IBody* pBody[BODY_COUNT] = { 0 };
		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody);

		if (SUCCEEDED(hr)) {
			for (int count = 0; count < BODY_COUNT; count++) {
				BOOLEAN bTraked = false;
				hr = pBody[count]->get_IsTracked(&bTraked);

				if (SUCCEEDED(hr) && bTraked) {
					Joint joint[JOINT_COUNT];
					hr = pBody[count]->GetJoints(JOINT_COUNT, joint);
					// joint
					for (int type = 0; type < JOINT_COUNT; type++) {
						ColorSpacePoint colorSpacePoint = { 0 };
						pCoordinateMapper->MapCameraPointToColorSpace(joint[type].Position, &colorSpacePoint);
						int x = static_cast<int>(colorSpacePoint.X);
						int y = static_cast<int>(colorSpacePoint.Y);
						if (x >= 0 && x < COLOR_WIDTH && y >= 0 && y < COLOR_HEIGHT) {
							jointPos[count][type] = ofVec2f(x, y);
						}
					}
				}
				else {
					for (int type = 0; type < JOINT_COUNT; type++) {
						jointPos[count][type] = ofVec2f(-100);
					}
				}
			}
		}
		for (int count = 0; count < BODY_COUNT; count++) {
			SafeRelease(pBody[count]);
		}
	}
	SafeRelease(pBodyFrame);
}

void ofKinectV2::updateDepth() {
	if (!pDepthReader) {
		return;
	}

	HRESULT hr = S_OK;

	// Depth Frame
	Mat bufferMat(DEPTH_HEIGHT, DEPTH_WIDTH, CV_16UC1);
	IDepthFrame* pDepthFrame = nullptr;
	hr = pDepthReader->AcquireLatestFrame(&pDepthFrame);
	if (SUCCEEDED(hr)) {
		pDepthFrame->CopyFrameDataToArray(DEPTH_HEIGHT * DEPTH_WIDTH, &depthBuffer[0]);
		pCoordinateMapper->MapColorFrameToDepthSpace(DEPTH_HEIGHT * DEPTH_WIDTH, &depthBuffer[0], COLOR_WIDTH * COLOR_HEIGHT, &depthSpace[0]);
		hr = pDepthFrame->AccessUnderlyingBuffer(&dBufferSize, reinterpret_cast<UINT16**>(&bufferMat.data));
		if (SUCCEEDED(hr)) {
			bufferMat.convertTo(depthMat, CV_8U, -255.0f / 8000.0f, 255.0f);
			texDepth.loadData(depthMat.ptr(), depthMat.cols, depthMat.rows, GL_LUMINANCE);
			fboDepth.begin();
			ofSetColor(255);
			texDepth.draw(0, 0);
			fboDepth.end();
		}
		SafeRelease(pDepthFrame);
		depthMat.release();
	}
}

void ofKinectV2::drawDepth(float x, float y) {
	fboDepth.draw(x, y);
}

void ofKinectV2::drawColor(float x, float y) {
	fboColor.draw(x, y);
}

void ofKinectV2::drawBody(float x, float y) {
	ofPushMatrix();
	ofTranslate(x, y);
	for (int count = 0; count < BODY_COUNT; count++) {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString(count, jointPos[count][3].x, jointPos[count][3].y - 20);
		for (int type = 0; type < JOINT_COUNT; type++) {
			ofSetColor(255, 100, 0);
			ofDrawBitmapString(type, jointPos[count][type].x + 15, jointPos[count][type].y);
			ofSetColor(255, 150, 0);
			ofCircle(jointPos[count][type], 10);
		}
	}
	ofPopMatrix();
	ofSetColor(255);
}

bool ofKinectV2::getIsTraking(int count) {
	if (jointPos[count][0] != ofVec2f(-100)) {
		return true;
	}
	else {
		return false;
	}
}

float ofKinectV2::getDepthAt(float x, float y) {
	unsigned int colorIndex = (int)x + COLOR_WIDTH * (int)y;
	int depthX = static_cast<int>(depthSpace[colorIndex].X + 0.5f);
	int depthY = static_cast<int>(depthSpace[colorIndex].Y + 0.5f);

	if ((0 <= depthX) && (depthX < DEPTH_WIDTH) && (0 <= depthY) && (depthY < DEPTH_HEIGHT)) {
		unsigned int depthIndex = depthX + depthY * DEPTH_WIDTH;
		depthData[colorIndex] = depthBuffer[depthIndex];
		//return ofMap(static_cast<float>(depthData[colorIndex]), 0.0f, 8000.0f, 0.0f, 255.0f);
		return static_cast<float>(depthData[colorIndex]);
	}
	else {
		return 0;
	}
}

ofKinectV2::~ofKinectV2() {

}
