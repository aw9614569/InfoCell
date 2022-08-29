#include "Solver.h"
#include <array>
#include <set>

namespace synth {

Logger* loggerPtr = nullptr;

const std::array<cells::Color, 10> cellColors = {
    cells::Color(0x00, 0x00, 0x00), /* black */
    cells::Color(0x00, 0x74, 0xD9), /* blue */
    cells::Color(0xFF, 0x41, 0x36), /* red */
    cells::Color(0x2E, 0xCC, 0x40), /* green */
    cells::Color(0xFF, 0xDC, 0x00), /* yellow */
    cells::Color(0xAA, 0xAA, 0xAA), /* grey */
    cells::Color(0xF0, 0x12, 0xBE), /* fuschia */
    cells::Color(0xFF, 0x85, 0x1B), /* orange */
    cells::Color(0x7F, 0xDB, 0xFF), /* teal */
    cells::Color(0x87, 0x0C, 0x25)  /* brown */
};

const std::map<const cells::Color, int> cellColorsToColorId = {
    { cellColors[0], 0 },
    { cellColors[1], 1 },
    { cellColors[2], 2 },
    { cellColors[3], 3 },
    { cellColors[4], 4 },
    { cellColors[5], 5 },
    { cellColors[6], 6 },
    { cellColors[7], 7 },
    { cellColors[8], 8 },
    { cellColors[9], 9 }
};

Solver::Solver(Logger& logger, const ArcTask& arcTask) :
    logger(logger), m_arcTask(arcTask)
{
    solve();
}


class Pixel
{
public:
    bool operator<(const Pixel& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    int x;
    int y;
};

class Patch;
class PatchBoardI
{
public:
    virtual void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y)       = 0;
    virtual void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) = 0;

    virtual int width() const  = 0;
    virtual int height() const = 0;
};

class Patch : public std::enable_shared_from_this<Patch>
{
public:
    Patch(cells::Color color, PatchBoardI* patchBoardI) :
        m_color(color), m_patchBoardI(patchBoardI), m_boardWidth(patchBoardI->width()), m_boardHeight(patchBoardI->height()) { }

    const cells::Color& color() const
    {
        return m_color;
    }

    void addPixelCoordinate(int x, int y)
    {
        refreshBoundaries(x, y);
        m_pixels.push_back({ x, y });
        m_patchBoardI->subscribePatchForPixel(shared_from_this(), x, y);
    }

    void registerSubscribedPixel(int x, int y)
    {
        m_subscribedPixels.insert({x, y});
    }

    void merge(std::shared_ptr<Patch> other)
    {
        for (const auto& pixel : other->m_pixels) {
            m_pixels.push_back(pixel);
        }
        sortPixels();
        m_patchBoardI->mergePatch(shared_from_this(), other);
    }

    void sortPixels()
    {
        int patchBoardWidth = m_patchBoardI->width();
        std::sort(m_pixels.begin(), m_pixels.end(), [patchBoardWidth](const Pixel& p1, const Pixel& p2) {
            return p1.y * patchBoardWidth + p1.x < p2.y * patchBoardWidth + p2.x;
        });
    }

    void vectorize()
    {
        vectors.clear();

        const Pixel* prevPixel = &m_pixels.front();
        bool firstPixel        = true;

        for (const auto& pixel : m_pixels) {
            if (firstPixel) {
                firstPixel = false;
                continue;
            }
            const Pixel* currPixel = &pixel;
            vectors.push_back({ currPixel->x - prevPixel->x, currPixel->y - prevPixel->y });
            prevPixel = currPixel;
        }
    }

    void refreshBoundaries(int x, int y)
    {
        if (x < m_leftX)
            m_leftX = x;
        if (x > m_rightX)
            m_rightX = x;
        if (y < m_topY)
            m_topY = y;
        if (y > m_bottomY)
            m_bottomY = y;
    }

    void assignId(int id)
    {
        m_id = id;
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
        return pixel.y * m_boardWidth + pixel.x;
    }

    int firstPixelIndex() const
    {
        const Pixel& pixel = firstPixel();
        return pixel.y * m_boardWidth + pixel.x;
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

    std::string toString() const
    {
        char boardColor = '0' + cellColorsToColorId.at(color());
        std::string board(m_boardWidth * m_boardHeight, '.');
        for (const Pixel& pixel : m_pixels) {
            board[pixel.y * m_boardWidth + pixel.x] = boardColor;
        }
        std::string ret;
        ret += std::to_string(m_boardWidth);
        ret += ' ';
        ret += std::to_string(m_boardHeight);
        ret += ' ';
        ret += board;

        return ret;
    }

    const std::set<Pixel>& subscribedPixels() const
    {
        return m_subscribedPixels;
    }

    int m_id = 0;

private:
    int m_leftX   = 0;
    int m_rightX  = 0;
    int m_topY    = 0;
    int m_bottomY = 0;

    cells::Color m_color;
    PatchBoardI* m_patchBoardI = nullptr;
    const int m_boardWidth;
    const int m_boardHeight;
    std::vector<cells::Vector> vectors;
    std::vector<Pixel> m_pixels;
    std::set<Pixel> m_subscribedPixels;
};

struct PatchSlot
{
    PatchSlot(PatchBoardI* patchBoardI) :
        m_patchBoardI(patchBoardI) { }

