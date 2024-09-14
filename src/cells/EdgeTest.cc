#include "CellTestBase.h"

using namespace synth;
using namespace synth::cells;

using synth::cells::test::CellTest;

TEST_F(CellTest, EdgeTest)
{
    auto& ShaperStruct = getStruct("arc::Shaper");

    const auto& printPixels = [this](CellI& pixelList) -> std::string {
        std::stringstream ss;
        Visitor::visitList(pixelList, [this, &ss](CellI& arcPixel, int, bool&) {
            ss << fmt::format("[{}, {}]", arcPixel["x"].label(), arcPixel["y"].label());
        });

        return ss.str();
    };

    // 0 7 7
    // 7 7 7
    // 0 7 7
    input::Grid inputGrid1("inputGrid1", "[[0, 7, 7, 7], [7, 7, 7, 7], [0, 7, 7, 7]]");
    cells::hybrid::arc::Grid grid1(kb, inputGrid1);
//    printGrid(grid1);
    Object shaper(kb, ShaperStruct, kb.name("constructor"), { "grid", grid1 });
    shaper.method("process");
    printAs.value(shaper["shapes"]["size"], "shaper[shapes][size]");
    EXPECT_EQ(&shaper["shapes"]["size"], &_3_);
    EXPECT_EQ(&shaper["shapes"]["first"]["value"]["pixels"]["size"], &_1_);
    CellI& shape2 = static_cast<Object&>(shaper["shapes"]["first"]["next"]["value"]);
    printAs.value(shape2["pixels"]["size"], R"("shape2["pixels"]["size"])");
    EXPECT_EQ(&shape2["pixels"]["size"], &toCellNumber(10));
    EXPECT_EQ(&toCellNumber(inputGrid1.width()), &shaper["width"]);
    EXPECT_EQ(&toCellNumber(inputGrid1.height()), &shaper["height"]);
    Object& shapePixels = static_cast<Object&>(shaper["shapePixels"]);

    CellI* previousUpPixel = nullptr;
    CellI* upPixel         = nullptr;
    CellI* leftPixel       = nullptr;
    CellI* firstShapePixel = nullptr;
    for (int y = 0; y < inputGrid1.height(); ++y) {
        Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
        for (int x = 0; x < inputGrid1.width(); ++x) {
            CellI& shapePixel = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
            if (!firstShapePixel) {
                firstShapePixel = &shapePixel;
            }
            CellI& pixel      = shapePixel["pixel"];
            if (leftPixel) {
                (*leftPixel).set("right", shapePixel);
                shapePixel.set("left", *leftPixel);
            }
            if (upPixel) {
                (*upPixel).set("down", shapePixel);
                shapePixel.set("up", *upPixel);
            }
            if (pixel.has(kb.directions.right)) {
                leftPixel = &shapePixel;
                if (upPixel) {
                    upPixel = &(*upPixel)["right"];
                }
            } else {
                // last column
                leftPixel = nullptr;
                if (previousUpPixel) {
                    upPixel = previousUpPixel;
                }
            }
            if (pixel.missing(kb.directions.left)) {
                // first column
                previousUpPixel = &shapePixel;
            }
        }
    }
    CellI* firstColumnPixelPtr = firstShapePixel;
    CellI* currentShapePixelPtr = firstShapePixel;

    // 1. row
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_FALSE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));
    EXPECT_EQ(&(*currentShapePixelPtr)["pixel"]["x"], &_0_);
    EXPECT_EQ(&(*currentShapePixelPtr)["pixel"]["y"], &_0_);
    EXPECT_EQ(static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]).m_x.value(), 0);
    EXPECT_EQ(static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]).m_y.value(), 0);
    EXPECT_TRUE(currentShapePixelPtr->has("shape"));
    EXPECT_EQ(&(*currentShapePixelPtr)["shape"]["id"], &_1_);

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));
    EXPECT_TRUE(currentShapePixelPtr->has("shape"));
    EXPECT_EQ(&(*currentShapePixelPtr)["shape"]["id"], &_2_);

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_FALSE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_FALSE(currentShapePixelPtr->has("right"));

    // 2. row
    currentShapePixelPtr = &firstColumnPixelPtr->get("down");
    firstColumnPixelPtr = currentShapePixelPtr;

    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_FALSE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_TRUE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_FALSE(currentShapePixelPtr->has("right"));

    // 3. row
    currentShapePixelPtr = &firstColumnPixelPtr->get("down");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_FALSE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_TRUE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = &currentShapePixelPtr->get("right");
    EXPECT_TRUE(currentShapePixelPtr->has("up"));
    EXPECT_FALSE(currentShapePixelPtr->has("down"));
    EXPECT_TRUE(currentShapePixelPtr->has("left"));
    EXPECT_FALSE(currentShapePixelPtr->has("right"));

    currentShapePixelPtr = firstShapePixel;
    firstColumnPixelPtr = firstShapePixel;

    while (currentShapePixelPtr) {
        hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["pixel"]);
        const int x                         = currentArcPixel.m_x.value();
        const int y                         = currentArcPixel.m_y.value();
        std::cout << "[" << x << ", " << y << "]";
        if (x == 0) {
            EXPECT_FALSE(currentShapePixelPtr->has("left"));
        } else {
            hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["left"]["pixel"]);
            EXPECT_EQ(x - 1, otherArcPixel.m_x.value());
        }
        if (x == inputGrid1.width() - 1) {
            EXPECT_FALSE(currentShapePixelPtr->has("right"));
            std::cout << std::endl;
        } else {
            hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["right"]["pixel"]);
            EXPECT_EQ(x + 1, otherArcPixel.m_x.value());
        }
        if (y == 0) {
            EXPECT_FALSE(currentShapePixelPtr->has("up"));
        } else {
            hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["up"]["pixel"]);
            EXPECT_EQ(y - 1, otherArcPixel.m_y.value());
        }
        if (y == inputGrid1.height() - 1) {
            EXPECT_FALSE(currentShapePixelPtr->has("down"));
        } else {
            hybrid::arc::Pixel& otherArcPixel = static_cast<hybrid::arc::Pixel&>((*currentShapePixelPtr)["down"]["pixel"]);
            EXPECT_EQ(y + 1, otherArcPixel.m_y.value());
        }
        if (currentShapePixelPtr->has("right")) {
            currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
        } else if (firstColumnPixelPtr->has("down")) {
            currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
            firstColumnPixelPtr  = currentShapePixelPtr;
        } else {
            currentShapePixelPtr = nullptr;
            std::cout << std::endl;
        }
    }

    currentShapePixelPtr = firstShapePixel;
    firstColumnPixelPtr  = firstShapePixel;
    enum class ScanLineState
    {
        Up,
        Middle,
        Down
    } scanLineState = ScanLineState::Up;
    while (currentShapePixelPtr) {
        CellI& currentShapePixel = *currentShapePixelPtr;
        hybrid::arc::Pixel& currentArcPixel = static_cast<hybrid::arc::Pixel&>(currentShapePixel["pixel"]);
        const int x                         = currentArcPixel.m_x.value();
        const int y                         = currentArcPixel.m_y.value();

        CellI& currentShape      = currentShapePixel["shape"];

        switch (scanLineState) {
        case ScanLineState::Up:
            if (currentShapePixel.has("up")) {
                CellI& upShape = currentShapePixel["up"]["shape"];
                if (&currentShape != &upShape) {
                    if (currentShapePixel.missing("right")) {
                        std::cout << "--+";
                    } else if (currentShapePixel.missing("left")) {
                        std::cout << "+-";
                    } else {
                        std::cout << "--";
                    }
                } else {
                    if (currentShapePixel.missing("right")) {
                        std::cout << "..|";
                    } else if (currentShapePixel.missing("left")) {
                        std::cout << "|.";
                    } else {
                        if (currentShapePixel.has("left") && &currentShapePixel["left"]["shape"] != &currentShape) {
                            std::cout << "+.";
                        } else if (currentShapePixel.has("left") && currentShapePixel.has("up") && &currentShapePixel["left"]["up"]["shape"] != &currentShape) {
                            std::cout << "+.";
                        } else {
                            std::cout << "..";
                        }
                    }
                }
            } else {
                if (currentShapePixel.missing("right")) {
                    std::cout << "--+";
                } else if (currentShapePixel.missing("left")) {
                    std::cout << "+-";
                } else {
                    std::cout << "--";
                }
            }
            break;
        case ScanLineState::Middle:
            if (currentShapePixel.has("left")) {
                CellI& leftShape = currentShapePixel["left"]["shape"];
                if (&currentShape != &leftShape) {
                    std::cout << "|" << currentShape["id"].label();
                } else {
                    if (currentShapePixel.missing("right")) {
                        std::cout << "." << currentShape["id"].label() << "|";
                    } else {
                        std::cout << "." << currentShape["id"].label();
                    }
                }
            } else {
                std::cout << "|" << currentShape["id"].label();
            }
            /*
            if (currentShapePixel.has("right")) {
                CellI& rightShape = currentShapePixel["right"]["shape"];
                if (&currentShape != &rightShape) {
                    std::cout << currentShape["id"].label() << "|";
                } else {
                    std::cout << currentShape["id"].label() << ".";
                }
            } else {
                std::cout << currentShape["id"].label() << "|";
            }
            */
            break;
        case ScanLineState::Down:
            if (currentShapePixel.missing("down")) {
                if (currentShapePixel.missing("right")) {
                    std::cout << "--+";
                } else if (currentShapePixel.missing("left")) {
                    std::cout << "+-";
                } else {
                    std::cout << "--";
                }
            }
            break;
        }

        if (currentShapePixelPtr->has("right")) {
            currentShapePixelPtr = &(*currentShapePixelPtr)["right"];
        } else if (firstColumnPixelPtr->has("down")) {
            if (scanLineState == ScanLineState::Up) {
                scanLineState = ScanLineState::Middle;
                currentShapePixelPtr = firstColumnPixelPtr;
                std::cout << std::endl;
            } else if (scanLineState == ScanLineState::Middle) {
                scanLineState = ScanLineState::Down;
                currentShapePixelPtr = firstColumnPixelPtr;
                std::cout << std::endl;
            } else if (scanLineState == ScanLineState::Down) {
                scanLineState = ScanLineState::Up;
                currentShapePixelPtr = &(*firstColumnPixelPtr)["down"];
                firstColumnPixelPtr  = currentShapePixelPtr;
            }
        } else {
            if (scanLineState == ScanLineState::Up) {
                scanLineState        = ScanLineState::Middle;
                currentShapePixelPtr = firstColumnPixelPtr;
            } else if (scanLineState == ScanLineState::Middle) {
                scanLineState        = ScanLineState::Down;
                currentShapePixelPtr = firstColumnPixelPtr;
            } else if (scanLineState == ScanLineState::Down) {
                currentShapePixelPtr = nullptr;
            }
            std::cout << std::endl;
        }
    }

    for (int y = 0; y < inputGrid1.height(); ++y) {
        Object& colX = static_cast<Object&>(shapePixels.method(kb.name("getValue"), { kb.ids.key, toCellNumber(y) }));
        for (int x = 0; x < inputGrid1.width(); ++x) {
            CellI& shapePixel         = colX.method(kb.name("getValue"), { kb.ids.key, toCellNumber(x) });
            CellI& shape              = shapePixel["shape"];
            hybrid::arc::Pixel& pixel = static_cast<hybrid::arc::Pixel&>(shapePixel["pixel"]);
            EXPECT_EQ(x, pixel.m_x.value());
            EXPECT_EQ(y, pixel.m_y.value());
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cout << "Constructed: " << CellI::s_constructed << ", destructed: " << CellI::s_destructed << ", live: " << CellI::s_constructed - CellI::s_destructed << std::endl;

    return ret;
}