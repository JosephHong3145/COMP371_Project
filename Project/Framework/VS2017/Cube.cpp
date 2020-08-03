#include "Cube.h"
Cube::Cube() {

}

Cube::Cube(vec3 basePos, bool H, Shader S) {
	//setting base scaling, translation and rotation
	isHorizontal = H;
	currentShader = S;
	basePosition = basePos;
	this->update();
}

//update the cube 
void Cube::update() {
	if (isHorizontal == true) {
		//horizontal bar
		size.x = 5.0f;
		size.y = 1.0f;
	}

	modelMatrix = rotate(mat4(1.0f), radians(baseRotation), vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), size * currentScaleFactor);

	modelMatrix = translate(mat4(1.0f), (basePosition + centering) * currentScaleFactor) * modelMatrix;

	//setting custom rotation
	modelMatrix = customRotation * modelMatrix;

	//setting custom translation 
	modelMatrix = customTranslation * modelMatrix;
}

void Cube::setDefaultSize(vec3 s) {
	size = s;
	this->update();
}

void Cube::setDefaultRotation(float angle) {
	baseRotation = angle;
	this->update();
}

void Cube::setDefaultPosition(vec3 position) {
	basePosition = position;
	this->update();
}

void Cube::setCustomScaling(float scaleFactor) {
	currentScaleFactor = scaleFactor;
}

void Cube::setCustomRotation(mat4 rotationMatrix) {
	customRotation = rotationMatrix;
}

void Cube::setCustomTranslation(vec3 offSet) {
	customTranslation = translate(mat4(1.0f), offSet);
}

void Cube::setMode(int mode) {
	modelMode = mode;
}

void Cube::drawModel() {
	currentShader.setMat4("worldMatrix", modelMatrix);
	switch (modelMode) {
	case 0:
		glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
		break;
	case 1:
		glDrawArrays(GL_LINES, 0, 36);
		break;
	case 2:
		glDrawArrays(GL_POINTS, 0, 36);
		break;
	default:
		glDrawArrays(GL_TRIANGLES, 0, 36);
		break;
	}
}

void Cube::setCurrentShader(Shader S) {
	currentShader = S;
	this->update();
}

void Cube::rotateWithWorld(mat4 rotateX, mat4 rotateY) {
	modelMatrix = rotateY * rotateX * modelMatrix;
}