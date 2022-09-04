#pragma once

#include "cells/Cells.h"
#include "util/ArcTask.h"
#include "util/Logging.h"

#include <set>

namespace synth {

enum class RotationDir
{
    Degree_0,   // 🡩
    Degree_45,  // 🡭
    Degree_90,  // 🡪
    Degree_135, // 🡮
    Degree_180, // 🡫
    Degree_225, // 🡯
    Degree_270, // 🡨
    Degree_315  // 🡬
};

extern const std::array<cells::Color, 10> cellColors;
extern const std::map<const cells::Color, int> cellColorsToColorId;
const cells::Color& color(arc::Colors c);

// ============================================================================
class Pixel
{
public:
    Pixel(int x, int y) :
        x(x), y(y)
    {
    }

    bool operator<(const Pixel& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    int x;
    int y;
};

// ============================================================================
class Vector
{
public:
    bool operator<(const Vector& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    Vector operator+(const Vector& rhs) const
    {
        return Vector({ x + rhs.x, y + rhs.y });
    }

    Vector operator-(const Vector& rhs) const
    {
        return Vector({ x - rhs.x, y - rhs.y });
    }

    Vector& operator+=(const Vector& rhs);
    Vector& operator-=(const Vector& rhs);

    Vector rotate(RotationDir rotationDir) const;

    int x;
    int y;
};

// ============================================================================
class VectorShape
{
public:
    VectorShape(const std::vector<Vector>& vectors, const cells::Color& color, const Pixel& firstPixel) :
        m_vectors(vectors), m_color(color), m_firstPixel(firstPixel)
    {
    }
    VectorShape(std::vector<Vector>&& vectors, const cells::Color& color, const Pixel& firstPixel) :
        m_vectors(std::move(vectors)), m_color(color), m_firstPixel(firstPixel)
    {
    }

    VectorShape rotate(RotationDir rotationDir) const;
    VectorShape stretch(int horizontal, int vertical) const;
    VectorShape mirror(const Vector firsPixelDistance, RotationDir axisDir) const;

    const cells::Color& color() const
    {
        return m_color;
    }

    const std::vector<Vector>& vectors() const
    {
        return m_vectors;
    }

    const Pixel& firstPixel() const
    {
        return m_firstPixel;
    }

    void firstPixel(const Pixel& pixel)
    {
        m_firstPixel = pixel;
    }

private:
    static void stretchPixel(std::vector<Vector>& stretchVectors, int horizontal, int vertical);

    cells::Color m_color;
    std::vector<Vector> m_vectors;
    Pixel m_firstPixel;
};

// ============================================================================
class Patch;
class PatchBoardI
{
public:
    virtual void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y)       = 0;
    virtual void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) = 0;

    virtual int width() const  = 0;
    virtual int height() const = 0;
};

// ============================================================================
class Patch : public std::enable_shared_from_this<Patch>
{
public:
    Patch(cells::Color color, PatchBoardI* patchBoardI) :
        m_color(color), m_patchBoardI(patchBoardI), m_width(patchBoardI->width()), m_height(patchBoardI->height()) { }

    void registerSubscribedPixel(int x, int y)
    {
        m_subscribedPixels.insert({ x, y });
    }

    void addPixelCoordinate(int x, int y);
    void merge(std::shared_ptr<Patch> other);
    void sortPixels();
    VectorShape vectorize() const;
    void refreshBoundaries(int x, int y);

    int id() const
    {
        return m_id;
    }
    void id(int id)
    {
        m_id = id;
    }

    const cells::Color& color() const
    {
        return m_color;
    }

    size_t size() const
    {
        return m_pixels.size();
    }

    const Pixel& firstPixel() const
    {
        return m_pixels.front();
    }

    const Pixel& lastPixel() const
    {
        return m_pixels.back();
    }

    int pixelIndex(const Pixel& pixel) const
    {
        return pixel.y * m_width + pixel.x;
    }

    int firstPixelIndex() const
    {
        const Pixel& pixel = firstPixel();
        return pixel.y * m_width + pixel.x;
    }

