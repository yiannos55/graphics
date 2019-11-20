#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/

float lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

vector<Vector3> cameraPositions;
//vector<Vector3> endPos;

void Camera :: setStartPos() {
	cameraPositions.push_back(Vector3(-2394.0f, 4470.0f, 20075.0f));
	cameraPositions.push_back(Vector3(13323.0f, 1268.0f, 14475.0f));
	cameraPositions.push_back(Vector3(12645.0f, 2419.0f, 998.0f));
}

bool autoCam = true;

void Camera::UpdateCamera(float msec)	{
	//Update the mouse by how much
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw	  -= (Window::GetMouse()->GetRelativePosition().x);
	
	
	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch,90.0f);
	pitch = max(pitch,-90.0f);

	if(yaw <0) {
		yaw+= 360.0f;
	}
	if(yaw > 360.0f) {
		yaw -= 360.0f;
	}

	tempo += 0.01f;
	//msec *= 5.0f;
	//msec = 1;
	
	if (autoCam) {
	
		//position = Vector3(	lerp(tmpPosition.x ,cameraPositions[i].x, tempo),
		//						lerp(tmpPosition.y, cameraPositions[i].y, tempo),
		//						lerp(tmpPosition.z, cameraPositions[i].z, tempo));


		position = Vector3(lerp(this->GetPosition().x, cameraPositions[i].x, 0.03f),
							lerp(this->GetPosition().y, cameraPositions[i].y, 0.03f),
							lerp(this->GetPosition().z, cameraPositions[i].z, 0.03f));


		if (tempo>=1.0f) {
			i++;
			tempo = 0.0f;
		}
		if (i >= cameraPositions.size()) {
			i = 0;
		}
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(0,0,-1) * msec;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(0,0,-1) * msec;
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position += Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(-1,0,0) * msec;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(-1,0,0) * msec;
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += msec;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= msec;
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_E)) {
		autoCam = !autoCam;
		if (autoCam)
		{
			tmpPosition = position;
		}
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * 
			Matrix4::Rotation(-yaw, Vector3(0,1,0)) * 
			Matrix4::Translation(-position);
};
