/* Enter your code here. Read input from STDIN. Print output to STDOUT */
#include <iostream>
#include <map>
#include <set>

using namespace std;


struct Point
{
    Point(int x, int y) : xPosition(x), yPosition(y), onLine(false), refCount(0) { }

    // This only modifies mutable data, so it can be marked as const and called during an iteration.
    bool GetStatusAndClear(void) const
    {
        bool contained = onLine || refCount > 0;
        onLine = false;
        refCount = 0;
        return contained;
    }
    
    int xPosition;
    int yPosition;
    mutable bool onLine;
    mutable int refCount;
};
    

struct PointYLess : binary_function<Point, Point, bool>
{
    bool operator()(const Point& point1, const Point& point2)
        { return point1.yPosition < point2.yPosition; }
};
    
    
typedef set<Point, PointYLess> PointsByYSet;
typedef map<int, PointsByYSet> XPositionsMap;


void registerLine(XPositionsMap& xPositions, int currX, int currY, int nextX, int nextY) {
    if (currX == nextX) {
        // line is vertical, mark all points on line permanently
        PointsByYSet& pointsByY = xPositions[currX];

        Point lookupPoint(0, min(currY, nextY));
        PointsByYSet::iterator it = pointsByY.lower_bound(lookupPoint);
        lookupPoint.yPosition = max(currY, nextY);
        PointsByYSet::iterator itEnd = pointsByY.upper_bound(lookupPoint); // upper_bound == just past max Y

        while (it != itEnd) {
            it->onLine = true;
            ++it;
        }

        return;
    }
    
    // else line is horizontal...

    int refCountAddition = currX < nextX ? 1 : -1; // decrement ref count if line moves towards X=0
    Point maxYPoint(0, 0); // outside of loop as an optimization
    
    int minX = min(currX, nextX);
    XPositionsMap::iterator xIt = xPositions.lower_bound(minX);
    int maxX = max(currX, nextX);
    XPositionsMap::iterator xItEnd = xPositions.upper_bound(maxX);
    while (xIt != xItEnd) {
        PointsByYSet& pointsByY = xIt->second;

        maxYPoint.yPosition = currY; // same as nextY because this is a horiztonal line
        PointsByYSet::iterator maxYIt = pointsByY.lower_bound(maxYPoint);

        // check to see if this point is on the line
        if (maxYIt != pointsByY.end() && maxYIt->yPosition == currY) // same as nextY
            maxYIt->onLine = true;

        // start from lowest point and stop before previously found iterator
        for (PointsByYSet::iterator yIt = pointsByY.begin(); yIt != maxYIt; ++yIt)
            yIt->refCount += refCountAddition;

        ++xIt;
    }
}


int runQuery(XPositionsMap& xPositions) {
    int numEndPoints = 0;
    cin >> numEndPoints; // guaranteed to be >= 4

    // Save initial position so we can connect back to it at the end
    int initialX = 0, initialY = 0;
    cin >> initialX >> initialY;

    int currentX = initialX, currentY = initialY;
    int nextX = 0, nextY = 0;
    for (int i = 1; i < numEndPoints; ++i) {
        cin >> nextX >> nextY;
        registerLine(xPositions, currentX, currentY, nextX, nextY);
        currentX = nextX;
        currentY = nextY;
    }
    // Register returning back to the original position
    registerLine(xPositions, currentX, currentY, initialX, initialY);

    // Iterate through all points, retrieving each's status (and clearing it for the next query)
    int pointCount = 0;
    for (XPositionsMap::iterator xIt = xPositions.begin(), xItEnd = xPositions.end(); xIt != xItEnd; ++xIt) {
        PointsByYSet& pointsByY = xIt->second;
        for (PointsByYSet::iterator yIt = pointsByY.begin(), yItEnd = pointsByY.end(); yIt != yItEnd; ++yIt) {
            if (yIt->GetStatusAndClear())
                ++pointCount;
        }
    }
    
    return pointCount;
}


int main(void) {
    int numPoints = 0, numQueries = 0;
    cin >> numPoints >> numQueries;
    
    XPositionsMap xPositions;

    int xPosition = 0, yPosition = 0;
    for (int i = 0; i < numPoints; ++i) {
        cin >> xPosition >> yPosition;
        xPositions[xPosition].insert(Point(xPosition, yPosition));
        // maybe could optimize this if we think points are specified in x-order?
    }
    
    for (int i = 0; i < numQueries; ++i)
        cout << runQuery(xPositions) << endl;
    
    return 0;
}
