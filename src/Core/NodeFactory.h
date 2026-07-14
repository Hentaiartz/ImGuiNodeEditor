#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <imgui.h>

class Node;

struct NodeEntry {
    std::string name;
    std::string desc;
    std::string cat;
    std::function<std::unique_ptr<Node>(int)> createFn;
};

class NodeFactory {
public:
    void RegisterAll();

    std::unique_ptr<Node> Create(const char* type, int id) const;
    const NodeEntry* GetEntry(const char* type) const;
    const std::vector<NodeEntry>& GetEntries() const { return m_Entries; }
    ImU32 GetCategoryColor(const char* cat) const;

    static NodeFactory& Global();

private:
    template<typename T>
    void Register(const char* name, const char* desc, const char* cat) {
        m_Entries.push_back({
            name, desc, cat,
            [](int id) { return std::make_unique<T>(id); }
        });
    }

    std::vector<NodeEntry> m_Entries;
};

std::unique_ptr<Node> CreateNode(const char* type, int id);
