#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"


#define SHADOWSIZE 2048

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
	void DrawMoon();
	void DrawKnights();

	void DrawShadowScene();
	void DrawCombinedScene();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* skeletonShader;



	Shader* shadowShader;
	Shader* sceneShader;



	HeightMap* heightMap;
	Mesh* quad;

	OBJMesh* sun;
	OBJMesh* moon;

	Light* light;
	Camera* camera;

	GLuint cubeMap;
	GLuint rocks;
	
	MD5FileData* hellData;
	MD5Node* hellNode;

	GLuint shadowTex;
	GLuint shadowFBO;

	float waterRotate;
};

