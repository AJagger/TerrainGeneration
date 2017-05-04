#include "Mesh.h"
#include "../../HeightMapGenerator.h"

Mesh::Mesh(void)
{
	for (int i = 0; i < MAX_BUFFER; i++) {
		bufferObject[i] = 0;
	}
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;
}


Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;
}

void Mesh::Draw()
{
	glBindVertexArray(arrayObject);
	glDrawArrays(type, 0, numVertices);
	glBindVertexArray(0);
}

Mesh * Mesh::GenerateTriangle()
{
	Mesh*m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();

	return m;
}

Mesh * Mesh::GenerateTriangleStrip()
{
	Mesh*m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.0f, 0.0f);
	m->vertices[1] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[2] = Vector3(0.5f, 0.0f, 0.0f);
	m->vertices[3] = Vector3(0.5f, 0.5f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[3] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();

	return m;
}

//Left->Right = +Z, Bottom->Top = +X
Mesh * Mesh::GenerateChunk(float ** heightmap, int chunkZ, int chunkX)
{
	Mesh*m = new Mesh();
	m->numVertices = (HeightMapGenerator::HEIGHTMAP_SIZE*HeightMapGenerator::HEIGHTMAP_SIZE) + ((HeightMapGenerator::HEIGHTMAP_SIZE - 1) * (HeightMapGenerator::HEIGHTMAP_SIZE - 2));
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	float heighDifPercentage = 0;
	float minHeight = -100;
	float heightDif = 250;

	int vertCount = 0;
	for (int row = 0; row < (HeightMapGenerator::HEIGHTMAP_SIZE - 1); row++) {
		//Even Rows
		if ((row % 2) == 0) 
		{
			for (int col = 0; col < HeightMapGenerator::HEIGHTMAP_SIZE; col++) {

				m->vertices[vertCount] = Vector3(
					((float)col * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),	//x
					heightmap[col][row],														//y
					((float)row * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)		//z
				);
				//heighDifPercentage = (heightmap[col][row] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.6f, (1.0f - (heighDifPercentage * 0.5)),(heighDifPercentage * 0.5) ,1.0f);
				m->colours[vertCount] = DetermineColour(heightmap[col][row]);
				vertCount++;

				m->vertices[vertCount] = Vector3(
					((float)col * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),			//x
					heightmap[col][row + 1],															//y
					((float)(row + 1) * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)		//z
				);
				//heighDifPercentage = (heightmap[col][row + 1] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.6f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
				m->colours[vertCount] = DetermineColour(heightmap[col][row + 1]);
				vertCount++;
			}
		}
		//Odd Rows
		else 
		{
			for (int col = (HeightMapGenerator::HEIGHTMAP_SIZE - 1); col > 0; col--) {

				m->vertices[vertCount] = Vector3(
					((float)col * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),			//x
					heightmap[col][row + 1],															//y
					((float)(row + 1) * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)		//z
				);
				//heighDifPercentage = (heightmap[col][row + 1] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.6f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
				m->colours[vertCount] = DetermineColour(heightmap[col][row + 1]);
				vertCount++;

				m->vertices[vertCount] = Vector3(
					((float)(col - 1) * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),	//x
					heightmap[col - 1][row],														//y
					((float)row * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)			//z
				);
				//heighDifPercentage = (heightmap[col - 1][row] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.5f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
				m->colours[vertCount] = DetermineColour(heightmap[col - 1][row]);
				vertCount++;
			}
		}
	}
	m->vertices[vertCount] = Vector3(
		0.0f + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),													//x
		heightmap[HeightMapGenerator::HEIGHTMAP_SIZE - 1][HeightMapGenerator::HEIGHTMAP_SIZE - 1],								//y
		((float)(HeightMapGenerator::HEIGHTMAP_SIZE - 1) * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)	//z
	);
	//heighDifPercentage = (heightmap[HeightMapGenerator::HEIGHTMAP_SIZE - 1][HeightMapGenerator::HEIGHTMAP_SIZE - 1] - minHeight) / heightDif;
	//m->colours[vertCount] = Vector4(0.5f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
	m->colours[vertCount] = DetermineColour(heightmap[HeightMapGenerator::HEIGHTMAP_SIZE - 1][HeightMapGenerator::HEIGHTMAP_SIZE - 1]);

	m->BufferData();

	return m;
}

