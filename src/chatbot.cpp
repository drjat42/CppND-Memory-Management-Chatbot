#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// constructor WITHOUT memory allocation
ChatBot::ChatBot()
{
    std::cout << "ChatBot No Image Constructor" << std::endl;

    // invalidate data handles
    _image = nullptr;
    _currentNode = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;

    std::cout << "End ChatBot No Image Constructor" << std::endl;

}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename)
{
    std::cout << "ChatBot Image Constructor" << std::endl;
    
    // invalidate data handles
    _currentNode = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;

    // load image into heap memory
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
    std::cout << "End ChatBot Image Constructor" << std::endl;
}

ChatBot::~ChatBot()
{
    std::cout << "ChatBot Destructor" << std::endl;

    // deallocate heap memory
    if(_image != NULL) // Attention: wxWidgets used NULL and not nullptr
    {
        delete _image;
        _image = NULL;
    }
    std::cout << "End ChatBot Destructor" << std::endl;
}

//// STUDENT CODE
////

// Copy constructor 
ChatBot::ChatBot(const ChatBot &other)
{
    std::cout << "ChatBot Copy Constructor" << std::endl;
    _currentNode = other._currentNode;
    _chatLogic = other._chatLogic;
    _chatLogic->SetChatbotHandle(this);
    _rootNode = other._rootNode;
    _image = other._image;
    std::cout << "End ChatBot Copy Constructor" << std::endl;

}

// Copy assignment operator
ChatBot & ChatBot::operator=(const ChatBot &other)
{
    std::cout << "ChatBot Copy Assignment Operator" << std::endl;
    if (this == &other) {
        return *this;
    }
    delete _image;
    _currentNode = other._currentNode;
    _chatLogic = other._chatLogic;
    _chatLogic->SetChatbotHandle(this);
    _rootNode = other._rootNode;
    _image = other._image;
    std::cout << "End ChatBot Assignment Constructor" << std::endl;

}

// Move constructor 
ChatBot::ChatBot(ChatBot &&other)
{
    std::cout << "ChatBot Move Constructor" << std::endl;
    _currentNode = other._currentNode;
    std::cout << "0" << std::endl;
    _chatLogic = other._chatLogic;
    std::cout << "1" << std::endl;
    _chatLogic->SetChatbotHandle(this);
    std::cout << "2" << std::endl;
    _rootNode = other._rootNode;
    std::cout << "3" << std::endl;
    _image = other._image;
    std::cout << "4" << std::endl;
    other._chatLogic = nullptr;
    std::cout << "5" << std::endl;
    other._rootNode = nullptr;
    std::cout << "6" << std::endl;
    if (other._image != NULL) {
        delete other._image;
    }
    std::cout << "End ChatBot Move Constructor" << std::endl;
}

// Move assignment operator
ChatBot & ChatBot::operator=(ChatBot &&other)
{
    std::cout << "ChatBot Move Assignment Operator" << std::endl;
    if (this == &other) {
        return *this;
    }
    if (_image != NULL) {
        delete _image;
    }
    _currentNode = other._currentNode;
    _chatLogic = other._chatLogic;
    _chatLogic->SetChatbotHandle(this);
    _rootNode = other._rootNode;
    _image = other._image;
    other._currentNode = nullptr;
    other._chatLogic = nullptr;
    other._rootNode = nullptr;
    other._image = NULL;
    std::cout << "End ChatBot Move Assignment Operator" << std::endl;

}
////
//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    // loop over all edges and keywords and compute Levenshtein distance to query
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // format is <ptr,levDist>

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
            levDists.push_back(ed);
        }
    }

    // select best fitting edge to proceed along
    GraphNode *newNode;
    if (levDists.size() > 0)
    {
        // sort in ascending order of Levenshtein distance (best fit is at the top)
        std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.at(0).first->GetChildNode(); // after sorting the best edge is at first position
    }
    else
    {
        // go back to root node
        newNode = _rootNode;
    }

    // tell current node to move chatbot to new node
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    std::cout << "ChatBot SetCurrentNode " << node->GetID() << std::endl;

    // update pointer to current node
    _currentNode = node;

    // select a random node answer (if several answers should exist)
    std::vector<std::string> answers = _currentNode->GetAnswers();

    std::cout << "ChatBot SetCurrentNode answers " << answers[0] << std::endl;

    std::mt19937 generator(int(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));

    std::cout << "ChatBot SetCurrentNode answer " << answer << std::endl;

    // send selected node answer to user
    _chatLogic->SendMessageToUser(answer);

    std::cout << "End ChatBot SetCurrentNode " << std::endl;

}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    // convert both strings to upper-case before comparing
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    // compute Levenshtein distance measure between both strings
    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0)
        return n;
    if (n == 0)
        return m;

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++)
        costs[k] = k;

    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;

    return result;
}