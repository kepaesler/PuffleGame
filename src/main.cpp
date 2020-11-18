#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	//phone size
	//ofSetupOpenGL(1334,750,OF_WINDOW);			// <-------- setup the GL context

	ofSetupOpenGL(1600,950, OF_WINDOW);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
