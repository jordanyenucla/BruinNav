// The main.cpp you can use for testing will replace this file soon.

#include "provided.h"
#include "MyMap.h"
#include <string>
#include <iostream>
using namespace std;

#if defined(_MSC_VER)  &&  !defined(_DEBUG)
#include <iostream>
#include <windows.h>
#include <conio.h>

struct KeepWindowOpenUntilDismissed
{
	~KeepWindowOpenUntilDismissed()
	{
		DWORD pids[1];
		if (GetConsoleProcessList(pids, 1) == 1)
		{
			std::cout << "Press any key to continue . . . ";
			_getch();
		}
	}
} keepWindowOpenUntilDismissed;
#endif

int main() {
	MapLoader map;
	Navigator lol;
	lol.loadMapData("mapdata.txt");
	NavResult wow = NAV_NO_ROUTE;
	vector<NavSegment> directions;
	wow = lol.navigate("Brentwood Country Mart", "Thalians Mental Health Center", directions);
	if (wow == NAV_SUCCESS) {
		cout << "got em" << endl;
	}
	else {
		cout << "shit" << endl;
	}
}