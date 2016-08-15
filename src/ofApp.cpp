#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	kinect.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
	kinect.updateColor();
	kinect.updateBody();
	kinect.updateDepth();
	// return 0.0f ~ 8000.0f
	cout << "Depth at(" << ofGetMouseX() << ", " << ofGetMouseY() << ") = " << kinect.getDepthAt(ofGetMouseX(), ofGetMouseY()) << endl;
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
	kinect.drawColor(0, 0);
	kinect.drawBody(0, 0);
	kinect.drawDepth(0, 0);
	for (int count = 0; count < BODY_COUNT; count++) {
		if (kinect.getIsTraking(count)) {
			string str = ofToString(count) + " is tracking!!";
			ofSetColor(255, 0, 0);
			ofDrawBitmapString(str, 10, 30);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
