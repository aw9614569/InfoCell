#include <cstdint>  // for uint8_t
#include <fstream>  // IWYU pragma: keep
#include <iostream> // for operator<<, stringstream, basic_ostream, flush, cout, ostream
#include <map>      // for _Rb_tree_const_iterator, map, operator!=, operator==
#include <memory>   // for allocator
#include <sstream>  // IWYU pragma: keep
#include <utility>  // for pair

#include "screen.h"
#include "string.h" // for string_width

namespace fsvgui {

/// A fixed dimension.
/// @see Fit
/// @see Full
Dimensions Dimension::Fixed(int v)
{
    return { v, v };
}

/// Use the terminal dimensions.
/// @see Fixed
/// @see Fit
Dimensions Dimension::Full()
{
    return { 4096, 4096 };
}

// static
/// Create a screen with the given dimension along the x-axis and y-axis.
Screen Screen::Create(Dimensions width, Dimensions height)
{
    return { width.dimx, height.dimy };
}

// static
/// Create a screen with the given dimension.
Screen Screen::Create(Dimensions dimension)
{
    return { dimension.dimx, dimension.dimy };
}

Screen::Screen(int dimx, int dimy) :
    stencil { 0, dimx - 1, 0, dimy - 1 },
    dimx_(dimx),
    dimy_(dimy)
{
}

void Screen::addSvg(int x, int y, const std::string& svgElement)
{
    m_svgItems.insert({ { x, y }, svgElement });
}

/// Produce a std::string that can be used to print the Screen on the terminal.
/// Don't forget to flush stdout. Alternatively, you can use Screen::Print();
std::string Screen::toString()
{
    std::stringstream ss;

    for (const auto& svgItemPair : m_svgItems) {
        const std::string& svgItem = svgItemPair.second;
        ss << svgItem << "\n";
    }

    return ss.str();
}

void Screen::writeFile(const std::filesystem::path& path)
{
    std::ofstream svgFile(path);
    svgFile << "<svg  xmlns=\"http://www.w3.org/2000/svg\" width=\"" << dimx_ << "\" height=\"" << dimy_ << "\">\n";
    svgFile << toString();
    svgFile << "</svg>";
}

/// @brief Clear all the pixel from the screen.
void Screen::clear()
{
    m_svgItems.clear();
}

} // namespace fsvgui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
