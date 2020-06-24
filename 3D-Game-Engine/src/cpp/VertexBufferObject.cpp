#include "VertexBufferObject.h"

namespace VertexManager {
	const float TrianglePositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
	};

	GLuint initVertexBuffer()
	{
		GLuint vBufferHandle;
		glGenBuffers(1, &vBufferHandle);

		glBindBuffer(GL_ARRAY_BUFFER, vBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TrianglePositions), TrianglePositions, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return vBufferHandle;
	}

	GLuint initVertexArray()
	{
		GLuint vArrayHandle;
		glGenVertexArrays(1, &vArrayHandle);
		glBindVertexArray(vArrayHandle);

		glBindVertexArray(0);
		return vArrayHandle;
	}
}
