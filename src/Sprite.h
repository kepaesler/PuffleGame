#pragma once
//----------------------------------------------------------------------------------
//
// Basic Sprite Class
//
//
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"

typedef enum spriteType { MachineType, FlamethrowerType, BoomerangType, SnakeType, Other, None};

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	ofVec2f trans, scale;
	float	rot = 0;
	bool	bSelected;
	void setPosition(ofVec3f);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height;
	spriteType type;

	glm::mat4 getMatrix() {
		glm::mat4 m = glm::translate(glm::mat4(1.0), glm::vec3(trans.x, trans.y, 0));
		glm::mat4 M = glm::rotate(m, glm::radians(rot), glm::vec3(0, 0, 1));
		return M;
	}

};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void removeNear(ofVec3f point, float dist);
	spriteType isNear(ofVec3f point, float dist);
	void draw();
	vector<Sprite> sprites;

};
