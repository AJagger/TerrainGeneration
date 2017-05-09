#include "stdafx.h"
#include "SimplexNoise.h"
#include <cmath>
#include <cstdint>

SimplexNoise::SimplexNoise()
{
	for(int i = 0; i < 512; i++)
	{
		p[i] = permutation[i % 256];
	}
}

SimplexNoise::~SimplexNoise()
{
}

//double SimplexNoise::simplex(double x, double y)
//{
//	double n0, n1, n2;									// Noise contributions from the three corners
//	
//	// Skew the input space to determine which simplex cell we're in
//	double F2 = 0.5*(sqrt(3.0) - 1.0);
//	double s = (x + y)*F2;							// Hairy factor for 2D
//	int i = fastfloor(x + s);
//	int j = fastfloor(y + s);
//
//	double G2 = (3.0 - sqrt(3.0)) / 6.0;
//	double t = (i + j)*G2;
//	double X0 = i - t;									// Unskew the cell origin back to (x,y) space
//	double Y0 = j - t;
//	double x0 = x - X0;								// The x,y distances from the cell origin
//	double y0 = y - Y0;
//
//	// For the 2D case, the simplex shape is an equilateral triangle.
//	// Determine which simplex we are in.
//	int i1, j1;											// Offsets for second (middle) corner of simplex in (i,j) coords
//	if (x0>y0) { i1 = 1; j1 = 0; }						// lower triangle, XY order: (0,0)->(1,0)->(1,1)
//	else { i1 = 0; j1 = 1; }							// upper triangle, YX order: (0,0)->(0,1)->(1,1)
//
//	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
//	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
//	// c = (3-sqrt(3))/6
//	double x1 = x0 - i1 + G2;							// Offsets for middle corner in (x,y) unskewed coords
//	double y1 = y0 - j1 + G2;
//	double x2 = x0 - 1.0 + 2.0 * G2;					// Offsets for last corner in (x,y) unskewed coords
//	double y2 = y0 - 1.0 + 2.0 * G2;
//
//	// Work out the hashed gradient indices of the three simplex corners
//	int ii = i & 255;
//	int jj = j & 255;
//	int gi0 = p[ii + p[jj]] % 12;
//	int gi1 = p[ii + i1 + p[jj + j1]] % 12;
//	int gi2 = p[ii + 1 + p[jj + 1]] % 12;
//
//	// Calculate the contribution from the three corners
//	double t0 = 0.5 - x0*x0 - y0*y0;
//	if (t0<0) n0 = 0.0;
//	else {
//		t0 *= t0;
//		n0 = t0 * t0 * dot(grad3[gi0], x0, y0, 0);			// (x,y) of grad3 used for 2D gradient
//	}
//	double t1 = 0.5 - x1*x1 - y1*y1;
//	if (t1<0) n1 = 0.0;
//	else {
//		t1 *= t1;
//		n1 = t1 * t1 * dot(grad3[gi1], x1, y1, 0);
//	}
//	double t2 = 0.5 - x2*x2 - y2*y2;
//	if (t2<0) n2 = 0.0;
//	else {
//		t2 *= t2;
//		n2 = t2 * t2 * dot(grad3[gi2], x2, y2, 0);
//	}
//
//	// Add contributions from each corner to get the final noise value.
//	// The result is scaled to return values in the interval [-1,1].
//	return 70.0 * (n0 + n1 + n2);
//}

/**
* Helper functions to compute gradients-dot-residual vectors (2D)
*
* @param[in] hash  hash value
* @param[in] x     x coord of the distance to the corner
* @param[in] y     y coord of the distance to the corner
*
* @return gradient value
*/
float SimplexNoise::grad(int32_t hash, float x, float y) {
	int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
	float u = h < 4 ? x : y;  // into 8 simple gradient directions,
	float v = h < 4 ? y : x;  // and compute the dot product with (x,y).
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f*v : 2.0f*v);
}

/**
* Helper function to hash an integer using the above permutation table
*
*  This inline function costs around 1ns, and is called N+1 times for a noise of N dimension.
*
*  Using a real hash function would be better to improve the "repeatability of 256" of the above permutation table,
* but fast integer Hash functions uses more time and have bad random properties.
*
* @param[in] i Integer value to hash
*
* @return 8-bits hashed value
*/
uint8_t SimplexNoise::hash(int32_t i) {
	return p[static_cast<uint8_t>(i)];
}

double SimplexNoise::simplex(double x, double y) {
	float n0, n1, n2;   // Noise contributions from the three corners

						// Skewing/Unskewing factors for 2D
	const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
	const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

									// Skew the input space to determine which simplex cell we're in
	float s = (x + y) * F2;  // Hairy factor for 2D
	float xs = x + s;
	float ys = y + s;
	int32_t i = fastfloor(xs);
	int32_t j = fastfloor(ys);

	// Unskew the cell origin back to (x,y) space
	float t = static_cast<float>(i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;
	float x0 = x - X0;  // The x,y distances from the cell origin
	float y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;
	}
	else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0;
		j1 = 1;
	}

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Calculate the contribution from the first corner
	float t0 = 0.5f - x0*x0 - y0*y0;
	if (t0 < 0.0f) {
		n0 = 0.0f;
	}
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(hash(i + hash(j)), x0, y0);
	}

	// Calculate the contribution from the second corner
	float t1 = 0.5f - x1*x1 - y1*y1;
	if (t1 < 0.0f) {
		n1 = 0.0f;
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(hash(i + i1 + hash(j + j1)), x1, y1);
	}

	// Calculate the contribution from the third corner
	float t2 = 0.5f - x2*x2 - y2*y2;
	if (t2 < 0.0f) {
		n2 = 0.0f;
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(hash(i + 1 + hash(j + 1)), x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return ((45.23065f * (n0 + n1 + n2)) + 1) / 2;
}
