#include <memory>  // for make_shared, __shared_ptr_access
#include <utility> // for move
#include <vector>  // for __alloc_traits<>::value_type

#include "box.h"         // for Box
#include "elements.h"    // for Element, unpack, Decorator, reflect
#include "node.h"        // for Node, Elements
#include "requirement.h" // for Requirement
#include "screen.h"      // for Screen

namespace fsvgui {

// Helper class.
class Reflect : public Node
{
public:
    Reflect(Element child, Box& box) :
        Node(unpack(std::move(child))), reflected_box_(box) { }

    void ComputeRequirement() final
    {
        Node::ComputeRequirement();
        requirement_ = children_[0]->requirement();
    }

    void SetBox(Box box) final
    {
        reflected_box_ = box;
        Node::SetBox(box);
        children_[0]->SetBox(box);
    }

    void Render(Screen& screen) final
    {
        reflected_box_ = Box::Intersection(screen.stencil, reflected_box_);
        return Node::Render(screen);
    }

private:
    Box& reflected_box_;
};

Decorator reflect(Box& box)
{
    return [&](Element child) -> Element {
        return std::make_shared<Reflect>(std::move(child), box);
    };
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
