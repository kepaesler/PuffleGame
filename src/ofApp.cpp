#include "ofApp.h"
#include "Emitter.h"
#include <random>
#include "ParticleStuff/TransformObject.h"
//----------------------------------------------------------------------------------
//
//
//
//  requires ofxGUI add-in for sliders for changing parameters
//  
//  needs ofctouch for player1 to aim otherwise default is shoot straight
//  touchosc uses port defined in header file (currently 12346)
//
//  Keanu Paesler - CS 134 SJSU


//bullet path is a sin wave
ofVec3f curveEval(ofVec3f pos, ofVec3f vel, float birthTime)
{
	float direction = vel.x;
	float scale = 5;
	float cycles = 3;
	float x = pos.x;
	float y = pos.y;
	// x is in screen coordinates and u is in [0, WindowWidth]
	float u = (cycles * x * PI) / ofGetWidth();
	return (ofVec3f(x, -scale * sin(u) + y, 0));
}

//after half a second bullet bounces back
ofVec3f boomerangCurve(ofVec3f pos, ofVec3f vel, float birthTime)
{
	float direction = vel.x / ofGetFrameRate();
	float cycles = 3;

	float x = pos.x;
	float y = pos.y;
	float u = (cycles * x * PI) / ofGetWidth();
	
	if (ofGetElapsedTimeMillis() - birthTime > 500) {
		direction = -2 * direction;
	}
	y -= sqrt(u);
	if (vel.x != direction) {
		x += direction;
	}
	return (ofVec3f(x,  y, 0));

}



