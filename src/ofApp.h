#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Sprite.h"
#include "ofxOsc.h"

#include "ParticleStuff/Particle.h"
#include "ParticleStuff/ParticleEmitter.h"
#include "ParticleStuff/TransformObject.h"

#define PORT 12346

// max number of strings to display
#define NUM_MSG_STRINGS 20


typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;



class Emitter;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void checkCollisions();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void resetPlayers();
		bool isNearPt(ofVec3f point1, ofVec3f point2, float dist);
		
		Emitter *emitter; 
		Sprite collider;

		Emitter *emitter2;
		Sprite collider2;

		Emitter *gunEmitter;

		ofImage defaultImage;
		ofVec3f mouse_last;

		Sprite player1;
		Sprite player2;
		ofImage player1Img;
		ofImage player2Img;
		ofImage player1Squish;
		ofImage player2Squish;
		bool squish;

		ofImage player1gun;
		ofImage player2gun;

		int player1Score;
		int player2Score;
		bool drawScore;
		ofTrueTypeFont myFont;
		ofTrueTypeFont myFontBold;

		bool bHide;

		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxVec3Slider velocity;
		ofxLabel screenSize;

		ofxPanel gui;
		int playerSpeed;
		bool player1Movement[4] = { false, false, false, false };
		bool player2Movement[4] = { false, false, false, false };

		bool startedGame; 
		ofImage startMenu;

		//background images
		ofImage rock;
		ofVec3f rockPos;

		ofImage rock2;
		ofVec3f rock2Pos;
		ofImage backgroundImg;
		ofImage box;
		ofVec3f boxPos;
		ofImage rock3;
		ofVec3f rock3Pos;
		ofImage rock4;
		ofVec3f rock4Pos;

		ofImage line1;
		ofImage line2;
		ofImage line3;
		ofImage line4;
		ofImage line5;

		//background snow particle emitter
		ofxVec3Slider tmin;
		ofxVec3Slider tmax;
		ParticleEmitter snowEmitter;
		TurbulenceForce *t;

		ofxVec3Slider noiseSlider;
		NoiseForce *noise;

		//other guns
		Sprite machineGun;
		ofImage machineGunImg;

		Sprite flamethrower;
		ofImage flamethrowerImg;
		ofImage flame;

		Sprite boomerang;
		ofImage boomerangImg;

		Sprite snake;
		ofImage bazooka;
		ofImage snakeImg;

		//sounds
		ofSoundPlayer   pistolGunSound;
		ofSoundPlayer	machingeGunSound;
		ofSoundPlayer	flamethrowerSound;
		ofSoundPlayer	boomerangSound;
		ofSoundPlayer	pew;
		ofSoundPlayer	death;
		// maybe?
		//ofSoundPlayer     curveGunSound;

		//used for choosing randomly between guns. If more guns are added change this number
		int numberOfGuns = 4;

		//preset gun values
		int pistolXVelocity = 750;

		//uint64_t timer = 0;
		float timer = 0;

		//player force value
		//float playerForce;
		float xForce;
		float yForce;


		//osc controller stuff
		ofTrueTypeFont font;
		ofxOscReceiver receiver;

		int currentMsgString;
		string msgStrings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

		float xRec = 0;
		float yRec = 0;

		//aim point for player 1
		ofPoint *aim1;

		ofVec3f up = ofVec3f(0, -1, 0);


		string moveAddress = "/1/xy1";
		string aimAddress = "/1/xy2";

		bool won;
		
};
