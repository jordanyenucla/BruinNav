#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;

private:
	MyMap<GeoCoord, vector<StreetSegment>> m_street;
	MapLoader m_loader;
	void makeVector(GeoCoord coord, StreetSegment street);
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	for (int k = 0; k < ml.getNumSegments(); k++) {
		StreetSegment street;
		ml.getSegment(k, street); //get teh segment from the map

		makeVector(street.segment.start, street); //check to see if need ot make vector for start coordinates
		makeVector(street.segment.end, street); //check to see if need to make vector for end coordinates

		//loop thorugh all of the attractions of segment and see if need to make a vector
		for (int j = 0; j < street.attractions.size(); j++) {
			makeVector(street.attractions[j].geocoordinates, street);
		}
	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	const vector<StreetSegment>* check = m_street.find(gc); //create a pointer vector cause find returns a pointer
	if (check != nullptr) {
		return *check;
	}
	vector<StreetSegment> segments;
	return segments;  //else return empty array
}

void SegmentMapperImpl::makeVector(GeoCoord coord, StreetSegment street)
{
	vector<StreetSegment>* temp = m_street.find(coord); //create a pointer vector cause find returns a pointer

	if (temp == nullptr) {
		vector<StreetSegment> streetNames;
		streetNames.push_back(street);
		m_street.associate(coord, streetNames);
	} //if the vector is empty then add the streets inside and into the map

	//else if (m_street.size() == 0) {
	//	vector<StreetSegment> streetNames;
	//	streetNames.push_back(street);
	//	m_street.associate(coord, streetNames);
	//	return;
	//}//if my map is empty, then just push info into it

	else {
		(*temp).push_back(street); //otherwise just add to the vector
	}

}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
