#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;

private:
	MyMap<string, GeoCoord> m_attraction;
	MapLoader m_loader;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	StreetSegment holder;
	for (unsigned int i = 0; i < ml.getNumSegments(); i++) {
		ml.getSegment(i, holder);
		if (holder.attractions.size() > 0) {
			for (size_t a = 0; a < holder.attractions.size(); a++) {
				string loweringname = holder.attractions[a].name;
				for (int r = 0; r < loweringname.size(); r++)
					loweringname[r] = tolower(loweringname[r]);
				m_attraction.associate(loweringname, holder.attractions[a].geocoordinates);
			}
		}
	}
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	for (int k = 0; k < attraction.size(); k++) {
		attraction[k] = tolower(attraction[k]);
	}
	const GeoCoord* check = m_attraction.find(attraction);
	if (check == nullptr) {
		return false;
	} //if it doesn't return a geocord then function shoudl return false
	gc = *check;
	return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
