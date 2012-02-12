/* Enter your code here. Read input from STDIN. Print output to STDOUT */
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
    std::string email;
    std::string address;
    std::string creditCardNumber;
};
    

std::string sanitizeEmail(const std::string& rawEmail)
{
    std::ostringstream sanitizedEmailOss;
    bool ignoreChars = false, atSignFound = false;
    for (std::string::const_iterator it = rawEmail.begin(); it != rawEmail.end(); ++it) {
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


std::string toupper(const std::string& origString)
{
    std::ostringstream oss;
    for (std::string::const_iterator it = origString.begin(); it != origString.end(); ++it)
        oss << (char)toupper(*it);
    return oss.str();
}


std::string replaceWord(const std::string& origString,
                        const std::string& wordToRemove, const std::string& wordToAdd,
                        std::size_t pos = 0)
{
    std::size_t index = origString.find(wordToRemove, pos);
    if (index == std::string::npos)
        return origString;

    std::size_t indexAfterMatch = index + wordToRemove.length();
    bool foundAtEnd = indexAfterMatch == origString.length();

    char characterBeforeMatch = index == 0 ? '\0' : origString[index - 1];
    if (isalpha(characterBeforeMatch))
        return foundAtEnd ? origString : replaceWord(origString, wordToRemove, wordToAdd, indexAfterMatch);

    if (!foundAtEnd && isalpha(origString[indexAfterMatch]))
        return replaceWord(origString, wordToRemove, wordToAdd, indexAfterMatch);

    std::string stringCopy = origString;
    return stringCopy.replace(index, wordToRemove.length(), wordToAdd);
}


std::string sanitizeStreet(const std::string& rawStreet)
{
    std::string sanitizedStreet = toupper(rawStreet);
    sanitizedStreet = replaceWord(sanitizedStreet, "STREET", "ST.");
    sanitizedStreet = replaceWord(sanitizedStreet, "ROAD", "RD.");
    return sanitizedStreet;
}


std::string sanitizeState(const std::string& rawState)
{
    std::string sanitizedState = toupper(rawState);
    sanitizedState = replaceWord(sanitizedState, "CALIFORNIA", "CA");
    sanitizedState = replaceWord(sanitizedState, "ILLINOIS", "IL");
    sanitizedState = replaceWord(sanitizedState, "NEW YORK", "NY");
    return sanitizedState;
}
    
    
Order* createOrderFromStdIn(void) {
    Order* pOrder = new Order;
    char comma = '\0';
    std::cin >> pOrder->orderId;
    std::cin >> comma;

    std::cin >> pOrder->dealId;
    std::cin >> comma;

    std::string rawEmail;
    getline(std::cin, rawEmail, ',');
    pOrder->email = sanitizeEmail(rawEmail);

    //std::cout << rawEmail << " -> " << pOrder->email << std::endl;

    std::string rawStreet;
    getline(std::cin, rawStreet, ',');
   
    std::string rawCity;
    getline(std::cin, rawCity, ',');
    
    std::string rawState;
    getline(std::cin, rawState, ',');
    
    std::string zipCode;
    getline(std::cin, zipCode, ',');

    std::ostringstream addressStream;
    addressStream << sanitizeStreet(rawStreet) << "," << toupper(rawCity) << ","
        << sanitizeState(rawState) << "," << zipCode;
    pOrder->address = addressStream.str();

    //std::cout << rawStreet << "," << rawCity << "," << rawState << "," << zipCode
    //    << " -> " << pOrder->address << std::endl;
    
    std::string creditCardNumber;
    getline(std::cin, pOrder->creditCardNumber);
    
    return pOrder;
}


struct OrderEmailHasher : std::unary_function<std::size_t, Order*>
{
    std::hash<std::string> stringHasher;
    std::size_t operator()(const Order* pOrder) const
        { return stringHasher(pOrder->email); }
};

struct OrderEmailEqualityTester : std::binary_function<bool, Order*, Order*>
{
    bool operator()(const Order* pO1, const Order* pO2) const
        { return pO1->email == pO2->email; }
};


struct OrderAddressHasher : std::unary_function<std::size_t, Order*>
{
    std::hash<std::string> stringHasher;
    std::size_t operator()(const Order* pOrder) const
        { return stringHasher(pOrder->address); }
};

struct OrderAddressEqualityTester : std::binary_function<bool, Order*, Order*>
{
    bool operator()(const Order* pO1, const Order* pO2) const
        { return pO1->address == pO2->address; }
};


int main(void)
{
    int testCases = 0;
    std::cin >> testCases;

    typedef std::vector<const Order*> OrderVector;
    OrderVector orders;
    orders.reserve(testCases);

    std::map<int, int> dealIdCounts;

    for (int i = 0; i < testCases; ++i) {
        Order* pOrder = createOrderFromStdIn();
        orders.push_back(pOrder);
        ++dealIdCounts[pOrder->dealId];
    }

    typedef std::unordered_set<const Order*, OrderEmailHasher, OrderEmailEqualityTester> EmailHashSet;
    std::map<int, EmailHashSet> dealIdEmailHashSets;

    typedef std::unordered_set<const Order*, OrderAddressHasher, OrderAddressEqualityTester> AddressHashSet;
    std::map<int, AddressHashSet> dealIdAddressHashSets;

    std::set<int> fraudulentOrderIds;
    
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

    std::set<int>::const_iterator it = fraudulentOrderIds.begin();
    if (it != fraudulentOrderIds.end()) {
        std::cout << *it;
        for (++it; it != fraudulentOrderIds.end(); ++it)
            std::cout << "," << *it;
    }
    
    return 0;
}
