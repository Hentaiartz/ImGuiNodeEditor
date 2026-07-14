#pragma once
#include <vector>
#include <memory>
#include "NodeEditor/Node.h"

struct ThemeNode;
struct Link;

void RenderLivePreview(const std::vector<std::unique_ptr<Node>>& allNodes,
                       const std::vector<Link>& allLinks);
