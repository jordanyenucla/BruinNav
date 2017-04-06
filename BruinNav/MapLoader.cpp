#include "provided.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;

private:
	vector<StreetSegment> m_array;
	int m_numSegments;
	void parseCoords(string line, string &lat1, string &lon1, string &lat2, string &lon2);
	void parseAttraction(string line, string &name, string &lat, string &lon);
};

MapLoaderImpl::MapLoaderImpl()
{
	m_numSegments = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	if (!infile) {
		return false;  //did not load properly
	}

	string line;
	while (getline(infile, line)) {
		//getting streetname
		StreetSegment street;
		street.streetName = line;

		//getting geocords
		string lat1, lon1, lat2, lon2;
		string geoLines;
		getline(infile, geoLines);
		parseCoords(geoLines, lat1, lon1, lat2, lon2);
		street.segment = GeoSegment(GeoCoord(lat1, lon1), GeoCoord(lat2, lon2));

		//getting attraction number
		string number;
		getline(infile, number);
		int numofAttractions = stoi(number);
		if (numofAttractions > 0) {
			for (int k = 0; k < numofAttractions; k++) {
				string attractionLine;
				getline(infile, attractionLine);
				string name, lat, lon;
				parseAttraction(attractionLine, name, lat, lon);
				Attraction coolPlaces;
				coolPlaces.name = name;
				coolPlaces.geocoordinates = GeoCoord(lat, lon);
				street.attractions.push_back(coolPlaces);
			}
		}
		m_numSegments++;
		m_array.push_back(street);
	};
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_numSegments;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < 0 || segNum > getNumSegments() - 1) {
		return false;
	} //segNum is out of bounds

	seg = m_array[segNum];
	return true;
}

void MapLoaderImpl::parseCoords(string line, string &lat1, string &lon1, string &lat2, string &lon2)
{
	int counter = 0;
	//first latitude
	for (; counter < line.length(); counter++) {
		if (line[counter] != ',') {
			lat1 += line[counter];
		}
		else {
			counter++;
			break;
		}
	}
	//first longitude
	if (line[counter] == ' ') {
		counter++;
	}
	for (; counter < line.length(); counter++) {
		if (line[counter] != ' ') {
			lon1 += line[counter];
		}
		else {
			counter++;
			break;
		}
	}
	//second latitude
	for (; counter < line.length(); counter++) {
		if (line[counter] != ',') {
			lat2 += line[counter];
		}
		else {
			counter++;
			break;
		}
	}
	if (line[counter] == ' ') {
		counter++;
	} //if there is space, skip over it
	//second longitude
	for (; counter < line.length(); counter++) {
		lon2 += line[counter];
	}
}

void MapLoaderImpl::parseAttraction(string line, string & name, string & lat, string & lon)
{
	int counter = 0;
	//name of attraction
	for (; counter < line.length(); counter++) {
		if (line[counter] == '|') {
			counter++;
			break;
		}
		name += line[counter];
	}
	//latitude of attraction
	for (; counter < line.length(); counter++) {
		if (line[counter] != ',') {
			lat += line[counter];
		}
		else {
			counter++;
			break;
		}
	}
	if (line[counter] == ' ') {
		counter++;
	} //if there is space, skip over it
	for (; counter < line.length(); counter++) {
		lon += line[counter];
	}
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
