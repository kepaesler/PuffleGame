#pragma once
//----------------------------------------------------------------------------------
//
// Basic Sprite Emitter Class
//
//
//  Kevin M. Smith - CS 134 SJSU

#include "Sprite.h"

typedef enum gunType { MachineGunType, FlamethrowerGunType, BoomerangGunType, SnakeGunType, OtherGun };

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public BaseObject {
public:
	Emitter(SpriteSystem *);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(ofVec3f);
	void setChildImage(ofImage);
	void setChildSize(float w, float h) { childWidth = w; childHeight = h; }
	void setImage(ofImage);
	void setRate(float);
	void setGun(Sprite *s);

	gunType gun;

	float maxDistPerFrame();
	void update();
	SpriteSystem *sys;
	float rate;
	ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	float childWidth, childHeight;

	//physics
	void integrate();
	ofVec3f *force = new ofVec3f(0,0,0);
	ofVec3f velocityEmitter;
	ofVec3f accelerationEmitter;
	float	damping;
	float   mass;
	//used for rotation
	float   radius;
	float torque;
	float angularVelocity;
	float angularAcceleration;

	//# of sprites emitted at once (ie if  =2 then 2 child sprites will be shot at same time like a shotgun effect)
	int spritesShot = 1;


	//returns new postion based on past position, velocity,  and birthTime of sprite
	std::function < ofVec3f(ofVec3f, ofVec3f, float)> move = NULL;

	ofVec3f offset;
	ofSoundPlayer   *gunSound = NULL;

	Sprite *curGun = NULL;
};