//--------------------------------------------------------------
void ofApp::setup() {
	//ofSetVerticalSync(true);
	//reciver for controller setup
	ofLog() << "listening for osc messages on port " << PORT;
	receiver.setup(PORT);
	aim1 = new ofPoint(0, 0);

	won = false;

	startedGame = false;
	ofSetVerticalSync(true);
	ofBackground(235, 255, 255);

	//load background image
	//backgroundImg.loadImage("images/SpaceScaled.jpg");
	backgroundImg.loadImage("images/lightSnow.png");
	//rock.loadImage("images/rock1.png");


	rock.loadImage("images/lightRock.png");
	rock.setAnchorPercent(0.5, 0.5);
	rockPos = ofVec3f(650, 300, 0);

	rock2.loadImage("images/lightRock.png");
	rock2.setAnchorPercent(0.5, 0.5);
	rock2Pos = ofVec3f(1000, 650, 0);

	rock3.loadImage("images/rock3.png");
	rock3.setAnchorPercent(0.5, 0.5);
	rock3Pos = ofVec3f(350, 750, 0);

	rock4.loadImage("images/lightRock.png");
	rock4.setAnchorPercent(0.5, 0.5);
	rock4Pos = ofVec3f(250, 150, 0);

	box.loadImage("images/Box.png");
	box.setAnchorPercent(0.5, 0.5);
	boxPos = ofVec3f(1300, 250, 0);

	line1.loadImage("images/line1.png");
	line2.loadImage("images/line2.png");
	line3.loadImage("images/line3.png");
	line4.loadImage("images/curveLine.png");
	line5.loadImage("images/curveLine2.png");

	player1Score = 0;
	player2Score = 0;
	drawScore = false;
	playerSpeed = 10;
	squish = false;
	myFont.load("Fonts/OpenSans-Regular.ttf", 20, true, true);
	myFontBold.load("Fonts/OpenSans-Bold.ttf", 24, true, true);
	//playerForce = 300.0f;
	//player1 
	emitter = new Emitter(new SpriteSystem());
	emitter->setChildSize(10, 10);
	collider.setPosition(ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
	collider.width = 20;
	collider.height = 20;
	emitter->setVelocity(ofVec3f(-750, 0, 0));
	emitter->offset = ofVec3f(-20, 0, 0);
	emitter->setLifespan(7.0f * 1000);

	//player2
	emitter2 = new Emitter(new SpriteSystem());
	emitter2->setChildSize(10, 10);
	collider2.setPosition(ofVec3f(800, 800));
	collider2.width = 20;
	collider2.height = 20;
	emitter2->setVelocity(ofVec3f(750, 0, 0));
	emitter2->offset = ofVec3f(20, 0, 0);
	emitter2->setLifespan(7.0f * 1000);

	//physics init play1
	emitter->accelerationEmitter = ofVec3f(0, 0, 0);
	emitter->damping = .9f;
	emitter->velocityEmitter = ofVec3f(0, 0, 0);
	emitter->mass = 1.0f;
	emitter->force = new ofVec3f(0, 0, 0);

	//optional for rotational physics
	emitter->torque = 0;
	emitter->angularAcceleration = 0;
	emitter->angularVelocity = 0;
	emitter->radius = 10;

	//phyics for player2
	emitter2->accelerationEmitter = ofVec3f(0, 0, 0);
	emitter2->damping = .9f;
	emitter2->velocityEmitter = ofVec3f(0, 0, 0);
	emitter2->mass = 1.0f;
	emitter2->force = new ofVec3f(0, 0, 0);

	//optional for rotational physics
	emitter2->torque = 0;
	emitter2->angularAcceleration = 0;
	emitter2->angularVelocity = 0;
	emitter2->radius = 10;


	xForce = 3500.0f;
	yForce = 3500.0f;

	if (defaultImage.loadImage("images/BulletCircle.png")) {
		emitter->setChildImage(defaultImage);
		emitter2->setChildImage(defaultImage);
	}
	if (player1gun.loadImage("images/BlueGun.png")) {
		emitter->setImage(player1gun);
	}
	if (player2gun.loadImage("images/BlueGun.png")) {
		player2gun.mirror(false, true);
		emitter2->setImage(player2gun);
	}

	player1Img.loadImage("images/Red1TestAngry.png");
	player1Img.mirror(false, true);
	player2Img.loadImage("images/Blue1TestEyebrows2.png");
	player1.setImage(player1Img);
	player2.setImage(player2Img);

	//load animation images
	player1Squish.loadImage("images/RedSquishWide.png");
	player1Squish.mirror(false, true);
	player2Squish.loadImage("images/BlueSquishedWide.png");

	//load gun images
	//machine gun
	machineGunImg.loadImage("images/MachineGunScaled.png");
	machineGun.setImage(machineGunImg);
	machineGun.setPosition(ofVec3f(-100, -100, 0));

	//flamethrower
	flamethrowerImg.loadImage("images/flamethrower1-removebg-preview.png");
	flamethrowerImg.resize(92, 32);
	flamethrowerImg.mirror(false, true);
	flamethrower.setImage(flamethrowerImg);
	flame.loadImage("images/flame1.png");
	flame.resize(45, 45);
	flame.rotate90(1);

	//boomerang
	boomerangImg.loadImage("images/Boomerang.png");
	boomerangImg.resize(50, 50);
	boomerang.setImage(boomerangImg);

	//snake
	snakeImg.loadImage("images/snake.png");
	snakeImg.resize(31, 78);
	snakeImg.rotate90(3);
	bazooka.loadImage("images/bazooka.png");
	bazooka.resize(56, 43);
	snake.setImage(snakeImg);

	//gun emitter setup 
	gunEmitter = new Emitter(new SpriteSystem());
	gunEmitter->setVelocity(ofVec3f(0, 200, 0));
	//gunEmitter->setRate(.1f);
	gunEmitter->setRate(.25f);
	gunEmitter->offset = ofVec3f(0, 0, 0);
	gunEmitter->setPosition(ofVec3f(ofGetWindowWidth() / 2, -50, 0));
	gunEmitter->setLifespan(10 * 1000);

	//what gun gunEmitter will shoot out to players
	gunEmitter->setGun(&flamethrower);

	

	gui.setup();
	gui.add(rate.setup("rate", 3, 1, 10));
	gui.add(life.setup("life", 6, .1, 10));
	gui.add(velocity.setup("velocity", ofVec3f(750, 0, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));

	bHide = true;

	emitter->setPosition(ofVec3f(ofGetWindowWidth()*2 / 3, ofGetWindowHeight() / 2, 0));
	emitter2->setPosition(ofVec3f(ofGetWindowWidth() / 3, ofGetWindowHeight() / 2, 0));

	startMenu.loadImage("images/startMenu.png");
	startMenu.resize(ofGetWindowWidth(), ofGetWindowHeight());


	//sounds
	pistolGunSound.load("Sounds/Sound1.mp3");
	pew.load("Sounds/pewfast2.mp3");
	death.load("Sounds/pain.mp3");
	emitter->gunSound = &pistolGunSound;
	emitter2->gunSound = &pistolGunSound;
	gunEmitter-> gunSound = &pistolGunSound;

	//snow emitter set up
	snowEmitter.start();
	snowEmitter.sys->addForce(new GravityForce(ofVec3f(0, 10, 0)));
	t = new TurbulenceForce(ofVec3f(100, 0, 0), ofVec3f(-100, 0, 0));
	noise = new NoiseForce(ofVec3f(0, 0, 0));
	snowEmitter.sys->addForce(t);
	snowEmitter.sys->addForce(noise);
	snowEmitter.setRate(10.0f);
	snowEmitter.setLifespan(5.0);
	snowEmitter.setVelocity(glm::vec3(ofVec3f(10, 200, 0)));
	snowEmitter.setParticleRadius(5.0f);
	snowEmitter.setDamping(1.0f);
	tmin = ofVec3f(-10, 0, 0);
	tmax = ofVec3f(10, 0, 0);
	t->set(glm::vec3(tmin), glm::vec3(tmax));
	noise->set(glm::vec3(ofVec3f(100, 0, 0)));
	snowEmitter.setPosition(glm::vec3(500,0,0));
}

//--------------------------------------------------------------
void ofApp::update() {
	snowEmitter.update();
	snowEmitter.setPosition(snowEmitter.position + glm::vec3(50, 0, 0));
	if(snowEmitter.position.x > ofGetWindowWidth())
		snowEmitter.setPosition(glm::vec3(0, 0, 0));
	//snowEmitter2.update();

	if (!startedGame) {
		timer = ofGetElapsedTimeMillis();
		gunEmitter->lastSpawned = timer;
		return;
	}
	if (drawScore) {

		timer = ofGetElapsedTimeMillis();
		gunEmitter->lastSpawned = timer;
		return;
	}

	//randomize gun every x seconds
	if (ofGetElapsedTimeMillis() - timer > 1000) {
		//reset timer
		timer = ofGetElapsedTimeMillis();

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> dist(0, numberOfGuns-1);
		int i = dist(mt);
		if (i == 0) {
			gunEmitter->setGun(&machineGun);
			gunEmitter->setChildImage(machineGunImg);
			gunEmitter->gun = MachineGunType;
			//cout << "machine";
		}
		else if (i == 1) {
			gunEmitter->setGun(&flamethrower);
			gunEmitter->setChildImage(flamethrowerImg);
			gunEmitter->gun = FlamethrowerGunType;
			//cout << "flame";
		}
		else if (i == 2) {
			gunEmitter->setGun(&boomerang);
			gunEmitter->setChildImage(boomerangImg);
			gunEmitter->gun = BoomerangGunType;
			//cout << "boom";
		}
		else if (i == 3) {
			gunEmitter->setGun(&snake);
			gunEmitter->setChildImage(snakeImg);
			gunEmitter->gun = SnakeGunType;
			//cout << "snake";
		}
	}
	gunEmitter->update();

	//slider stuff

	//emitter->setRate(rate);
	//emitter->setLifespan(life * 1000);    // convert to milliseconds 
	//flip velocity for player1 
	glm::vec3 v = glm::vec3(velocity);
	v.x = v.x * -1;
	//emitter->setVelocity(glm::vec3(v));


	//testing move function
	//emitter->move = boomerangCurve;
	emitter->update();

	//emitter2->setRate(rate);
	//emitter2->setLifespan(life * 1000);    // convert to milliseconds 
	//emitter2->setVelocity(glm::vec3(velocity));
	emitter2->update();

	//
	checkCollisions();
	

	//if player1 is outside window stop movement
	if (emitter->trans.x >= (float)ofGetWindowWidth() - player1Img.getWidth()) {
		//emitter->trans.x -= playerSpeed;
		emitter->trans.x = (float)ofGetWindowWidth() - 50;
	}
	if (emitter->trans.x <= 0) {
		//emitter->trans.x += playerSpeed;
		emitter->trans.x = 0 + 5;
	}
	if (emitter->trans.y <= 0 + player1Img.getWidth()/2) {
		//emitter->trans.y += playerSpeed;
		emitter->trans.y = 0 + 25;
	}
	if (emitter->trans.y >= (float)ofGetWindowHeight() - player1Img.getWidth()/2) {
		//emitter->trans.y -= playerSpeed;
		emitter->trans.y = (float)ofGetWindowHeight() - 25;
	}


	//if player2 is outside window stop movement
	if (emitter2->trans.x >= (float)ofGetWindowWidth()) {
		//emitter2->trans.x -= playerSpeed;
		emitter2->trans.x = (float)ofGetWindowWidth() - 5;
	}
	if (emitter2->trans.x <= 0 + player2Img.getWidth()) {
		//emitter2->trans.x += playerSpeed;
		emitter2->trans.x = 0 + 50;
	}
	if (emitter2->trans.y <= 0 + player2Img.getWidth()/2) {
		//emitter2->trans.y += playerSpeed;
		emitter2->trans.y = 0 + 25;
	}
	if (emitter2->trans.y >= (float)ofGetWindowHeight() - player2Img.getWidth()/2) {
		//emitter2->trans.y -= playerSpeed;
		emitter2->trans.y = (float)ofGetWindowHeight() - 25;
	}
	
	//movement
	float angle1 = ((emitter->rot - 180) * PI / 180);
	float angle2 = ((emitter2->rot - 180) * PI / 180);
	//player1 movement
	if (player1Movement[0]) {
		emitter->force = &(*(emitter->force) + ofVec3f(-xForce/1.25, 0, 0));
		//emitter->trans.x -= playerSpeed;
	}
	if (player1Movement[1]) {
		emitter->force = &(*(emitter->force) + ofVec3f(0, -yForce / 1.25, 0));
		//emitter->trans.y -= playerSpeed;
	}
	if (player1Movement[2]) {
		emitter->force = &(*(emitter->force) + ofVec3f(0, yForce / 1.25, 0));
		//emitter->trans.y += playerSpeed;
	}
	if (player1Movement[3]) {
		emitter->force = &(*(emitter->force) + ofVec3f(xForce / 1.25, 0, 0));
		//emitter->trans.x += playerSpeed;
	}
	emitter->integrate();

	//player2 movement
	if (player2Movement[0]) {
		emitter2->force = &(*(emitter2->force) + ofVec3f(-xForce, 0, 0));
		//emitter2->trans.x -= playerSpeed;
	}
	if (player2Movement[1]) {
		emitter2->force = &(*(emitter2->force) + ofVec3f(0, -yForce, 0));
		//emitter2->trans.y -= playerSpeed;
	}
	if (player2Movement[2]) {
		emitter2->force = &(*(emitter2->force) + ofVec3f(0, yForce, 0));
		//emitter2->trans.y += playerSpeed;
	}
	if (player2Movement[3]) {
		emitter2->force = &(*(emitter2->force) + ofVec3f(xForce, 0, 0));
		//emitter2->trans.x += playerSpeed;
	}
	emitter2->integrate();


	//player animations
	if (ofGetFrameNum() % 15 == 0) {
		if (squish) {
			player1.setImage(player1Img);
			player2.setImage(player2Img);
			squish = false;
		}
		else {
			player1.setImage(player1Squish);
			player2.setImage(player2Squish);
			squish = true;
		}
	}


	//osc controller 
	// hide old messages
	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
		if (timers[i] < ofGetElapsedTimef()) {
			msgStrings[i] = "";
		}
	}


	// check for waiting messages
	while (receiver.hasWaitingMessages()) {

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);
		//used for testing (dont use tho because it will lower framerate)
		//cout << m.getArgAsFloat(0) << endl;
		//cout << m.getArgAsFloat(1) << endl;
		//cout << m.getAddress() << endl;
		
		xRec = m.getArgAsFloat(1);
		yRec = m.getArgAsFloat(0);
		//using move joystick
		if (m.getAddress() == moveAddress) {

			//player1 go left
			if (xRec <= 1) {
				player1Movement[0] = true;
				player1Movement[3] = false;
			}
			//player1 go up
			if (yRec <= 1) {
				player1Movement[1] = true;
				player1Movement[2] = false;
			}
			//player1 go down
			if (yRec > 1) {
				player1Movement[2] = true;
				player1Movement[1] = false;
			}
			//player1 go right
			if (xRec > 1) {
				player1Movement[3] = true;
				player1Movement[0] = false;
			}
		}
		//using aim joystick
		else if (m.getAddress() == aimAddress) {

			*aim1 = ofPoint(xRec, yRec);
			//aim1->x = xRec;
			//aim1->y = yRec;
			int dir = 1;
			ofVec3f delta = *aim1 - emitter->trans;
			if (delta.x < 0) {
				dir = -1;
			}

			//ofVec3f up = ofVec3f(0, -1, 0);

			//plus 90 is because gun image normally points left rather than up for p1
			float angle = (up.angle(delta) * dir) + 90;
			//cout << angle << endl;;
			emitter->rot = angle;

		}
		//touch messages for aim
		//input ==1 when player touches aim xy joystick
		//input ==0 when player releases aim xy joystick
		else if (m.getAddress() == aimAddress + "/z") {
			//start or stop shooting
			float input = m.getArgAsFloat(0);
			if (input == 1) {
				if (!(emitter->started))
					emitter->start();
			}
			else {
				emitter->stop();
			}

		}
		else if (m.getAddress() == moveAddress + "/z") {
			float input = m.getArgAsFloat(0);
			if (input == 0) {
				//reset/stop movement if no input
				player1Movement[0] = false;
				player1Movement[1] = false;
				player1Movement[2] = false;
				player1Movement[3] = false;
			}
		}
		else {
			cout << "error w/ recieving msg" << endl;
		}
	}

}


