


#include "ofMain.h"
#include "Emitter.h"

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;

	lastSpawned = 0;
	rate = 2;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	childWidth = 10;
	childHeight = 10;

	gun = OtherGun;
	//testing sound
	//gunSound.load("Sounds/Sound1.mp3");

}


//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {
	if (drawable) {
		ofPushMatrix();
		if (haveImage) {
			ofTranslate(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
			ofRotateDeg(rot);
			ofPushMatrix();
			image.draw(0, 0);
			ofPopMatrix();
			//image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
		}
		else {
			ofSetColor(0, 0, 255);
			ofDrawRectangle(-width / 2 + trans.x, -height / 2 + trans.y, width, height);
		}
		ofPopMatrix();
	}

	// draw sprite system
	//
	sys->draw();
}

void rotate(const vector<glm::vec3> &vin, vector<glm::vec3> &vout, float angle) {
	for (int i = 0; i < vin.size(); i++) {
		vout.push_back(glm::vec3(vin[i].x * cos(angle) - vin[i].y *sin(angle),
			vin[i].x * sin(angle) + vin[i].y *cos(angle), 0));
	}
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	
	//adjust velocity based on rotation
	ofVec3f velocityAdjusted = velocity.getRotated(rot, ofVec3f(0, 0, 1));
	
	sys->update();
	if (move) {
		//cout << "moving";
		for (int i = 0; i < sys->sprites.size(); i++) {
			ofVec3f sVel = sys->sprites[i].velocity;
			sys->sprites[i].trans = move(sys->sprites[i].trans, sVel, sys->sprites[i].birthtime);
			//sys->sprites[i].trans += ofVec3f(0, 10, 0);
		}
			//s->trans = move(s->trans);
			//s++;
	}

	if (!started) return;

	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0 / rate)) {
		// spawn a new sprite
		Sprite sprite;
		if (curGun != NULL) {
			//sprite = *curGun;
			//set sprite enum
			//if(curGun)
			if (gun == MachineGunType) {
				sprite.type = MachineType;
			}
			else if (gun == FlamethrowerGunType) {
				sprite.type = FlamethrowerType;
			}
			else if (gun == BoomerangGunType) {
				sprite.type = BoomerangType;
			}
			else if (gun == SnakeGunType) {
				sprite.type = SnakeType;
			}
			else if (gun == OtherGun) {
				sprite.type = Other;
			}
		}
		else {
			sprite.type = Other;
			//gunSound->play();
			//if (haveChildImage) sprite.setImage(childImage);
		}
		gunSound->play();
		if (haveChildImage) sprite.setImage(childImage);

		sprite.velocity = velocityAdjusted;
		sprite.lifespan = lifespan;
		sprite.setPosition(trans + offset);
		sprite.birthtime = time;
		sprite.width = childWidth;
		sprite.height = childHeight;
		sys->add(sprite);
		lastSpawned = time;
	}


	//sys->update();
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
	childWidth = img.getWidth();
	childHeight = img.getHeight();
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}

float Emitter::maxDistPerFrame() {
	return  velocity.length() / ofGetFrameRate();
}

void Emitter::setGun(Sprite *s) {
	curGun = s;
	//setChildImage(s->image);
}

void Emitter::integrate() {

	// (1) update position from velocity and time interval
	// (2) update velocity (based on acceleration
	// (3) multiply final result by the damping factor to sim drag
	//
	float deltaTime = 1 / ofGetFrameRate();

	//trans.z + (velocity.z * deltaTime)
	trans = glm::vec3(trans.x + (velocityEmitter.x * deltaTime), trans.y + (velocityEmitter.y * deltaTime), 0 );

	//update rotation
	rot += angularVelocity * deltaTime;
	//update angular acceleration
	float i = mass * (2 * radius) / 18.0f; //moment of inertia for triangle
	angularAcceleration = torque / i;
	//update angular velocity
	angularVelocity += angularAcceleration * deltaTime;
	//dampen
	angularVelocity = angularVelocity * damping;
	//set torque to 0
	torque = 0;

	//update acceleration
	accelerationEmitter = (1 / mass) * (*force);
	//update velocity
	velocityEmitter += ofVec3f(accelerationEmitter.x * deltaTime, accelerationEmitter.y * deltaTime, accelerationEmitter.z * deltaTime);
	//dampen
	velocityEmitter = velocityEmitter * damping;

	//set force to 0
	force = new ofVec3f(0, 0, 0);

}
