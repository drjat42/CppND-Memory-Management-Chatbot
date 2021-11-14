#include "graphedge.h"
#include "graphnode.h"
#include <iostream>

GraphNode::GraphNode(int id)
{
    std::cout << "GraphNode " << id << " Constructor"  << std::endl;
    _id = id;
    _chatBot = nullptr;
    std::cout << "End GraphNode " << id << " Constructor"  << std::endl;
}

GraphNode::~GraphNode()
{
    //// STUDENT CODE
    ////
    std::cout << "GraphNode " << _id << " Destructor"  << std::endl;
    ////
    //// EOF STUDENT CODE
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    _childEdges.push_back(std::move(edge));
}

//// STUDENT CODE
////
void GraphNode::MoveChatbotHere(std::unique_ptr<ChatBot> chatbot)
{
    std::cout << "GraphNode MoveChatbotHere " << GetID() << std::endl;

    _chatBot = std::move(chatbot);
    _chatBot->SetCurrentNode(this);

    std::cout << "End GraphNode MoveChatbotHere " << std::endl;

}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    std::cout << "GraphNode MoveChatbotToNewNode from " << GetID() << " to " << newNode->GetID() << std::endl;

    newNode->MoveChatbotHere(std::move(_chatBot));
    _chatBot = nullptr;

    std::cout << "EndGraphNode MoveChatbotToNewNode " << std::endl;

}
////
//// EOF STUDENT CODE

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    //// STUDENT CODE
    ////

    return _childEdges[index].get();

    ////
    //// EOF STUDENT CODE
}