    void registerCandidate(std::shared_ptr<Patch> patch)
    {
        std::set<std::shared_ptr<Patch>>& patches = m_candidates[patch->color()];
        patches.insert(patch);
    }

    void unRegisterCandidate(std::shared_ptr<Patch> patch)
    {
        std::set<std::shared_ptr<Patch>>& patches = m_candidates[patch->color()];
        patches.erase(patch);
    }

    std::shared_ptr<Patch> getCandidate(const cells::Color& color)
    {
        auto findIt = m_candidates.find(color);

        // no patch
        if (findIt == m_candidates.end()) {
            return std::shared_ptr<Patch>();
        }

        std::set<std::shared_ptr<Patch>>& patches = m_candidates[color];

        // one patch
        auto firstPatchIt = patches.begin();
        if (patches.size() == 1)
            return *firstPatchIt;

        std::shared_ptr<Patch> returnPatch = *firstPatchIt;

        // multiple patch
        for (auto i = ++firstPatchIt; i != patches.end(); ) {
            std::set<std::shared_ptr<Patch>>::iterator current = i++;

            std::shared_ptr<Patch> candidatePatch = *current;
            if (*candidatePatch < *returnPatch) {
                auto tmp       = returnPatch;
                returnPatch    = candidatePatch;
                candidatePatch = tmp;
            }
            //loggerPtr->log(DEBUG) << " - patch (" << candidatePatch.get() << ") merged to " << returnPatch.get();
            //loggerPtr->log(DEBUG) << " - returnPatch (" << returnPatch.get() << ")";
            //loggerPtr->logBoard(DEBUG) << returnPatch->toString() << "\n";
            //loggerPtr->log(DEBUG) << " - candidatePatch (" << candidatePatch.get() << ")";
            //loggerPtr->logBoard(DEBUG) << candidatePatch->toString() << "\n";
            returnPatch->merge(candidatePatch);

            //loggerPtr->log(DEBUG) << " - returnPatch (" << returnPatch.get() << ")";
            //loggerPtr->logBoard(DEBUG) << returnPatch->toString() << "\n";
        }

        return returnPatch;
    }

    PatchBoardI* m_patchBoardI = nullptr;
    std::function<void(std::shared_ptr<Patch>)> m_deletePatchCb;
    std::map<cells::Color, std::set<std::shared_ptr<Patch>>> m_candidates;
};

std::ostream& operator<<(std::ostream& os, const cells::Color& color)
{
    os << "[" << color.red.value << "," << color.green.value << "," << color.blue.value << "]";
    return os;
}

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

    void process()
    {
        for (int y = 0; y < height(); ++y) {
            for (int x = 0; x < width(); ++x) {
                const cells::Pixel& pixel = m_sensor.getPixel(x, y);
                //                logger.log(DEBUG) << "Processing pixel[" << x << ", " << y << "]" << pixel.color;
                processPixel(x, y, pixel.color);
            }
        }
        int id = 1;
        std::vector<std::shared_ptr<Patch>> sortedPatches;
        for (std::shared_ptr<Patch> patch : m_patches) {
            sortedPatches.push_back(patch);
        }
        std::sort(sortedPatches.begin(), sortedPatches.end(), [](const std::shared_ptr<Patch>& lhs, const std::shared_ptr<Patch>& rhs) { return *lhs < *rhs; });
        for (std::shared_ptr<Patch> patch : sortedPatches) {
            patch->assignId(id++);
            patch->vectorize();
            loggerPtr->log(DEBUG) << "Patch " << patch->m_id << "\n";
            loggerPtr->logBoard(DEBUG) << patch->toString() << "\n";
        }
    }

    void processPixel(int x, int y, const cells::Color& color)
    {
        PatchSlot& patchSlot             = getPatchSlot(x, y);
        std::shared_ptr<Patch> candidate = patchSlot.getCandidate(color);

        if (candidate) {
            //loggerPtr->log(DEBUG) << " - pixel[" << x << ", " << y << "] " << color << " - patch found " << "(" << candidate.get() << ")";
        } else {
            candidate = std::make_shared<Patch>(color, this);
            //loggerPtr->log(DEBUG) << " - pixel[" << x << ", " << y << "] " << color << " - patch created " << "(" << candidate.get() << ")";
            m_patches.insert(candidate);
        }
        candidate->addPixelCoordinate(x, y);
    }

