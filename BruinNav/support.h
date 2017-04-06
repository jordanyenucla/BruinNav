#ifndef SUPPORT_H
#define SUPPORT_H
#include "provided.h"
#include "MyMap.h"

inline bool operator<(const GeoCoord& a, const GeoCoord& b) {
	if (a.latitudeText < b.latitudeText)
		return true;
	else if ((a.latitudeText == b.latitudeText) && (a.longitudeText < b.longitudeText))
		return true;
	return false;
}

inline bool operator>(const GeoCoord& a, const GeoCoord& b) {
	if (a.latitudeText > b.latitudeText)
		return true;
	else if ((a.latitudeText == b.latitudeText) && (a.longitudeText > b.longitudeText))
		return true;
	return false;
}

inline bool operator==(const GeoCoord& a, const GeoCoord& b) {
	if (a.latitudeText == b.latitudeText && a.longitudeText == b.longitudeText)
		return true;
	return false;
}
inline bool operator!=(const GeoCoord& a, const GeoCoord& b) {
	if (a.latitudeText != b.latitudeText || a.longitudeText != b.longitudeText)
		return true;
	return false;
}
#endif
