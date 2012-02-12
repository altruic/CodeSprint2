/* Enter your code here. Read input from STDIN. Print output to STDOUT */
#include <cmath>
#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;


struct Location
{
    Location(void) : longitude(0.0), latitude(0.0) { }
    double longitude; // kinda like x on a grid
    double latitude; // kinda like y on a grid
};
    
double distanceBetweenLocations(const Location& loc1, const Location& loc2)
{
    // use basic Pythagorean Theorem for now because it's efficient.
    // NOTE: this is not accurate on the Earth's surface, especially for long distances.
    double longDiff = fabs(loc1.longitude - loc2.longitude);
    double latDiff = fabs(loc1.latitude - loc2.latitude);
    return sqrt(longDiff * longDiff + latDiff * latDiff);
}


typedef pair<double, int> DistanceEntityIdPair;
struct DistanceEntityIdPairLess
{
    bool operator()(const DistanceEntityIdPair& p1, const DistanceEntityIdPair& p2)
    {
        // I don't know if this is precise enough, but with locations possibly being up to 10 B,
        // this is just on the edge of double's guaranteed precision.
        static double threshold = 0.0001;

        double distanceDiff = p1.first - p2.first;
        return distanceDiff < -threshold ? true // p1 is closer than p2
            : distanceDiff > threshold ? false // p1 is further than p2
            // p1 is about the same distance as p2, return less (higher rank) if entity id is greater
            : p1.second > p2.second;
    }
};


int main(void)
{
    typedef int TopicIdType;
    typedef int QuestionIdType;

    int topicCount = 0, questionCount = 0, queryCount = 0;
    cin >> topicCount >> questionCount >> queryCount;

    typedef unordered_map<TopicIdType, Location> TopicLocationsMap;
    TopicLocationsMap topicLocations;
    for (int i = 0; i < topicCount; ++i) {
        TopicIdType topicId = 0;
        cin >> topicId;
        Location& loc = topicLocations[topicId];
        cin >> loc.latitude >> loc.longitude;
    }

    typedef unordered_multimap<TopicIdType, QuestionIdType> TopicQuestionsMultiMap;
    TopicQuestionsMultiMap topicQuestions;
    for (int i = 0; i < questionCount; ++i) {
        QuestionIdType questionId = 0;
        int topicCountForQuestion = 0;
        cin >> questionId >> topicCountForQuestion;
        for (int j = 0; j < topicCountForQuestion; ++j) {
            TopicIdType topicId = 0;
            cin >> topicId;
            topicQuestions.insert(make_pair(topicId, questionId));
        }
    }
    
    for (int i = 0; i < queryCount; ++i) {
        char queryType = '\0';
        int maxReturnValues = 0;
        Location location;
        cin >> queryType >> maxReturnValues >> location.latitude >> location.longitude;

        // need to find closest topics regardless of query type
        typedef set<DistanceEntityIdPair, DistanceEntityIdPairLess> DistanceEntityIdPairSet;
        DistanceEntityIdPairSet distanceTopicIdPairs;

        for (TopicLocationsMap::const_iterator it = topicLocations.begin(); it != topicLocations.end(); ++it) {
            double distance = distanceBetweenLocations(location, it->second);
            distanceTopicIdPairs.insert(DistanceEntityIdPair(distance, it->first));
        }

        DistanceEntityIdPairSet results;
        if (queryType == 't') { // topic
            // we don't have any more work to do, so just put our distance/topics in the final "results" set
            swap(results, distanceTopicIdPairs);
        }
        else { // queryType == 'q' // question
            // we can store our distance/questions directly to the final "results" set here
            set<QuestionIdType> addedQuestions;
            for (DistanceEntityIdPairSet::const_iterator distTopicIt = distanceTopicIdPairs.begin();
                    distTopicIt != distanceTopicIdPairs.end(); ++distTopicIt) {
                // Find all questions that reference this topic
                pair<TopicQuestionsMultiMap::const_iterator, TopicQuestionsMultiMap::const_iterator> range
                    = topicQuestions.equal_range(distTopicIt->second);
                for (TopicQuestionsMultiMap::const_iterator topicQuestionIt = range.first;
                        topicQuestionIt != range.second; ++topicQuestionIt) {
                    QuestionIdType questionId = topicQuestionIt->second;
                    if (addedQuestions.find(questionId) != addedQuestions.end())
                        continue; // This question is already recorded under a closer topic
                    results.insert(DistanceEntityIdPair(distTopicIt->first, questionId));
                    addedQuestions.insert(questionId);
                }
            }
        }

        if (maxReturnValues > 0 && !results.empty()) {
            DistanceEntityIdPairSet::const_iterator it = results.begin();
            cout << it->second;
            int resultsPrinted = 1;
            ++it; // advance to second result to start loop
            for (; resultsPrinted < maxReturnValues && it != results.end(); ++resultsPrinted, ++it)
                cout << " " << it->second;
        }
        cout << endl;
    }
    
    return 0;
}

