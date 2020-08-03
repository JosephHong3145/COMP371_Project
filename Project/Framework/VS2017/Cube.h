#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include "shader_compile.h"

#pragma once

using namespace glm;

class Cube
{
private:
	vec3 position;
	vec3 size = vec3(1.0f, 6.5f, 1.0f);
	vec3 baseTranslation;
	vec3 basePosition;

	const vec3 centering = vec3(-(textOffset + 5.0) / 2, -3.25, 0.0);
	bool isHorizontal;
	const float textOffset = 8.0f;
	float baseRotation = 0.0f;

	mat4 customRotation;
	mat4 customTranslation;

	mat4 modelMatrix;

	float currentScaleFactor;

	Shader currentShader;

	int modelMode;

public: 
	Cube();
	Cube(vec3 basePos, bool H, Shader S);

	void setDefaultSize(vec3 s);
	void setDefaultRotation(float angle);
	void setDefaultPosition(vec3 pos);

	void update();
	void setCustomRotation(mat4 rotationMatrix);
	void setCustomTranslation(vec3 offset);
	void setCustomScaling(float scaleFactor);
	void rotateWithWorld(mat4 rotateX, mat4 rotateY);
	void setMode(int mode);

	void drawModel();
};