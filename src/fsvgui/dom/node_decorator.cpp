#include <memory> // for __shared_ptr_access
#include <vector> // for __alloc_traits<>::value_type

#include "box.h"  // for Box
#include "node.h" // for Node, Elements
#include "node_decorator.h"
#include "requirement.h" // for Requirement

namespace fsvgui {

void NodeDecorator::ComputeRequirement()
{
    Node::ComputeRequirement();
    requirement_ = children_[0]->requirement();
}

void NodeDecorator::SetBox(Box box)
{
    Node::SetBox(box);
    children_[0]->SetBox(box);
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