//--------------------------------------------------------------
void ofApp::draw(){
	
	if (!startedGame) {
		startMenu.draw(0, 0);
		snowEmitter.draw();
		ofSetColor(255, 255, 255);
		return;
	}
	else {
		snowEmitter.draw();
		ofSetColor(255, 255, 255);
	}

	//draws score board
	if (drawScore) {
		ofSetColor(0, 0, 0);
		string score = "Player 1: " + to_string(player1Score) + "   Player 2: " + to_string(player2Score);
		myFontBold.drawString(score, ofGetWindowWidth() / 2 - 175, 100);
		//ofDrawBitmapString(score, ofGetWindowWidth()/2 -50, 100);
		myFont.drawString("Press C to Continue", ofGetWindowWidth() / 2 - 125, 200);
		//ofDrawBitmapString("Press C to Continue", ofGetWindowWidth() / 2 -50, 200);
		ofSetColor(255, 255, 255);

		if (player1Score > 4) {
			ofSetColor(0, 0, 0);
			myFontBold.drawString("Player1 Wins!", ofGetWindowWidth() / 2 - 100, ofGetWindowHeight()/2);
			won = true;
			ofSetColor(255, 255, 255);
		}
		else if (player2Score > 4) {
			ofSetColor(0, 0, 0);
			myFontBold.drawString("Player2 Wins!", ofGetWindowWidth() / 2 - 100, ofGetWindowHeight() / 2);
			won = true;
			ofSetColor(255, 255, 255);
		}
	}
	//backgroundImg.draw(0, 0);
	rock.draw(rockPos.x, rockPos.y);
	rock2.draw(rock2Pos.x, rock2Pos.y);
	rock3.draw(rock3Pos.x, rock3Pos.y);
	rock4.draw(rock4Pos.x, rock4Pos.y);
	box.draw(boxPos.x, boxPos.y);
	line1.draw(700, 700);
	//line1.draw(1200, 800);
	line2.draw(300, 400);
	line3.draw(950, 200);
	line3.draw(1200, 800);
	line4.draw( 1300, 450);
	line5.draw(400, 100);
	//backgroundImg.draw(750, 750);

	//aim dots
	ofSetColor(ofColor::red);
	ofDrawCircle(ofVec3f(*aim1), 5.0f);
	ofSetColor(ofColor::cyan);
	ofDrawCircle(ofVec3f(mouseX, mouseY,0), 5.0f);
	ofSetColor(255, 255, 255);

	gunEmitter->draw();
	ofSetColor(255, 255, 255);


	//rotate all emitter sprites if boomerang
	if (emitter->curGun == &boomerang) {
		for (int i = 0; i < emitter->sys->sprites.size(); i++) {
			emitter->sys->sprites[i].rot += 5;
		}
	}
	emitter->draw();
	ofVec3f offset1 = ofVec3f(30 , 0, 0);
	ofVec3f offset2 = ofVec3f(-30, 0, 0);
	player1.trans = emitter->trans + offset1;
	player2.trans = emitter2->trans + offset2;
	player1.draw();
	player2.draw();
	//collider.draw();

	if (emitter2->curGun == &boomerang) {
		for (int i = 0; i < emitter2->sys->sprites.size(); i++) {
			emitter2->sys->sprites[i].rot += 5;
		}
	}
	emitter2->draw();
	//collider2.draw();


	if (!bHide) {
		gui.draw();
	}

	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::black);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);
	ofSetColor(255, 255, 255);
}

