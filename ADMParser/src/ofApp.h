#pragma once

#include "ADMParser.h"
#include <bw64/bw64.hpp>

#include "ofMain.h"

using namespace bw64;

class ofApp : public ofBaseApp {

	ofEasyCam cam;
	vector<ADMParser::Point> points;

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

};
