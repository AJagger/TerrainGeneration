#pragma once
class HMCoordinate
{
public:
	int x, y;

	HMCoordinate(const int x, const int y) {
		this->x = x;
		this->y = y;
	};
	~HMCoordinate();
};

