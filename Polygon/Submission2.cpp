/* Enter your code here. Read input from STDIN. Print output to STDOUT */
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

using namespace std;


struct Point
{
    Point(int xIn = 0, int yIn = 0) : x(xIn), y(yIn) { }
    int x;
    int y;
};


typedef set<int> IntSet;
typedef map<int, IntSet> IntSetsMap;


struct Polygon
{
    typedef map<int, bool> MaxOrMinMap;

    Polygon(int minXIn, int maxXIn, int minYIn, int maxYIn) :
        minX(minXIn), maxX(maxXIn), minY(minYIn), maxY(maxYIn), vectorOffset(minXIn),
        yPointsByX(maxXIn - minXIn + 1) // number of buckets in the vector (might be huge! *gulp*)
    { }
    
    void registerLine(const Point& prevPt, const Point& startPt, const Point& endPt, const Point& nextPt)
    {
        if (startPt.y == endPt.y) { // horizontal line
            int minX = min(startPt.x, endPt.x);
            int maxX = max(startPt.x, endPt.x);
            // ignore the endpoints, those are taken care of with vertical lines
            recordXRange(minX + 1, maxX - 1, startPt.y, startPt.x < endPt.x);
            return;
        }
        
        // vertical line, this is where the bulk of the work is

        bool increasingY = startPt.y < endPt.y;
        bool enteredIncreasingX = prevPt.x < startPt.x;
        bool exitedIncreasingX = endPt.x < nextPt.x;
        
        MaxOrMinMap& maxOrMins = yPointsByX[startPt.x - vectorOffset];
        if (increasingY) {
            if (!enteredIncreasingX)
                maxOrMins[startPt.y] = false; // hard minimum
            if (exitedIncreasingX)
                maxOrMins[endPt.y] = true; // hard maximum
        }
        else { // decreasingY
            if (enteredIncreasingX)
                maxOrMins[startPt.y] = true; // hard maximum
            if (!exitedIncreasingX)
                maxOrMins[endPt.y] = false; // hard minimum
        }
    }
    
    int countPointsIncluded(int xCoord, const IntSet& yCoords) const
    {
        int pointsIncluded = 0;
        const MaxOrMinMap& maxOrMins = yPointsByX[xCoord - vectorOffset];
        if (maxOrMins.size() % 2 != 0)
            throw new length_error("max and min are not properly paired");
        
        // since yPointsByX covers only the valid range of X, there is guaranteed at least one pair of
        // min/max points in each MaxOrMinMap.
        IntSet::const_iterator yIt = yCoords.begin();
        IntSet::const_iterator yItEnd = yCoords.end();
        for (MaxOrMinMap::const_iterator itMin = maxOrMins.begin(), itEnd = maxOrMins.end(); itMin != itEnd; ) {
            // if everything worked out correctly, it should point to a minimum, and it + 1 a maximum
            MaxOrMinMap::const_iterator itMax = itMin;
            ++itMax; // move to max item of the pair
            if (itMin->second || !itMax->second)
                throw new range_error("min/max pair are corrupted");

            // it's unclear if iterating up to range or searching for lower_bound is more performant here.
            // it would probably depend on the data input.
            
            // these exist below our min
            while (yIt != yItEnd && *yIt < itMin->first)
                ++yIt;
            // these exists at-or-above our min and at-or-below our max
            while (yIt != yItEnd && *yIt <= itMax->first) {
                ++pointsIncluded;
                ++yIt;
            }
            
            // move forward to next min/max pair
            ++itMin;
            ++itMin;
        }
        
        // if anything is left, it is above our highest max, so ignore
        
        return pointsIncluded;
    }

    void recordXRange(int lowerX, int higherX, int y, bool maxOrMin)
    {
        int firstVectorSlot = lowerX - vectorOffset;
        int lastVectorSlot = firstVectorSlot + (higherX - lowerX);
        for (int i = firstVectorSlot; i <= lastVectorSlot; ++i)
            yPointsByX[i][y] = maxOrMin;
    }
    
    void print(void) const
    {
        cout << "vectorOffset = " << vectorOffset << endl;
        for (vector<MaxOrMinMap>::const_iterator xIt = yPointsByX.begin(); xIt != yPointsByX.end(); ++xIt) {
            const MaxOrMinMap& maxOrMins = *xIt;
            for (MaxOrMinMap::const_iterator yIt = maxOrMins.begin(); yIt != maxOrMins.end(); ++yIt)
                cout << yIt->first << ":" << (yIt->second ? "max" : "min") << " - ";
            cout << endl;
        }
    }
    
    int minX;
    int maxX;
    int minY;
    int maxY;

    int vectorOffset;
    vector<MaxOrMinMap> yPointsByX;
};


int runQuery(const IntSetsMap& intSets) {
    int numEndPoints = 0;
    cin >> numEndPoints; // guaranteed to be >= 4

    deque<Point> endPoints(numEndPoints);
    deque<Point>::iterator it = endPoints.begin();
    cin >> it->x >> it->y;
    int minX = it->x, minY = it->y;
    int maxX = minX, maxY = minY;
    for (++it; it != endPoints.end(); ++it) {
        cin >> it->x >> it->y;
        minX = min(minX, it->x);
        maxX = max(maxX, it->x);
        minY = min(minY, it->y);
        maxY = max(maxY, it->y);
    }

    Point lastPoint = endPoints.back(); // guaranteed to exist, so copy it before /real/ last gets obscured
    
    it = endPoints.begin();
    Point firstPoint = *it;
    ++it;
    Point secondPoint = *it;
    // copy first and second points to end of list so last line segment knows what direction it ends going
    endPoints.push_back(firstPoint);
    endPoints.push_back(secondPoint);
    
    // copy last point to beginning of list so first line segment knows what direction is started from
    endPoints.push_front(lastPoint);

    Polygon polygon(minX, maxX, minY, maxY);
    for (int i = 0; i < numEndPoints; ++i) // same as the number of line segments
        polygon.registerLine(endPoints[i], endPoints[i+1], endPoints[i+2], endPoints[i+3]);
    
    int pointsIncluded = 0;
    for (IntSetsMap::const_iterator it = intSets.begin(), itEnd = intSets.end(); it != itEnd; ++it)
        if (it->first >= minX && it->first <= maxX)
            pointsIncluded += polygon.countPointsIncluded(it->first, it->second);
    
    return pointsIncluded;
}


int main(void) {
    int numPoints = 0, numQueries = 0;
    cin >> numPoints >> numQueries;
    
    IntSetsMap intSets;

    int xPosition = 0, yPosition = 0;
    for (int i = 0; i < numPoints; ++i) {
        cin >> xPosition >> yPosition;
        intSets[xPosition].insert(yPosition);
        // maybe could optimize this if we think points are specified in x-order?
    }
    
    for (int i = 0; i < numQueries; ++i)
        cout << runQuery(intSets) << endl;
    
    return 0;
}
