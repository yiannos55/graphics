#include "Camera.h"
#include "..//Graphics For Games/Graphics Coursework/Renderer.h"
/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/

float lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

void Camera :: setStartPos() {
	cameraPositions.push_back(Vector3(-8734.0f, 1818.0f, 10133.0f));
	cameraPositions.push_back(Vector3(3874.94f, 1850.72f, -6194.06f));
	cameraPositions.push_back(Vector3(10719.9f, 2984.84f, 2232.68f));
	cameraPositions.push_back(Vector3(2662.0f, 1700.53f, 5776.80f));
	cameraPositions.push_back(Vector3(-6986.85f, 799.906f, 5743.31f));
	cameraPositions.push_back(Vector3(-5571.55f, 599.719f, 4341.31f));
	cameraPositions.push_back(Vector3(-9255.1f, 1633.59f, 1318.73f));
	cameraPositions.push_back(Vector3(-632.335f, 6153.78f, -12104.1f));
}

void Camera::setPitchandYaw() {
	cameraPitchandYaw.push_back(Vector2(-17.8f, 331.0f));
	cameraPitchandYaw.push_back(Vector2(-5.39f, 178.488f));
	cameraPitchandYaw.push_back(Vector2(-18.55f, 86.79f));
	cameraPitchandYaw.push_back(Vector2(-19.25f, 58.018f));
	cameraPitchandYaw.push_back(Vector2(-14.14f, 300.417f));
	cameraPitchandYaw.push_back(Vector2(-12.74, 267.517f));
	cameraPitchandYaw.push_back(Vector2(-16.8f, 282.847f));
	cameraPitchandYaw.push_back(Vector2(-30.17f, 183.726f));
}



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
