#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;

private:
	MapLoader m_loader;
	AttractionMapper m_attraction;
	SegmentMapper m_segment;
	string calculateDirection(double degrees) const;
	string leftOrRight(double degrees) const;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	m_loader.load(mapFile);
	m_attraction.init(m_loader);
	m_segment.init(m_loader);
	return true;  // This compiles, but may not be correct
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	MyMap<GeoCoord, GeoCoord> childToParent; //what we gonna use to backtrace later
	queue<GeoCoord> m_route; //holds our route eventually
	MyMap<GeoCoord, string> visitedStreets; //maps visited geocoords to street
	bool m_atLocation = false; //checks if we're at the location
	bool m_beenTo = false; //checks if we have visited or not
	bool firstLoop = true; //had to put this here to fix bugs

	//get geocoords for start/end location
	GeoCoord startLocation;
	GeoCoord endLocation;
	m_attraction.getGeoCoord(start, startLocation); //now we have geocoords of start
	if (&startLocation == nullptr) {
		return NAV_BAD_SOURCE;
	}
	m_attraction.getGeoCoord(end, endLocation); //now we have geocoords of end location
	if (&endLocation == nullptr) {
		return NAV_BAD_DESTINATION;
	}
	m_route.push(startLocation); //push starting point onto route
	visitedStreets.associate(startLocation, "starting place"); //we visted

	while (!m_route.empty()) {
		GeoCoord currentLocation = m_route.front();
		m_route.pop();
		//check if we have found our locatino
		if (currentLocation == endLocation) {
			m_atLocation = true;
			break;
		}
		//create a vector hold all of the streetsegments associated to the current geocoord
		vector<StreetSegment> neighbors = m_segment.getSegments(currentLocation); 
		//loop through the vector to find connecting geocoords
		for (int k = 0; k < neighbors.size(); k++) {
			if (firstLoop) {
				m_route.push(neighbors[k].segment.start);
				visitedStreets.associate(neighbors[k].segment.start, neighbors[k].streetName);
				childToParent.associate(neighbors[k].segment.start, currentLocation);
				firstLoop = false;
			}
			else if (currentLocation == neighbors[k].segment.start) {
				//check if already visited
				string *visited = visitedStreets.find(neighbors[k].segment.end);
				if (visited != nullptr) {
					m_beenTo = true;
				}
				if (!m_beenTo) {
					m_route.push(neighbors[k].segment.end); //push location onto our route
					visitedStreets.associate(neighbors[k].segment.end, neighbors[k].streetName); //mark location as visited
					childToParent.associate(neighbors[k].segment.end, currentLocation); //associate these geocoords, so it implies that they connect
				}
				else {
					m_beenTo = false;
				}
			}
			else if (currentLocation == neighbors[k].segment.end) {
				//check if already visited
				string *visited = visitedStreets.find(neighbors[k].segment.start);
				if (visited != nullptr) {
					m_beenTo = true;
				}
				if (!m_beenTo) {
					m_route.push(neighbors[k].segment.start); //push location onto our route
					visitedStreets.associate(neighbors[k].segment.start, neighbors[k].streetName); //mark location as visited
					childToParent.associate(neighbors[k].segment.start, currentLocation); //associate these geocoords, so it implies that they connect
				}
				else {
					m_beenTo = false;
				}
			}
			if (!m_beenTo && neighbors[k].attractions.size() > 0) {
				for (int j = 0; j < neighbors[k].attractions.size(); j++) {
					//check if aleady visited
					string *visited = visitedStreets.find(neighbors[k].attractions[j].geocoordinates);
					if (visited != nullptr) {
						m_beenTo = true;
					}
					if (!m_beenTo) {
						m_route.push(neighbors[k].attractions[j].geocoordinates); //push location onto our route
						//m_visited.push_back(neighbors[k].attractions[j].geocoordinates); //mark location as visited
						visitedStreets.associate(neighbors[k].attractions[j].geocoordinates, neighbors[k].attractions[j].name);
						childToParent.associate(neighbors[k].attractions[j].geocoordinates, currentLocation); //associate these geocoords, so it implies that they connect
					}
					else {
						m_beenTo = false;
					}
				}
			}
		}
	}
	if (!m_atLocation) {
		return NAV_NO_ROUTE; 
	} //could not find path

	vector<NavSegment> m_directions; //navsegment of our actual route
	string previousStreet;
	string newStreet;
	GeoCoord current = endLocation; //start with end location
	GeoCoord* parent = &current;
	bool emptyString = true;

	while (*parent != startLocation) {
		GeoSegment* line1 = new GeoSegment(current, *parent);
		current = *parent; //current should now be where parent is

		parent = childToParent.find(current);
		vector<StreetSegment> streetCoords = m_segment.getSegments(current); //get the street segments associated with the geocoord
		if (emptyString) {
			newStreet = streetCoords[0].streetName; //get the name
			previousStreet = newStreet; //there is no previous street
			emptyString = false; //now we have dealt with first case of empty strings
		}
		for (int k = 0; k < streetCoords.size(); k++) {
			if ((streetCoords[k].segment.start == current && streetCoords[k].segment.end == *parent) ||
				(streetCoords[k].segment.start == *parent && streetCoords[k].segment.end == current)) {
				newStreet = streetCoords[k].streetName;
			}//if the geocoords show up that there is a new street, keep track of it
		}
		GeoSegment* line2 = new GeoSegment(current, *parent);
		if (previousStreet != newStreet) {
			double degrees = angleBetween2Lines(*line2, *line1);
			string direction = leftOrRight(degrees);
			m_directions.push_back(NavSegment(direction, previousStreet));
			previousStreet = newStreet;
		} //if streets don't match then should tell user to turn
		else {
			double distance = distanceEarthMiles(current, *parent); //calculate distance between two geocoords 
			double degrees = angleOfLine(*line2);
			string direction = calculateDirection(degrees);
			m_directions.push_back(NavSegment(direction, newStreet, distance, GeoSegment(current, *parent)));
		} //streets do match and should tell user to proceed
	}

	vector<NavSegment> transferDirections; //create vector to give over directions
	directions = transferDirections; //clear directions cause setting it to empty vector
	for (int k = m_directions.size() - 1; k >= 0; k--) {
		directions.push_back(m_directions[k]);
	}
	return NAV_SUCCESS;
}

string NavigatorImpl::calculateDirection(double degrees) const
{
	string direction = "";
	if (degrees >= 337.5 || degrees <= 22.5) {
		direction = "west";
	}
	else if (degrees > 22.5 && degrees <= 67.5) {		
		direction = "southwest";
	}
	else if (degrees > 67.5 && degrees <= 112.5) {
		direction = "south";
	}
	else if (degrees > 112.5 && degrees <= 157.5) {
		direction = "southeast";
	}
	else if (degrees > 157.5 && degrees <= 202.5) {
		direction = "east";
	}
	else if (degrees > 202.5 && degrees <= 247.5) {
		direction = "northeast";
	}
	else if (degrees > 247.5 && degrees <= 292.5) {
		direction = "north";
	}
	else if (degrees > 292.5 && degrees <= 337.5) {
		direction = "northwest";
	}
	
	return direction;
}

string NavigatorImpl::leftOrRight(double degrees) const
{
	string direction = "";
	if (degrees >= 0 && degrees <= 180) {
		direction = "left";
	}
	else {
		direction = "right";
	}
	return direction;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