void ofApp::checkCollisions() {

	// remove all sprites emitted within a radius equal to the max
	// distance sprite can travel in one frame.
	//
	float dist = emitter->maxDistPerFrame();
	
	// -10 to create a smaller hitbox (hitbox is same for both players)
	dist = player2.height -10;

	spriteType collision = None;

	//player 1 hits player2
	collision = emitter->sys->isNear(player2.trans, dist);
	if ( collision != None) {
		//death animation bool
		//score update
		//show scoreboard
		//reset players and bullets
		cout << "hit player2";
		player1Score++;
		drawScore = true;
		resetPlayers();
		
		//sound
		death.play();
	}
	//player 2 hits player 1
	collision = emitter2->sys->isNear(player1.trans, dist);
	if (collision != None) {
		cout << "hit player 1";
		player2Score++;
		drawScore = true;
		resetPlayers();

		//sound
		death.play();
	}

	//check to see if bullets hit obstacles (rocks,box, etc)
	//isnear will delete the bullets if within distance
	emitter->sys->isNear(rockPos, rock.getHeight()/2  - 25);
	emitter2->sys->isNear(rockPos, rock.getHeight() / 2  -25);

	emitter->sys->isNear(rock2Pos, rock2.getHeight() / 2  -25);
	emitter2->sys->isNear(rock2Pos, rock2.getHeight() / 2);

	emitter->sys->isNear(rock3Pos, rock3.getHeight() / 2);
	emitter2->sys->isNear(rock3Pos, rock3.getHeight() / 2);

	emitter->sys->isNear(rock4Pos, rock4.getHeight() / 2 -25 );
	emitter2->sys->isNear(rock4Pos, rock4.getHeight() / 2 -25);

	emitter->sys->isNear(boxPos, box.getHeight() / 2);
	emitter2->sys->isNear(boxPos, box.getHeight() / 2);


	//make hitbox slightly bigger to pick up guns
	dist += 5;

	//player 1 gets gun
	collision = gunEmitter->sys->isNear(player1.trans, dist);
	if (collision != None) {
		
		//machine gun 
		//if (gunEmitter->curGun == &machineGun) {
		if(collision == MachineType){
			emitter->setChildImage(defaultImage);
			emitter->move = NULL;
			cout << "machinegunn";
			machineGunImg.mirror(false, true);
			emitter->setImage(machineGunImg);
			emitter->setRate(10);
			emitter->setLifespan(7.0f * 1000);

			//flip image back
			machineGunImg.mirror(false, true);
			emitter->setGun(&machineGun);
		}
		//flamethrower
		//else if (gunEmitter->curGun == &flamethrower) {
		else if (collision == FlamethrowerType) {
			emitter->move = NULL;
			cout << "flame1";
			flamethrowerImg.mirror(false, true);
			emitter->setImage(flamethrowerImg);
			emitter->setRate(25);
			emitter->setVelocity(ofVec3f(-400, 0, 0));
			emitter->setLifespan(.65f * 1000);
			flame.mirror(false, true);
			emitter->setChildImage(flame);
			//flip images back
			flamethrowerImg.mirror(false, true);
			flame.mirror(false, true);
			emitter->setGun(&flamethrower);
		}
		//boomerang
		//else if (gunEmitter->curGun == &boomerang) {
		else if (collision == BoomerangType) {
			emitter->move = boomerangCurve;
			emitter->setImage(boomerangImg);
			emitter->setGun(&boomerang);
			emitter->setVelocity(ofVec3f(-pistolXVelocity, 0, 0));
			emitter->setRate(3);
			emitter->setLifespan(7.0f * 1000);

			emitter->setChildImage(boomerangImg);
		}
		//snake
		//else if (gunEmitter->curGun == &snake) {
		else if (collision == SnakeType) {
			emitter->setVelocity(ofVec3f(-pistolXVelocity, 0, 0));
			emitter->setRate(3);
			emitter->setLifespan(7.0f * 1000);
			emitter->setImage(bazooka);
			emitter->move = curveEval;
			emitter->setGun(&snake);

			emitter->setChildImage(snakeImg);
		}
	}
	//player 2 gets gun
	collision = gunEmitter->sys->isNear(player2.trans, dist);
	if ( collision != None) {
		
		//machine gun
		//if (gunEmitter->curGun == &machineGun) {
		if (collision == MachineType) {
			emitter2->setChildImage(defaultImage);
			emitter2->move = NULL;
			cout << "machinegunn2";
			emitter2->setImage(machineGunImg);
			emitter2->setRate(10);
			emitter2->setLifespan(7.0f * 1000);
			emitter2->setGun(&machineGun);
		}
		//flamethrower
		//else if (gunEmitter->curGun == &flamethrower) {
		else if (collision == FlamethrowerType) {
			emitter2->move = NULL;
			cout << "flame";
			emitter2->setImage(flamethrowerImg);
			//
			emitter2->setChildImage(flame);
			emitter2->setVelocity(ofVec3f(400, 0, 0));
			emitter2->setRate(25);
			emitter2->setLifespan(.65f * 1000);
			emitter2->setGun(&flamethrower);
		}
		//boomerang
		//else if (gunEmitter->curGun == &boomerang) {
		else if (collision == BoomerangType) {
			emitter2->setVelocity(ofVec3f(pistolXVelocity, 0, 0));
			emitter2->setRate(3);
			emitter2->setLifespan(7.0f * 1000);
			emitter2->move = boomerangCurve;
			emitter2->setImage(boomerangImg);
			emitter2->setChildImage(boomerangImg);
			emitter2->setGun(&boomerang);
		}
		//snake
		//else if (gunEmitter->curGun == &snake) {
		else if (collision == SnakeType) {
			emitter2->setVelocity(ofVec3f(pistolXVelocity, 0, 0));
			emitter2->setRate(3);
			emitter2->setLifespan(7.0f * 1000);
			emitter2->setImage(bazooka);
			emitter2->move = curveEval;
			emitter2->setChildImage(snakeImg);
			emitter2->setGun(&snake);
		}
	}


}

