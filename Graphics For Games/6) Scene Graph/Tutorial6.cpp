#include "../../nclgl/window.h"
#include "Renderer.h"

#pragma comment(lib, "nclgl.lib")

int main() {	
	Window w("Scene Graphs!", 800,600,false);
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}
	int counter = 0;			// set FPS on window title
	float time_acc = 0.f;		// set FPS on window title


	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float dt = w.GetTimer()->GetTimedMS();


		renderer.UpdateScene(dt);
		renderer.RenderScene();

		// set FPS on window title
		time_acc += dt;
		counter++;

		if (time_acc >= 1000.f) {
			w.SetWindowTitle(std::to_string(counter) + "FPS");
			time_acc = 0.f;
			counter = 0;
		}
	}

	return 0;
}