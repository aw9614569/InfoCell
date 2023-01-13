#pragma once

namespace fsvgui {

struct Box
{
    int x_min = 0;
    int x_max = 0;
    int y_min = 0;
    int y_max = 0;

    int width() const;
    int height() const;

    static auto Intersection(Box a, Box b) -> Box;
    static auto Union(Box a, Box b) -> Box;
    bool Contain(int x, int y) const;
    bool operator==(const Box& other) const;
    bool operator!=(const Box& other) const;
};

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
