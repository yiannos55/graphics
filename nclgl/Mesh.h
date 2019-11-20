#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER
};
class Mesh {
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	void SetTexture(GLuint tex) { texture = tex; }
	void SetTexture2(GLuint tex) { texture2 = tex; }
	GLuint GetTexture() { return texture; }
	GLuint GetTexture2() { return texture2; }

	void SetBumpMap(GLuint tex) { bumpTexure = tex; }
	GLuint GetBumpMap() { return bumpTexure; }

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

protected:
	void BufferData();
	void GenerateNormals();
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b,
		const Vector3& c, const Vector2& ta,
		const Vector2& tb, const Vector2& tc);


	GLuint texture;
	GLuint texture2;
	Vector2* textureCoords;

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	GLuint numIndices;
	unsigned int* indices;

	Vector3* vertices;
	Vector4* colours;
	Vector3* normals;
	Vector3* tangents;
	GLuint bumpTexure;
	Vector3 position;

};