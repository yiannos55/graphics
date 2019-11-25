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
	//void DrawMoon();
	void DrawKnights();

	void DrawShadowScene();
	void DrawCombinedScene();
	///////////post process
	Mesh* quad;
	Shader* processShader;
	void DrawPostProcessBlurr();

	void DrawPostProcessEdge();
	
	void PresentScene(GLuint tex);
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	
	GLuint splitFBO;

	//////////////

	void rotateLight(float msec);

	bool lightrot = true;
	bool blur=false;
	bool split = false;
	bool edge = false;

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* skeletonShader;
	Shader* postProcessShader;
	Shader* processShaderEdge;


	Shader* shadowShader;
	Shader* sceneShader;



	HeightMap* heightMap;
	Mesh* water;

	OBJMesh* sun;

	//OBJMesh* moon;

	Light* light;
	Camera* camera;

	GLuint cubeMap;
	GLuint rocks;
	
	MD5FileData* hellData;
	MD5Node* hellNode;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint splitTex;


	float waterRotate;
};

