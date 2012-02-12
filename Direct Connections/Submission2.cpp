#include <algorithm>
#include <iostream>
#include <list>

using namespace std;


struct City
{
    int location;
    int population;
};


class Country
{
public:
    void initializeFromStdIn(void) {
        int cityCount = 0;
        cin >> cityCount;
        
        cities.resize(cityCount);
        for (list<City>::iterator it = cities.begin(), itEnd = cities.end(); it != itEnd; ++it)
            cin >> it->location;
        for (list<City>::iterator it = cities.begin(), itEnd = cities.end(); it != itEnd; ++it)
            cin >> it->population;
    }
    
    int calcCableRequired(void) {
        long long cableRequired = 0;

        for (list<City>::iterator it = cities.begin(), itEnd = cities.end(); it != itEnd; ++it) {
            list<City>::iterator itRemote = it;
            ++itRemote; // start counting distances with next city
            for ( ; itRemote != itEnd; ++itRemote) {
                // population * distance could already overflow 32bit number, so start these as 64bit
                long long higherPopulation = max(it->population, itRemote->population);
                long long distance = abs(it->location - itRemote->location);
                cableRequired += higherPopulation * distance;
                cableRequired %= 1000000007;
            }
        }

        // cableRequired has been modulo'd after every addition, so it will definitely fit in a 32bit int now
        return (int)cableRequired;
    }

private:
    list<City> cities;
};


int main(void)
{
    int countryCount = 0;
    cin >> countryCount;
    for (int i = 0; i < countryCount; ++i) {
        Country country;
        country.initializeFromStdIn();
        cout << country.calcCableRequired() << endl;
    }
    
    return 0;
}
