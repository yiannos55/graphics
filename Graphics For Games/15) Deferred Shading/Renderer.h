#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Light.h"

#define LIGHTNUM 8

class Renderer : public OGLRenderer{
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void FillBuffers();			//g-buffer fill render pass
	void DrawPointLights();		// lighting render pass
	void CombineBuffers();		// combination render pass

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;

	Light* pointLights;
	Mesh* heightMap;
	OBJMesh* sphere;
	Mesh* quad;
	Camera* camera;

	float rotation;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightEmmisiveTex;
	GLuint lightSpecularTex;
};

