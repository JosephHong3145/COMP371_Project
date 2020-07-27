#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include "shader_compile.h"
using namespace glm;
#pragma once


class Sphere
{
	private:
		vec3 position;
		float radius = 7.5f;
		vec3 baseTranslation;
		vec3 basePosition;

		const vec3 centering = vec3(-(textOffset + 5.0) / 2, -3.25, 0.0);
		const float textOffset = 8.0f;
		float baseRotation = 0.0f;

		mat4 customRotation;
		mat4 customTranslation;

		mat4 modelMatrix;

		float currentScaleFactor;

		float polyCount;

		Shader currentShader;

	public:
		Sphere();
		Sphere(vec3 basePos, float rad, Shader S);

		void setDefaultRadius(float rad);
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

