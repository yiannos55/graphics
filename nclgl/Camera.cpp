#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/

float lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

void Camera :: setStartPos() {
	cameraPositions.push_back(Vector3(2009.0f, 1509.0f, 14653.0f));
	cameraPositions.push_back(Vector3(14161.0f, 1726.0f, 15727.0f));
	cameraPositions.push_back(Vector3(11493.0f, 3281.0f, -815.0f));
}

void Camera::setPitchandYaw() {
		cameraPitchandYaw.push_back(Vector2(-23.0f, 320.0f));
		cameraPitchandYaw.push_back(Vector2(-23.0f, 45.f));
		cameraPitchandYaw.push_back(Vector2(-28.0f, 156.3f));
}

bool autoCam = false;

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
	
		position = Vector3(lerp(this->GetPosition().x, cameraPositions[i].x, 0.03f),
							lerp(this->GetPosition().y, cameraPositions[i].y, 0.03f),
							lerp(this->GetPosition().z, cameraPositions[i].z, 0.03f));

		this->SetPitch(lerp(this->GetPitch(), cameraPitchandYaw[i].x, 0.03f));
		this->SetYaw(lerp(this->GetYaw(), cameraPitchandYaw[i].y, 0.03f));

		if (tempo>=3.0f) {
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
