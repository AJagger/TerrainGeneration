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

#include "stdafx.h"
#include "DataArray.h"

template<class T> DataArray<T>::DataArray()
{
	Initialise();
}

template<class T> DataArray<T>::~DataArray()
{
	//delete data;
	freeList.clear();
}

template<class T> void DataArray<T>::Initialise()
{
	//Set Next ID and data count to 0.
	nextId = 0;
	count = 0;

	//Set lastLookup to -1 so that DataArray::Next() returns the first element.
	lastLookup = -1;

	//Register all locations within the data array as free in the free list
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		freeList.push_back(i);
	}
}

template<class T> T* DataArray<T>::CreateNew()
{
	if (!freeList.empty())
	{
		int newLocation = freeList.front();
		data[newLocation].id = nextId;

		freeList.pop_front();
		nextId++;
		count++;

		return &data[newLocation].object;
	}
	else
	{
		return nullptr;
	}
}

template<class T> void DataArray<T>::Free(int id)
{
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (data[i].id == id)
		{
			freeList.push_front(i);
			data[i].id = -1;
			count--;
		}
	}
}

template<class T> bool DataArray<T>::Exists(int id)
{
	bool found = false;
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (data[i].id == id)
		{
			found = true;
			break;
		}
	}

	return found;
}

//Returns a pointer to the object or, if the object does not exist, returns a nullptr.
template<class T> T * DataArray<T>::TryToGet(int id)
{
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (data[i].id == id)
		{
			lastLookup = i;
			return &data[i].object;
		}
	}
	return nullptr;
}

//Looks up the ID for a returned object. If the object no longer exists then returns -1.
template<class T> int DataArray<T>::GetId(T & object)
{
	int id = -1;
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (&data[i].object == &object)
		{
			id = i;
			break;
		}
	}

	return id;
}

template<class T> T * DataArray<T>::TryToGetFirst()
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(data[i].id !=-1)
		{
			lastLookup = i;
			return &data[i].object;
		}
	}

	return nullptr;
}

template<class T> bool DataArray<T>::IsNext()
{
	bool found = false;
	for (int i = lastLookup + 1; i < MAX_OBJECTS; i++)
	{
		if (data[i].id != -1)
		{
			found = true;
			break;
		}
	}
	return found;
}

//Allows for iteration over the dataArray without knowing individual object Ids.
template<class T> T * DataArray<T>::Next()
{
	for (int i = lastLookup+1; i < MAX_OBJECTS; i++)
	{
		if (data[i].id != -1)
		{
			lastLookup = i;
			return &data[i].object;
		}
	}
	return nullptr;
}

//Returns the number of unexpired objects within the data structure
template<class T> int DataArray<T>::Count()
{
	return count;
}

template<class T> void DataArray<T>::FreeAll()
{
	for (int i = (MAX_OBJECTS - 1); i >= 0; i--)
	{
		if (data[i].id != -1)
		{
			data[i].id = -1;
			freeList.push_front(i);
		}
	}

	lastLookup = -1;
	nextId = 0;
	count = 0;
}
