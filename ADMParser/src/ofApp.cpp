#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup() {
	// parse
	ADMParser admParser;
	//admParser.ParseFile(ofToDataPath("all-params-ADM.txt", true).c_str(), points);

	std::unique_ptr<Bw64Reader> bw64File = readFile(ofToDataPath("Strike Up the Band v10.wav", true).c_str());
	Bw64Reader* ptr = bw64File.get();

	if (bw64File->axmlChunk()) {
		std::stringstream axmlStringstream;
		bw64File->axmlChunk()->write(axmlStringstream);
		std::cout << axmlStringstream.str();

		admParser.ParseString(axmlStringstream.str().c_str(), points);
	}
	else {
		std::cerr << "could not find an axml chunk";
		
	}
}


//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0);
	ofSetColor(255);


	// draw
	cam.setNearClip(0.001);
	cam.setFarClip(10);
	cam.setDistance(3);
	cam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));

	ofPushStyle();
	ofEnableDepthTest();
	cam.begin();
	ofDrawAxis(0.25);
	ofNoFill();
	ofSetColor(100);
	ofDrawBox(2); // [-1; 1]
	ofFill();
	for (auto point : points) {
		ofPushMatrix();
		ofTranslate(point.x, point.y, point.z);
		ofSetColor(0, 0, 255);
		ofDrawSphere(0, 0, 0, 0.02);
		ofPopMatrix();
	}
	cam.end();
	ofDisableDepthTest();
	ofPopStyle();

	for (auto point : points) {
		ofDrawBitmapString(point.audioTrack, cam.worldToScreen(ofVec3f(point.x, point.y, point.z)) + ofVec3f(5, 10, 0));
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
