#include "Node.h"

Node::Node(int id, const std::string& name)
    : m_Id(id), m_Name(name), m_Pos(100, 100), m_Size(NODE_WIDTH, 60.0f)
{
}

std::vector<Pin*> Node::GetContainerInputs() {
    std::vector<Pin*> result;
    for (auto& p : m_Inputs)
        if (p.type == PinType::Container)
            result.push_back(&p);
    return result;
}

Pin* Node::GetContainerOutput() {
    for (auto& p : m_Outputs)
        if (p.type == PinType::Container)
            return &p;
    return nullptr;
}

