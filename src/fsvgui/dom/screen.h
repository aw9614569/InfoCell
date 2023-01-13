#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string> // for string, allocator, basic_string
#include <vector> // for vector

#include "box.h"   // for Box
#include "color.h" // for Color, Color::Default

namespace fsvgui {
struct Dimensions
{
    int dimx;
    int dimy;
};

struct Position
{
    int x = 0;
    int y = 0;

    bool operator<(const Position& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }
};

/// @brief Define how the Screen's dimensions should look like.
/// @ingroup screen
namespace Dimension {
Dimensions Fixed(int);
Dimensions Full();
} // namespace Dimension

class Screen
{
public:
    // Constructors:
    Screen(int dimx, int dimy);
    static Screen Create(Dimensions dimension);
    static Screen Create(Dimensions width, Dimensions height);

    // Node write into the screen using Screen::at.
    void addSvg(int x, int y, const std::string& svgElement);

    // Print the whole SVG
    std::string toString();
    void writeFile(const std::filesystem::path& path);

    // Get screen dimensions.
    int dimx() const { return dimx_; }
    int dimy() const { return dimy_; }

    // Clear every item
    void clear();

    Box stencil;

protected:
    int dimx_;
    int dimy_;
    std::multimap<Position, std::string> m_svgItems;
};

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
