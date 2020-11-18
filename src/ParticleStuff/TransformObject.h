#pragma once
#include "ofMain.h"

//  Kevin M. Smith - CS 134 SJSU
//

//  Base class for any object that needs a transform.
//
class TransformObject {
public:
	TransformObject();
	glm::vec3 position, scale;
	float	rotation;
	bool	bSelected;
	void setPosition(const glm::vec3 &);
};