//checks if two points are near eachother
bool ofApp::isNearPt(ofVec3f point1, ofVec3f point2, float dist) {
	ofVec3f v = point1 - point2;
	if (v.length() < dist) {
		return true;
	}
	else return false;
}



void ofApp::resetPlayers() {
	//remove all bullets
	emitter->sys->removeNear(player2.trans, 2000);
	emitter2->sys->removeNear(player1.trans, 2000);

	gunEmitter->sys->removeNear(player2.trans, 4000);
	//reset players pos
	emitter->setPosition(ofVec3f(ofGetWindowWidth() * 2 / 3, ofGetWindowHeight() / 2, 0));
	emitter2->setPosition(ofVec3f(ofGetWindowWidth() / 3, ofGetWindowHeight() / 2, 0));

	//reset guns!
	emitter->setChildImage(defaultImage);
	emitter2->setChildImage(defaultImage);
	emitter->setImage(player1gun);
	emitter2->setImage(player2gun);

	//player1
	emitter->setVelocity(ofVec3f(-750, 0, 0));
	emitter->offset = ofVec3f(-20, 0, 0);
	emitter->setLifespan(7.0f * 1000);
	emitter->setRate(2.0f);
	emitter->move = NULL;
	emitter->gun = OtherGun;

	//player2
	emitter2->setVelocity(ofVec3f(750, 0, 0));
	emitter2->offset = ofVec3f(20, 0, 0);
	emitter2->setLifespan(7.0f * 1000);
	emitter2->setRate(2.0f);
	emitter2->move = NULL;
	emitter2->gun = OtherGun;
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	ofPoint mouse_cur = ofPoint(x, y);
	int dir = 1;
	ofVec3f delta = mouse_cur - emitter2->trans;
	if (delta.x < 0) {
		dir = -1;
	}

	ofVec3f up = ofVec3f(0, -1, 0);

	//minus 90 is because gun image normally points right rather than up for p2
	float angle = (up.angle(delta) * dir) -90;
	//cout << angle << endl;;
	emitter2->rot = angle;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

	ofPoint mouse_cur = ofPoint(x, y);
	//ofVec3f delta = mouse_cur - mouse_last;
	int dir = 1;
	ofVec3f delta = mouse_cur - emitter2->trans;
	if (delta.x < 0) {
		dir = -1;
	}

	ofVec3f up = ofVec3f(0, -1, 0);

	//minus 90 is because gun image normally points right rather than up for p2
	float angle = (up.angle(delta) * dir) - 90;
	//cout << angle << endl;;
	emitter2->rot = angle;
	mouse_last = mouse_cur;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mouse_last = ofPoint(x, y);
	if (emitter2->started)
		return;
	emitter2->start();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	emitter2->stop();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	
	switch (key) {
	case 'a':
		//player1 go left
		player1Movement[0] = true;
		break;
	case 'w':
		//player1 go up
		player1Movement[1] = true;
		break;
	case 's':
		//player1 go down
		player1Movement[2] = true;
		break;
	case 'd':
		//player1 go right
		player1Movement[3] = true;
		break;
	case OF_KEY_LEFT:
		player2Movement[0] = true;
		break;
	case OF_KEY_RIGHT:
		player2Movement[3] = true;
		break;
	case OF_KEY_UP:
		player2Movement[1] = true;
		break;
	case OF_KEY_DOWN:
		player2Movement[2] = true;
		break;
	case ' ':
		if (!startedGame) {
			startedGame = true;
			gunEmitter->start();
			return;
		}
		if (emitter->started)
			return;
		emitter->start();
		break;
	case 'C':
	case 'c':
		drawScore = false;
		if (won) {
			player1Score = 0;
			player2Score = 0;
			won = false;
		}
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'r':
		break;
	case 'u':
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		emitter->stop();
		break;
	case 'a':
		//player1 stop going left
		player1Movement[0] = false;
		break;
	case 'w':
		//player1 stop go up
		player1Movement[1] = false;
		break;
	case 's':
		//player1 stop go down
		player1Movement[2] = false;
		break;
	case 'd':
		//player1 stop go right
		player1Movement[3] = false;
		break;
	case OF_KEY_LEFT:
		player2Movement[0] = false;
		break;
	case OF_KEY_RIGHT:
		player2Movement[3] = false;
		break;
	case OF_KEY_UP:
		player2Movement[1] = false;
		break;
	case OF_KEY_DOWN:
		player2Movement[2] = false;
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	}
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

