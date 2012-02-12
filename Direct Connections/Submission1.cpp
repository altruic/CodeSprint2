/* Enter your code here. Read input from STDIN. Print output to STDOUT */
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;


struct City
{
    City(void) : location(0), population(0) { }
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
        for (int i = 0; i < cityCount; ++i) {
            int location = 0;
            cin >> location;
            cities[i].location = location;
        }
        for (int i = 0; i < cityCount; ++i) {
            int population = 0;
            cin >> population;
            cities[i].population = population;
        }
    }
    
    int calcCableRequired(void) {
        long long cableRequired = 0;
        int cityCount = cities.size();
        for (int i = 0; i < cityCount; ++i) {
            City& localCity = cities[i];
            for (int j = i + 1; j < cityCount; ++j) {
                City& remoteCity = cities[j];
                // population * distance could already overflow 32bit number, so start these as 64bit
                long long higherPopulation = max(localCity.population, remoteCity.population);
                long long distance = abs(localCity.location - remoteCity.location);
                cableRequired += higherPopulation * distance;
                cableRequired %= 1000000007;
            }
        }
        // cableRequired has been modulo'd after every addition, so it will definitely fit in a 32bit int now
        return (int)cableRequired;
    }

private:
    vector<City> cities;
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