    bool operator<(const Patch& rhs) const
    {
        return firstPixelIndex() < rhs.firstPixelIndex();
    }

    int patchWidth() const
    {
        return m_rightX - m_leftX;
    }

    int patchHeight() const
    {
        return m_bottomY - m_topY;
    }

    std::string toString() const;

    const std::set<Pixel>& subscribedPixels() const
    {
        return m_subscribedPixels;
    }

private:
    int m_id      = 0;
    int m_leftX   = 0;
    int m_rightX  = 0;
    int m_topY    = 0;
    int m_bottomY = 0;

    cells::Color m_color;
    PatchBoardI* m_patchBoardI = nullptr;
    const int m_width;
    const int m_height;
    std::vector<Pixel> m_pixels;
    std::set<Pixel> m_subscribedPixels;
};

// ============================================================================
struct PatchSlot
{
    PatchSlot(PatchBoardI* patchBoardI) :
        m_patchBoardI(patchBoardI) { }

    void registerCandidate(std::shared_ptr<Patch> patch);
    void unRegisterCandidate(std::shared_ptr<Patch> patch);
    std::shared_ptr<Patch> getCandidate(const cells::Color& color);

private:
    PatchBoardI* m_patchBoardI = nullptr;
    std::map<cells::Color, std::set<std::shared_ptr<Patch>>> m_candidates;
};

// ============================================================================
class DrawingBoard
{
public:
    DrawingBoard(int width, int height);

    void clear();
    void setColor(int x, int y, cells::Color color);
    void renderLine(int x, int y, const cells::Color& color, RotationDir direction);
    void renderVectorShape(const VectorShape& vectorShape)
    {
        renderVectorShape(vectorShape.firstPixel().x, vectorShape.firstPixel().y, vectorShape);
    }

    void renderVectorShape(int x, int y, const VectorShape& vectorShape);

    std::string toString() const;

    int lastXIndex() const
    {
        return m_width - 1;
    }

    int lastYIndex() const
    {
        return m_height - 1;
    }

private:
    int currentIndex(int x, int y) const
    {
        return y * m_width + x;
    }

    bool isInRange(int x, int y) const
    {
        if (y < 0 || x < 0 || x >= m_width || y >= m_height) {
            return false;
        }

        return true;
    }

    const int m_width;
    const int m_height;
    const cells::Color& m_defaultBgColor;
    std::vector<cells::Color> m_colors;
};

// ============================================================================
class PatchBoard : public PatchBoardI
{
public:
    PatchBoard(const cells::Sensor& sensor) :
        m_width(sensor.width()), m_height(sensor.height()), m_sensor(sensor), m_patchSlots(m_height * m_width, PatchSlot(this))
    {
    }

    int width() const override
    {
        return m_width;
    }

    int height() const override
    {
        return m_height;
    }

    void process();
    void processPixel(int x, int y, const cells::Color& color);

    const std::set<std::shared_ptr<Patch>>& patches() const
    {
        return m_patches;
    }

    void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y) override;
    void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) override;

protected:
    void subscribePatchForPixelImpl(std::shared_ptr<Patch> patch, int x, int y);

    PatchSlot& getPatchSlot(int x, int y)
    {
        return m_patchSlots[currentIndex(x, y)];
    }

    int currentIndex(int x, int y) const
    {
        return y * m_width + x;
    }

    bool isInRange(int x, int y) const
    {
        if (y < 0 || x < 0 || x > m_width - 1 || y > m_height - 1) {
            return false;
        }

        return true;
    }

    const int m_width;
    const int m_height;
    const cells::Sensor& m_sensor;
    std::vector<PatchSlot> m_patchSlots;
    std::set<std::shared_ptr<Patch>> m_patches;
};

// ============================================================================
class Solver
{
public:
    Solver(Logger& logger, const ArcTask& arcTask);

    void solve();
    void solveOne(const cells::Sensor& sensor);

    Logger& logger;
    const ArcTask& m_arcTask;
};

} // namespace synth