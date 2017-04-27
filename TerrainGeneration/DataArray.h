/* CSC3224 Code
* Author: Aidan Jagger | 130281034
* Class Description:
* The DataArray class is built to store game assets such as GameObjects and Meshes. It is designed for quick lookup and importantly for minimal overhead when
* creating and deleting objects.
* The Data is stored in a fixed size array of 1000 rather than a vector which saves on memory allocation operations when dealing with fluctuating numbers of objects being
* stored. It uses a freelist to store which locations in the array are available for new data to be stored in and when creating a "new" object in the DataArray,
* it uses this freelist to find a location which can be used or overwritten in the main array. New objects are given a unique id which is stored in a struct alongside
* the data allowing for quick lookup.
* Objects are not deleted, rather, their location is marked as free.
*
* This class is used exstensively throughout the engine.
*/

#pragma once
#include <deque>

template<class T>
class DataArray
{
	struct DataElement
	{
		T object;
		int id = -1;
	};

public:
	DataArray();
	~DataArray();

	T* CreateNew();
	void Free(int id);

	bool Exists(int id);
	T* TryToGet(int id);
	int GetId(T &object);
	T* TryToGetFirst();
	bool IsNext();
	T* Next();
	int Count();
	void FreeAll();

private:
	static const int MAX_OBJECTS = 1000;
	int count;
	int nextId;
	int lastLookup;

	DataElement data[MAX_OBJECTS];
	std::deque<int> freeList;

	void Initialise();
};
