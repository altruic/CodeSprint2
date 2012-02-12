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


void printMap(const map<int, int>& valueCounts)
{
    cout << "Printing map: ";
    for (map<int, int>::const_iterator it = valueCounts.begin(); it != valueCounts.end(); ++it)
       cout << "(" << it->first << "," << it->second << ") -> ";
    cout << "[END]" << endl;
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
    
    printMap(valueCounts);

    int countSoFar = 0;
    for (map<int, int>::iterator it = valueCounts.begin(); it != valueCounts.end(); ++it) {
        if (countSoFar < it->first) {
            // we can never pick up these cards (or any afterwards)
            return 0;
        }
        countSoFar += it->second;
        it->second = countSoFar;
    }

    printMap(valueCounts);

//    map<int, int>::const_iterator it = valueCounts.begin();
//    // todo: check if bounds are correct here. do I need to go to numCards + 1?
//    for (int cardsPickedUp = 0; cardsPickedUp < numCards; ++cardsPickedUp) {
//        it = valueCounts.lower_bound(cardsPickedUp);
//        // todo: check if "--it" works when lower_bound finds end()???
//        if (it == valueCounts.end() || it->first != cardsPickedUp)
//            --it; // we don't have any cards with this value, move backward to investigate a lower value
//    }

//    int cardsPickedUp = 0;
//    int waysToPickUpCards = 1;
//    for (map<int, int>::const_iterator itNext = valueCounts.begin(); itNext != valueCounts.end(); ) {
//        map<int, int>::const_iterator it = itNext++;
//        int maxTotalCardsThatCanBePickedUp = itNext == valueCounts.end() || itNext->first > cardsPickedUp
//            ? it->second : itNext->second;
//    }

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
    for (int i = 0; i < testRuns; ++i) {
        cout << "Test " << i << endl;
        cout << performTestRun() << endl;
    }
    return 0;
}
