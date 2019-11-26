#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"


#define SHADOWSIZE 4096
#define POST_PASSES 10

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawSun();
	void DrawKnights();
	void DrawShadowScene();
	void DrawCombinedScene();
	void splitScreen();
	void DrawPostProcessBlurr();
	void edgeDetection();
	void PresentScene(GLuint tex);
	void rotateLight(float msec);

	Mesh* quad;
	Shader* processShader;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* skeletonShader;
	Shader* postProcessShader;
	Shader* processShaderEdge;
	Shader* shadowShader;
	Shader* sceneShader;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint shadowFBO;

	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	GLuint shadowTex;

	bool lightrot = true;
	bool blur=false;
	bool split = false;
	bool edge = false;
	float time = 0.0f;
	float waterRotate;

	HeightMap* heightMap;
	Mesh* water;

	OBJMesh* sun;

	Light* light;
	Camera* camera;

	GLuint cubeMap;
	GLuint rocks;
	
	MD5FileData* hellData;
	MD5Node* hellNode;
};

