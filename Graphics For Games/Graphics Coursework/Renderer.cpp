#include "Renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	camera = new Camera(-29.17f, 184.726f,
		Vector3(-8735.0f, 1817.0f, 10132.0f));

	heightMap = new HeightMap(TEXTUREDIR"myTerrain.raw");
	water = Mesh::GenerateQuad();
	quad = Mesh::GenerateQuad();


	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR"lightvertex.glsl", SHADERDIR"PerPixelFragment.glsl");
	skeletonShader = new Shader(SHADERDIR"hellVert.glsl", SHADERDIR"hellfrag.glsl");

	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");

	postProcessShader = new Shader(SHADERDIR"TexturedVertexPost.glsl", SHADERDIR"TexturedFragment.glsl");
	
	processShader = new Shader(SHADERDIR "TexturedVertexPost.glsl", SHADERDIR "processfrag.glsl");
	processShaderEdge = new Shader(SHADERDIR "TexturedVertexPost.glsl", SHADERDIR "sobelFrag.glsl");

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2md5anim");

	light = new Light(Vector3(1, 15000.0f, 0), Vector4(1, 1, 1, 1), (RAW_WIDTH * RAW_WIDTH));


	sun = new OBJMesh();
	sun->LoadOBJMesh(MESHDIR"sphere.obj");


	if (!reflectShader->LinkProgram()	|| !skyboxShader->LinkProgram() || !lightShader->LinkProgram()	 || !skeletonShader->LinkProgram()		|| 
		!shadowShader->LinkProgram()	|| !sceneShader->LinkProgram()	|| !processShader->LinkProgram() || !postProcessShader->LinkProgram()	|| !processShaderEdge->LinkProgram()) {
		return;
	}

	water->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	rocks = SOIL_load_OGL_texture(TEXTUREDIR"snow.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	heightMap->SetTexture2(rocks);


	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"mystic_ft.png", TEXTUREDIR"mystic_bk.png",
		TEXTUREDIR"mystic_up.png", TEXTUREDIR"mystic_dn.png",
		TEXTUREDIR"mystic_rt.png", TEXTUREDIR"mystic_lf.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	if (!cubeMap || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture2(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(water->GetTexture(), true);

	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(100.0f, 150000.0f, (float)width / (float)height, 45.0f);

	/////////////////////////// post process
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO); //render the scene into this
	glGenFramebuffers(1, &processFBO); //do post processing in this

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);

	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////end post process


	//////////////// shadows
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;

}

Renderer::~Renderer(void) {

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);

	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &rocks);

	delete quad;

	delete processShader;
	delete postProcessShader;

	delete camera;
	delete heightMap;
	delete light;
	delete water;
	delete lightShader;
	delete reflectShader;
	delete skyboxShader;
	delete skeletonShader;
	delete processShaderEdge;
	delete sun;

	delete hellData;
	delete hellNode;
	

	delete sceneShader;
	delete shadowShader;

	currentShader = 0;
}
void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_R)) {
		lightrot = !lightrot;
		if (lightrot) {
			rotateLight(msec);
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_B)) {
		blur = !blur;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
		split = !split;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_J)) {
		edge = !edge;
	}
}

void Renderer::rotateLight(float msec) {
	Vector3 lightPos = light->GetPosition();
	Matrix4 rot = Matrix4::Rotation(msec / 40, Vector3(1, 0, 0));
	lightPos = rot * lightPos;
	light->SetPosition(lightPos);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (time <= 1.0f) {
		time += 0.002f;
	}

	DrawShadowScene();
	DrawCombinedScene();
	if (edge) {
		edgeDetection();
		PresentScene(bufferColourTex[1]);
	}
	if (blur) {
		DrawPostProcessBlurr();
		PresentScene(bufferColourTex[0]);
	}
	if (split&&!blur) {
		splitScreen();
	}
	SwapBuffers();
	glViewport(0, 0, width, height);
}

/////////////////->>split Screen
void Renderer::splitScreen() {

	projMatrix = Matrix4::Perspective(400.0f, 100000.0f, ((float)width) / ((float)height), 70.0f);
	glViewport(0, 0, width / 2, height);
	DrawCombinedScene();

	projMatrix = Matrix4::Perspective(400.0f, 100000.0f, ((float)width) / ((float)height), 70.0f);
	glViewport(width / 2, 0, width / 2, height);
	DrawCombinedScene();
}
///////////////////////////split Screen end 


void Renderer::edgeDetection() {
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(processShaderEdge);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);

	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

//////////////////////////post process blur
void Renderer::DrawPostProcessBlurr() {
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[0]);
		quad->Draw();

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

		quad->SetTexture(bufferColourTex[1]);
		quad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene(GLuint tex) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(postProcessShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();
	quad->SetTexture(tex);
	quad->Draw();
	glUseProgram(0);
}
////////////////////////// end of blur effect 

///////////////////// shadow casting-->>>
void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(6800, 21400, 1, 85);
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();

	DrawWater();
	glDisable(GL_CULL_FACE);
	DrawHeightmap();
	glEnable(GL_CULL_FACE);
	DrawKnights();

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(100.0f, 150000.0f, (float)width / (float)height, 45.0f);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
////////////////////////end of shadows



///////////////////////combine scene----->
void Renderer::DrawCombinedScene() {
	if (blur||edge){
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		SetCurrentShader(sceneShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

		SetShaderLight(*light);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowTex);

		viewMatrix = camera->BuildViewMatrix();
		UpdateShaderMatrices();

		glDisable(GL_CULL_FACE);
		DrawSkybox();
		glEnable(GL_CULL_FACE);
		DrawWater();
		DrawHeightmap();
		DrawKnights();
		DrawSun();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		SetCurrentShader(sceneShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

		SetShaderLight(*light);

		glActiveTexture(GL_TEXTURE2);

		glBindTexture(GL_TEXTURE_2D, shadowTex);
		viewMatrix = camera->BuildViewMatrix();
		UpdateShaderMatrices();
		
		glDisable(GL_CULL_FACE);
		DrawSkybox();
		glEnable(GL_CULL_FACE);
		DrawWater();
		DrawHeightmap();
		DrawKnights();
		DrawSun();

		glUseProgram(0);
	}
}
//////////////////// end of combining scene 

void Renderer::DrawKnights() {
	SetCurrentShader(skeletonShader);
	SetShaderLight(*light);
	UpdateShaderMatrices();

	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	tempMatrix.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			modelMatrix = Matrix4::Translation(Vector3(3500.0f + (x * 125)-RAW_WIDTH * HEIGHTMAP_X / 2.0f, 220.0f, 12500.0f + (y * 125)-RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)) *
			(hellNode->GetTransform() * Matrix4::Scale(hellNode->GetModelScale())) *
			Matrix4::Rotation(20 * x + 10 * y + 10, Vector3(0, 1, 0));

			UpdateShaderMatrices();
			hellNode->Draw(*this);
		}
	}
}

void Renderer::DrawSun() {
	SetCurrentShader(skeletonShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	modelMatrix = Matrix4::Translation(Vector3(light->GetPosition())) *
		Matrix4::Scale(Vector3(300, 300, 300));
	UpdateShaderMatrices();

	sun->Draw();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	water->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "rockTex"), 3);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
	glUseProgram(0);
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix = Matrix4::Translation(Vector3(0, heightY - 260, 0)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();

	water->Draw();

	glUseProgram(0);
}