//Left->Right = +Z, Bottom->Top = +X
Mesh * Mesh::GenerateChunkGreyScale(float ** heightmap, int chunkZ, int chunkX)
{
	Mesh*m = new Mesh();
	m->numVertices = (HeightMapGenerator::HEIGHTMAP_SIZE*HeightMapGenerator::HEIGHTMAP_SIZE) + ((HeightMapGenerator::HEIGHTMAP_SIZE - 1) * (HeightMapGenerator::HEIGHTMAP_SIZE - 2));
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	float heighDifPercentage = 0;
	float minHeight = 0;
	float maxHeight = 0;
	float heightDifRange = 1;

	for (int row = 0; row < (HeightMapGenerator::HEIGHTMAP_SIZE - 1); row++)
	{
		for (int col = 0; col < HeightMapGenerator::HEIGHTMAP_SIZE; col++)
		{
			if(heightmap[col][row] > maxHeight)
			{
				maxHeight = heightmap[col][row];
			}
			if (heightmap[col][row] < minHeight)
			{
				minHeight = heightmap[col][row];
			}
		}
	}

	heightDifRange = maxHeight - minHeight;

	int vertCount = 0;
	for (int row = 0; row < (HeightMapGenerator::HEIGHTMAP_SIZE - 1); row++) {
		//Even Rows
		if ((row % 2) == 0)
		{
			for (int col = 0; col < HeightMapGenerator::HEIGHTMAP_SIZE; col++) {

				m->vertices[vertCount] = Vector3(
					((float)col * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),	//x
					heightmap[col][row],														//y
					((float)row * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)		//z
				);
				//heighDifPercentage = (heightmap[col][row] - minHeight) / heightDif;
				m->colours[vertCount] = DetermineColour((heightmap[col][row] - minHeight) / heightDifRange);
				//m->colours[vertCount] = DetermineColour(heightmap[col][row]);
				vertCount++;

				m->vertices[vertCount] = Vector3(
					((float)col * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),			//x
					heightmap[col][row + 1],															//y
					((float)(row + 1) * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)		//z
				);
				//heighDifPercentage = (heightmap[col][row + 1] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.6f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
				m->colours[vertCount] = DetermineColour((heightmap[col][row + 1] - minHeight) / heightDifRange);
				vertCount++;
			}
		}
		//Odd Rows
		else
		{
			for (int col = (HeightMapGenerator::HEIGHTMAP_SIZE - 1); col > 0; col--) {

				m->vertices[vertCount] = Vector3(
					((float)col * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),			//x
					heightmap[col][row + 1],															//y
					((float)(row + 1) * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)		//z
				);
				//heighDifPercentage = (heightmap[col][row + 1] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.6f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
				m->colours[vertCount] = DetermineColour((heightmap[col][row + 1] - minHeight) / heightDifRange);
				vertCount++;

				m->vertices[vertCount] = Vector3(
					((float)(col - 1) * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),	//x
					heightmap[col - 1][row],														//y
					((float)row * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)			//z
				);
				//heighDifPercentage = (heightmap[col - 1][row] - minHeight) / heightDif;
				//m->colours[vertCount] = Vector4(0.5f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
				m->colours[vertCount] = DetermineColour((heightmap[col - 1][row] - minHeight) / heightDifRange);
				vertCount++;
			}
		}
	}
	m->vertices[vertCount] = Vector3(
		0.0f + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),													//x
		heightmap[HeightMapGenerator::HEIGHTMAP_SIZE - 1][HeightMapGenerator::HEIGHTMAP_SIZE - 1],								//y
		((float)(HeightMapGenerator::HEIGHTMAP_SIZE - 1) * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)	//z
	);
	//heighDifPercentage = (heightmap[HeightMapGenerator::HEIGHTMAP_SIZE - 1][HeightMapGenerator::HEIGHTMAP_SIZE - 1] - minHeight) / heightDif;
	//m->colours[vertCount] = Vector4(0.5f, (1.0f - (heighDifPercentage * 0.5)), (heighDifPercentage * 0.5), 1.0f);
	m->colours[vertCount] = DetermineColour((heightmap[HeightMapGenerator::HEIGHTMAP_SIZE - 1][HeightMapGenerator::HEIGHTMAP_SIZE - 1] - minHeight) / heightDifRange);

	m->BufferData();

	return m;
}

Mesh * Mesh::GenerateWaterChunk(int chunkZ, int chunkX)
{
	Mesh*m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(
		(0.0f * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),
		HeightMapGenerator::WATER_LEVEL,
		(0.0f * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)
	);
	m->vertices[1] = Vector3(
		((float)HeightMapGenerator::HEIGHTMAP_SIZE * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),
		HeightMapGenerator::WATER_LEVEL,
		(0.0f * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)
	);
	m->vertices[2] = Vector3(
		(0.0f * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),
		HeightMapGenerator::WATER_LEVEL,
		((float)HeightMapGenerator::HEIGHTMAP_SIZE * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)
	);
	m->vertices[3] = Vector3(
		((float)HeightMapGenerator::HEIGHTMAP_SIZE * GRID_SIZE) + (chunkX * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET),
		HeightMapGenerator::WATER_LEVEL,
		((float)HeightMapGenerator::HEIGHTMAP_SIZE * GRID_SIZE) + (chunkZ * HeightMapGenerator::HEIGHTMAP_SIZE_OFFSET)
	);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(0.4f, 0.7f, 1.0f, 0.4f);
	m->colours[1] = Vector4(0.4f, 0.7f, 1.0f, 0.4f);
	m->colours[2] = Vector4(0.4f, 0.7f, 1.0f, 0.4f);
	m->colours[3] = Vector4(0.4f, 0.7f, 1.0f, 0.4f);

	m->BufferData();

	return m;
}

void Mesh::BufferData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (colours) {
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	
	glBindVertexArray(0);
}

Vector4 Mesh::DetermineColour(float elevation)
{
	float r, g, b = 0.0f;

	if (elevation < -10)
	{
		r = 1.0f;
		g = 1.0f;
		b = 0.10f + (elevation / -100);
	}
	else if (elevation < 150)
	{
		float heightDifPercentage = (elevation + 10) / 170;
		r = 0.5f;
		g = (1.0f - (heightDifPercentage * 0.5));
		b = (heightDifPercentage * 0.5);
	}
	else
	{
		float increase = (elevation - 150) * 0.001;
		if (increase > 0.5f) 
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}
		else 
		{
			r = 0.5f + increase;
			g = 0.5f + increase;
			b = 0.5f + increase;
		}
	}

	return Vector4(r, g, b, 1.0f);
}

//Vector4 Mesh::DetermineColour(float elevation)
//{
//	float r, g, b = 0.0f;
//
//	if (elevation < 0)
//	{
//		r = 1.0f;
//		g = 1.0f;
//		b = 1.0f;
//	}
//	else if (elevation <= 1)
//	{
//		r = 1 - elevation;
//		g = 1 - elevation;
//		b = 1 - elevation;
//	}
//	else
//	{
//		r = 0.0f;
//		g = 0.0f;
//		b = 0.0f;
//	}
//
//	return Vector4(r, g, b, 1.0f);
//}
