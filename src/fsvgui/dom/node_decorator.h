#pragma once

#include <utility> // for move

#include "elements.h" // for Element, unpack
#include "node.h"     // for Node

namespace fsvgui {
struct Box;

// Helper class.
class NodeDecorator : public Node
{
public:
    NodeDecorator(Element child) :
        Node(unpack(std::move(child))) { }
    void ComputeRequirement() override;
    void SetBox(Box box) override;
};

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
