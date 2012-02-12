// Cleaned-up version of my original submission

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_set>
#include <vector>

using namespace std;


struct Order
{
    int orderId;
    int dealId;
    string email;
    string address;
    string creditCardNumber;
};
    

string sanitizeEmail(const string& rawEmail)
{
    ostringstream sanitizedEmailOss;
    bool ignoreChars = false, atSignFound = false;
    for (string::const_iterator it = rawEmail.begin(); it != rawEmail.end(); ++it) {
        switch (*it) {
            case '@':
                atSignFound = true;
                ignoreChars = false;
                break;
            case '.':
                if (!atSignFound)
                    continue;
            case '+':
                if (!atSignFound) {
                    ignoreChars = true;
                    continue;
                }
            default:
                break;
        }
        if (!ignoreChars)
            sanitizedEmailOss << (char)toupper(*it);
    }
    return sanitizedEmailOss.str();
}


string toupper(const string& origString)
{
    ostringstream oss;
    for (string::const_iterator it = origString.begin(); it != origString.end(); ++it)
        oss << (char)toupper(*it);
    return oss.str();
}


string replaceWord(const string& origString,
                   const string& wordToRemove, const string& wordToAdd,
                   size_t pos = 0)
{
    size_t index = origString.find(wordToRemove, pos);
    if (index == string::npos)
        return origString;

    size_t indexAfterMatch = index + wordToRemove.length();
    bool foundAtEnd = indexAfterMatch == origString.length();

    char characterBeforeMatch = index == 0 ? '\0' : origString[index - 1];
    if (isalpha(characterBeforeMatch))
        return foundAtEnd ? origString : replaceWord(origString, wordToRemove, wordToAdd, indexAfterMatch);

    if (!foundAtEnd && isalpha(origString[indexAfterMatch]))
        return replaceWord(origString, wordToRemove, wordToAdd, indexAfterMatch);

    string stringCopy = origString;
    return stringCopy.replace(index, wordToRemove.length(), wordToAdd);
}


string sanitizeStreet(const string& rawStreet)
{
    string sanitizedStreet = toupper(rawStreet);
    sanitizedStreet = replaceWord(sanitizedStreet, "STREET", "ST.");
    sanitizedStreet = replaceWord(sanitizedStreet, "ROAD", "RD.");
    return sanitizedStreet;
}


string sanitizeState(const string& rawState)
{
    string sanitizedState = toupper(rawState);
    sanitizedState = replaceWord(sanitizedState, "CALIFORNIA", "CA");
    sanitizedState = replaceWord(sanitizedState, "ILLINOIS", "IL");
    sanitizedState = replaceWord(sanitizedState, "NEW YORK", "NY");
    return sanitizedState;
}
    
    
Order* createOrderFromStdIn(void) {
    Order* pOrder = new Order;
    char comma = '\0';
    cin >> pOrder->orderId;
    cin >> comma;

    cin >> pOrder->dealId;
    cin >> comma;

    string rawEmail;
    getline(cin, rawEmail, ',');
    pOrder->email = sanitizeEmail(rawEmail);

    //cout << rawEmail << " -> " << pOrder->email << endl;

    string rawStreet;
    getline(cin, rawStreet, ',');
   
    string rawCity;
    getline(cin, rawCity, ',');
    
    string rawState;
    getline(cin, rawState, ',');
    
    string zipCode;
    getline(cin, zipCode, ',');

    ostringstream addressStream;
    addressStream << sanitizeStreet(rawStreet) << "," << toupper(rawCity) << ","
        << sanitizeState(rawState) << "," << zipCode;
    pOrder->address = addressStream.str();

    //cout << rawStreet << "," << rawCity << "," << rawState << "," << zipCode
    //    << " -> " << pOrder->address << endl;
    
    string creditCardNumber;
    getline(cin, pOrder->creditCardNumber);
    
    return pOrder;
}


struct OrderEmailHasher : unary_function<Order*, size_t>
{
    hash<string> stringHasher;
    size_t operator()(const Order* pOrder) const
        { return stringHasher(pOrder->email); }
};

struct OrderEmailEqualityTester : binary_function<Order*, Order*, bool>
{
    bool operator()(const Order* pO1, const Order* pO2) const
        { return pO1->email == pO2->email; }
};


struct OrderAddressHasher : unary_function<Order*, size_t>
{
    hash<string> stringHasher;
    size_t operator()(const Order* pOrder) const
        { return stringHasher(pOrder->address); }
};

struct OrderAddressEqualityTester : binary_function<Order*, Order*, bool>
{
    bool operator()(const Order* pO1, const Order* pO2) const
        { return pO1->address == pO2->address; }
};


int main(void)
{
    int testCases = 0;
    cin >> testCases;

    typedef vector<const Order*> OrderVector;
    OrderVector orders;
    orders.reserve(testCases);

    map<int, int> dealIdCounts;

    for (int i = 0; i < testCases; ++i) {
        Order* pOrder = createOrderFromStdIn();
        orders.push_back(pOrder);
        ++dealIdCounts[pOrder->dealId];
    }

    typedef unordered_set<const Order*, OrderEmailHasher, OrderEmailEqualityTester> EmailHashSet;
    map<int, EmailHashSet> dealIdEmailHashSets;

    typedef unordered_set<const Order*, OrderAddressHasher, OrderAddressEqualityTester> AddressHashSet;
    map<int, AddressHashSet> dealIdAddressHashSets;

    set<int> fraudulentOrderIds;
    
    for (OrderVector::const_iterator it = orders.begin(); it != orders.end(); ++it) {
        const Order* pOrder = *it;

        EmailHashSet& emails = dealIdEmailHashSets[pOrder->dealId];
        EmailHashSet::iterator eait = emails.find(pOrder);
        if (eait == emails.end()) {
            emails.insert(pOrder);
        }
        else {
            fraudulentOrderIds.insert((*eait)->orderId);
            fraudulentOrderIds.insert(pOrder->orderId);
        }
        
        AddressHashSet& addresses = dealIdAddressHashSets[pOrder->dealId];
        AddressHashSet::iterator sait = addresses.find(pOrder);
        if (sait == addresses.end()) {
            addresses.insert(pOrder);
        }
        else {
            fraudulentOrderIds.insert((*sait)->orderId);
            fraudulentOrderIds.insert(pOrder->orderId);
        }
    }

    set<int>::const_iterator it = fraudulentOrderIds.begin();
    if (it != fraudulentOrderIds.end()) {
        cout << *it;
        for (++it; it != fraudulentOrderIds.end(); ++it)
            cout << "," << *it;
    }

	// cleanup all that allocated memory
    for (OrderVector::const_iterator it = orders.begin(); it != orders.end(); ++it)
		delete *it;
    
    return 0;
}