    int patchesCount() const
    {
        return m_patches.size();
    }

    void subscribePatchForPixel(std::shared_ptr<Patch> patch, int x, int y) override
    {
        subscribePatchForPixelImpl(patch, x + 1, y);
        subscribePatchForPixelImpl(patch, x + 1, y + 1);
        subscribePatchForPixelImpl(patch, x, y + 1);
        subscribePatchForPixelImpl(patch, x - 1, y + 1);
    }

    void mergePatch(std::shared_ptr<Patch> winner, std::shared_ptr<Patch> looser) override
    {
        const std::set<Pixel> subscribedPixels = looser->subscribedPixels();
        for (const Pixel& pixel : subscribedPixels) {
            PatchSlot& patchSlot = getPatchSlot(pixel.x, pixel.y);
            patchSlot.unRegisterCandidate(looser);
            patchSlot.registerCandidate(winner);
        }
        m_patches.erase(looser);
        //loggerPtr->log(DEBUG) << " - patch deleted (" << looser.get() << ")";
    }

protected:
    void subscribePatchForPixelImpl(std::shared_ptr<Patch> patch, int x, int y)
    {
        if (!isInRange(x, y))
            return;
        PatchSlot& patchSlot = getPatchSlot(x, y);
        patchSlot.registerCandidate(patch);
        patch->registerSubscribedPixel(x, y);
    }

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

void Solver::solve()
{
    loggerPtr = &logger;
    logger.clearLogFile();
    logger.log(INFO) << "There are " << m_arcTask.m_demonstrations.size() << " demo tasks";
    unsigned int i = 1;
    for (const auto& arcDemo : m_arcTask.m_demonstrations) {
        const cells::Sensor& m_input  = arcDemo.m_input;
        const cells::Sensor& m_output = arcDemo.m_output;
//        logger.log(INFO) << " (" << i << ") mapping[" << m_input.m_width << ", " << m_input.m_height << "] to[" << m_output.m_width << ", " << m_output.m_height << "] ";
        solveOne(m_input);
        solveOne(m_output);
        i++;
    }
//    solveOne(m_arcTask.m_testInput);

    const cells::Sensor& m_input = m_arcTask.m_testInput;
    logger.log(INFO) << "Mapping input[" << m_input.m_width << ", " << m_input.m_height << "] to ... ?";
}

void Solver::solveOne(const cells::Sensor& sensor)
{
    PatchBoard patchBoard(sensor);
    patchBoard.process();
    logger.log(DEBUG) << "Number of patches found: " << patchBoard.patchesCount();
}

/*

A program célja, hogy két pixelmátrixból megtalálja a transzformációs algoritmust.
A feladatok során mindig található egy transzformációs algoritmus, ami egyik mátrixból előállítja a másikat.

Első lépés, hogy a pixel mátrixban azonosítsuk az azonos színű alakzatokat. Az alakzat akár 1 pixeles is lehet.
Az egyik kitüntetett alakzat a "háttér" alakzat, ez jellemzően fekete. Sok példában, de nem mindben az alakzatok egy háttér előtt vannak.

Egy 2D alakzatot lehet forgatni, kicsinyíteni, nagyítani, mozgatni, átszínezni valamilyen szabály szerint.

Pixels => Shapes

Egyik fő stratégia, hogy megkeressük az egyik alakzatot a másikban.

Másik, hogy fő kategóriákat próbálunk végigpróbálni.

- az alakzat geometriai transzformációja
  az egyik a másiknak kicsinyítése - nagyítása, forgatása, x része (fele, negyede, ...), elmozgatása (más helyen van)
  Mert minden pixel egyben egy alakzat is. Az alakzaton lehet geometriai transzformációt végrehajtani.
  Minden pixel tetszőleges másik alakzatot alkothat, amin szintén lehet geometriai transzformációt végrehajtani.

- az alakzat darabszáma változik, ismétlődések, minták
  + az eredeti alakzat szimplán többször megtalálható a célban
  + az eredeti alakzat valamilyen formában megtalálható a célban, de hiányzik valami...

- az alakzat egy vagy több tulajdonságának vizuális leképzése jelenik meg
  pl. ahány kék négyzet van, annyi kék pixel lesz (1fad071e)

- reprezentáció
  pl. a piros reprezentálja a legkisebb alakzatot
  a pixelek rácsot alkotnak, amiben pixelek vannak
  van forrás-cél az alakzaton belül valamilyen elválasztó mentén

- kitüntett tulajdonság keresése
  pl. jelöld pirossal azokat a helyeket, ahol a legtöbb pixel van (29623171)

https://github.com/arc-community/wiki/wiki/Key-findings-by-various-people-and-FAQ#andreaskopf

Koncepciók, amik fontosak lehetnek:
 - alakzat / háttér felismerés
 - élek
 - sarok ( első körben 90 fokos egyenes vonalak esetén )
 - lekerekített / szögletes sarok
 - teli / üres
 - egyszerű alakzatok, pl. négyzet, téglalap, háromszög, kör
 - kívül / belül
 - ismétlődés
 - vonalak (egyenes, szaggatott, ...)
 - rendezés (pl. nagyság szerint)
 - szimmetriák
 - méretek
 - darabszám
 - távolság
 - minta folytatása
 - mozgások (visszapattan, vonz, taszít)
 - falak, labirintusok
 - logika
 - algoritmikus formák: spirál, hullámvonal
 - halmazműveletek
 - kivétel
 - zaj ( nem odaillő pixelek )

Két dimenziós világ:
 - a kép, mint x * y felbontású pixel mátrix
 - a mátrixon belül alakzatok lehetnek. Az alakzat lehet a háttér is.
 - a sík végtelen kiterjedésű, de a szenzor véges felbontású, tehát a szenzoron kívül is lehetnek pixelek, amiket a szenzor nem lát
 - az alakzatnak van körvonala
   - a körvonalnak van éle
   - az él lehet egyenes vagy görbe
   - egyenes élek találkozásakor sarokról beszélünk

Gondolkozási keret
==================

Forrás -> Cél
-------------
  - A feladat az, hogy rá kell jönni, mik azok a transzformációs lépések, amik a forrás alakzatból elvisznek a cél alakzatba.
  - A forrás és a cél egy pixel mátrix
  - A pixelnek lehet szine
  Cells: Szín, Pixel, Mátrix, Forrás, Cél

Az alakzat
----------
 - Az alakzat a forrás és cél alakzaton belüli pixel halmaz

 Nem tudjuk hány alakzat van, legalább egy van, de lehet több is.
 Az alakzatnak vannak tulajdonságai: pixelek száma, magasság - szélesség pixelben, pixelek színe
 Az alakzatokra bármilyen transzormáció alkalmazható.
 Az alakzat lehet háttér és objektum szerepben.
 A transzformáció bármilyen egy vagy többlépéses művelet lehet.
 A transzformációt műveletek írják le.

 A gondolkodási keret az, hogy van a forrás káp és a cél kép.
 A képen vannak alakzatok, amik megváltoznak valamilyen módon a cél képen.
 Az alakzatok valamilyen háttér előtt vannak.
 Az alakzatok kitakarják a hátteret.

Kategorizálás példák segítségével -> Kell egy önkategorizáló algoritmus
Felismerés, reprezentáció -> a pixelek reprezentálhatnak ismert dolgokat

Ismert dolgok: vonal (függőleges, vízszintes, ferde, görbe)
               A vonalak alkothatnak ábrákat.


Stratégia példák alapján:
https://qugank.github.io/pqa.github.io/

a. Körvonal által kijelölt terület beszínezése
   xxx.    xxx.
   x..x => xxxx
   .xx.    .xx.

b. Végpontok összekötese vonallá
   ....x...    ....x...
   x.......    x.......
   ........ => x.......
   x....x..    xxxxxx..
   ........    ........

c. Közelség értelmezése
   ....a...    ........
   b.......    ........
   ........ => ........
   c.dddd..    ..cccc..
   .....d..    .....c..
   A legközelebbi pixel színét veszi fel a több pixelből álló alakzat

d. Alakzat helyreállítása
   ........    ........
   xxx..x..    xxxxxx..
   ..xxx... => xxxxxx..
   x.xx.x..    xxxxxx..
   ........    ........
   Kiegészítés téglalappá

e. Alakzat és minta hasonlóság
   ab...a..    ab......
   .....b..    ........
   ........ => ........
   ..cccc..    ..aabb..
   ..cccc..    ..aabb..

f. Tükrözés 1 tengelyen
   ..aaa.a.    ..aaa.a.
   ..a...a.    ..a...a.
   ..bbbbb. => ..a...a.
   ........    ..aaa.a.
   ........    ........

g. 4 tengelyen tükrözött minta hiányzó részeinek megtalálása
   abcddcba    abcddcba
   bbadddab    bbadddab
   cbbbbbbc => cbbbbbbc
   cbb..bbc    cbbbbbbc
   bb....ab    bbadddab
   abcddcba    abcddcba
*/

} // namespace synth