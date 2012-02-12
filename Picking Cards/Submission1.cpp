/* Enter your code here. Read input from STDIN. Print output to STDOUT */
#include <iostream>
#include <map>

using namespace std;


long long multiplyByPartialFactorialAndMod(long long runningTotal, int factorialBase, int iterations)
{
    // I don't need to support negatives here.
    if (runningTotal <= 0 || factorialBase <= 0)
        return runningTotal;
    
    static const long long moduloNumber = 1000000007;

    for (int i = 0; i < iterations && factorialBase > 0; ++i) {
        runningTotal *= factorialBase;
        runningTotal %= moduloNumber;
        --factorialBase;
    }
    
    return runningTotal;
}


int performTestRun(void)
{
    int numCards = 0;
    cin >> numCards;
    
    map<int, int> valueCounts;
    int cardValue = 0;
    for (int i = 0; i < numCards; ++i) {
        cin >> cardValue;
        ++valueCounts[cardValue];
    }
    
    int countSoFar = 0;
    for (map<int, int>::iterator it = valueCounts.begin(); it != valueCounts.end(); ++it) {
        if (countSoFar < it->first) {
            // we can never pick up these cards (or any afterwards)
            return 0;
        }
        countSoFar += it->second;
        it->second = countSoFar;
    }

    int cardsPickedUp = 0;
    long long waysToPickUpCards = 1;
    for (map<int, int>::const_iterator itNext = valueCounts.begin(); itNext != valueCounts.end(); ) {
        map<int, int>::const_iterator it = itNext++;
        //cout << "(" << it->first << "," << it->second << ")" << endl;
        // If we're at the end, we need to pick up all the cards, so use current iterator's "count".
        int totalCardsNeededForNextLevel = itNext != valueCounts.end() ? itNext->first : it->second;
        int additionalCardsNeededForNextLevel = totalCardsNeededForNextLevel - cardsPickedUp;
        int cardsAvailableToPickUp = it->second - cardsPickedUp;
        waysToPickUpCards = multiplyByPartialFactorialAndMod(waysToPickUpCards,
                                                             cardsAvailableToPickUp,
                                                             additionalCardsNeededForNextLevel);
        cardsPickedUp += additionalCardsNeededForNextLevel;
    }

    // This gets modulo'd every time we modify it, so it will definitely fit in a 32bit int at this point.
    return (int)waysToPickUpCards;
}


int main(void)
{
    int testRuns = 0;
    cin >> testRuns;
    for (int i = 0; i < testRuns; ++i)
        cout << performTestRun() << endl;
    return 0;
}
