#include "ofApp.h"

#define USE_XML

//--------------------------------------------------------------
void ofApp::setup() {
	// parse
	ADMParser admParser;

#ifdef USE_XML
	admParser.ParseFile(ofToDataPath("FN_Trailer_Mix_V2_ADM.xml", true).c_str(), audioTracks);
#else
	std::unique_ptr<Bw64Reader> bw64File = readFile(ofToDataPath("Strike Up the Band v10.wav", true).c_str());
	Bw64Reader* ptr = bw64File.get();

	if (bw64File->axmlChunk()) {
		std::stringstream axmlStringstream;
		bw64File->axmlChunk()->write(axmlStringstream);
		std::cout << axmlStringstream.str();

		admParser.ParseString(axmlStringstream.str().c_str(), audioTracks);
	}
	else {
		std::cerr << "could not find an axml chunk";

	}
#endif

	// output yaml file
	std::ofstream out("output.atmos");
	out << "sampleRate: 48000" << std::endl;
	for (auto & audioTrack : audioTracks) {
		out << "  - ID: " << audioTrack.first << std::endl;
		std::vector<ADMParser::KeyPoint> points = audioTrack.second;
		for (size_t i = 0; i < points.size(); i++) {
			if (i == 0) {
				out << "    ";
			}
			else {
				out << "  - ";
			}

			out << "samplePos: " << (long long int)(points[i].time * 48000) << std::endl;
			out << "    pos: [" << points[i].y << ", " << points[i].z << ", " << points[i].x << "]" << std::endl;
		}
	}
	out.close();
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

	float time = ofGetElapsedTimef();

	for (auto & audioTrack : audioTracks) {
		std::vector<ADMParser::KeyPoint> points = audioTrack.second;
		int idx = 0;
		for (size_t i = 0; i < points.size(); i++) {
			if (points[i].time <= time) idx = i;
		}

		ofPushMatrix();
		ofTranslate(points[idx].x, points[idx].y, points[idx].z);
		ofSetColor(0, 0, 255);
		ofDrawSphere(0, 0, 0, 0.02);
		ofPopMatrix();
	}
	cam.end();
	ofDisableDepthTest();
	ofPopStyle();

	for (auto & audioTrack : audioTracks) {
		std::vector<ADMParser::KeyPoint> points = audioTrack.second;
		int idx = 0;
		for (size_t i = 0; i < points.size(); i++) {
			if (points[i].time <= time) idx = i;
		}

		ofDrawBitmapString(audioTrack.first, cam.worldToScreen(ofVec3f(points[idx].x, points[idx].y, points[idx].z)) + ofVec3f(5, 10, 0));
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
