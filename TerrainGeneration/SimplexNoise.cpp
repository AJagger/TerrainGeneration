#include "stdafx.h"
#include "SimplexNoise.h"
#include <cmath>

SimplexNoise::SimplexNoise()
{
	repeat = -1;
	for(int i = 0; i < 512; i++)
	{
		p[i] = permutation[i % 256];
	}
}

SimplexNoise::~SimplexNoise()
{
}

double SimplexNoise::simplex(double x, double y, double z)
{
	//int unitX = (int)floor(x) & 255;                // FIND UNIT SQUARE THAT
	//int unitY = (int)floor(y) & 255;                 // CONTAINS POINT.

	//int relX = x - floor(x);                                // FIND RELATIVE X,Y,Z
	//int relY = y - floor(y);                                // OF POINT IN Square.

	//double fadeX = fade(x);                             // COMPUTE FADE CURVES
	//double fadeY = fade(y);                             // FOR EACH OF X,Y,Z.

	////Hash coordinates of 4 square corners
	//int AA = p[p[unitX] + unitY],
	//	AB = p[p[unitX] + (unitY + 1)],
	//	BA = p[p[(unitX + 1)] + unitY],
	//	BB = p[p[(unitX + 1)] + (unitY + 1)];


	//return lerp(fadeY, lerp(fadeX, grad(AA, relX, relY), grad(BA, (relX - 1), relY)), lerp(fadeX, grad(AB, relX, (relY - 1)), grad(BB, (relX -1), (relY-1))));

	////return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),  // AND ADD
	////	grad(p[BA], x - 1, y, z)), // BLENDED
	////	lerp(u, grad(p[AB], x, y - 1, z),  // RESULTS
	////		grad(p[BB], x - 1, y - 1, z))),// FROM  8
	////	lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),  // CORNERS
	////		grad(p[BA + 1], x - 1, y, z - 1)), // OF CUBE
	////		lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
	////			grad(p[BB + 1], x - 1, y - 1, z - 1))));

	//if (repeat > 0) {									// If we have any repeat on, change the coordinates to their "local" repetitions
	//	x = x % (double)repeat;
	//	y = y % repeat;
	//	z = z%repeat;
	//}

	int xi = (int)x & 255;								// Calculate the "unit cube" that the point asked will be located in
	int yi = (int)y & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	int zi = (int)z & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	double xf = x - (int)x;								// We also fade the location to smooth the result.
	double yf = y - (int)y;
	double zf = z - (int)z;
	double u = fade(xf);
	double v = fade(yf);
	double w = fade(zf);

	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = p[p[p[xi] + yi] + zi];
	aba = p[p[p[xi] + inc(yi)] + zi];
	aab = p[p[p[xi] + yi] + inc(zi)];
	abb = p[p[p[xi] + inc(yi)] + inc(zi)];
	baa = p[p[p[inc(xi)] + yi] + zi];
	bba = p[p[p[inc(xi)] + inc(yi)] + zi];
	bab = p[p[p[inc(xi)] + yi] + inc(zi)];
	bbb = p[p[p[inc(xi)] + inc(yi)] + inc(zi)];

	double x1, x2, y1, y2;
	x1 = lerp(grad(aaa, xf, yf, zf),				// The gradient function calculates the dot product between a pseudorandom
		grad(baa, xf - 1, yf, zf),				// gradient vector and the vector from the input coordinate to the 8
		u);										// surrounding points in its unit cube.
	x2 = lerp(grad(aba, xf, yf - 1, zf),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
		grad(bba, xf - 1, yf - 1, zf),				// values we made earlier.
		u);
	y1 = lerp(x1, x2, v);

	x1 = lerp(grad(aab, xf, yf, zf - 1),
		grad(bab, xf - 1, yf, zf - 1),
		u);
	x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
		grad(bbb, xf - 1, yf - 1, zf - 1),
		u);
	y2 = lerp(x1, x2, v);

	return (lerp(y1, y2, w) + 1) / 2;						// For convenience we bound it to 0 - 1 (theoretical min/max before is -1 - 1)
}
