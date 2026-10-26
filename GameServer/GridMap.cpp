#include "pch.h"
#include "GridMap.h"

#include "FileUtils.h"
#include <sstream>

#define ROOT2 (1.4142f)

bool GridMap::Init(const char* path)
{
    _cells.resize(100'0000, false);
    _xSize = 1000;
    _ySize = 1000;
    _cellWidth = 100;

    return true;


    Vector<BYTE> gridData = FileUtils::ReadFile(Utils::ToWString(path).c_str());
    Vector<string> lines = SplitLine(string(gridData.begin(), gridData.end()));

    Vector<string> tokens = SplitToken(lines[0]);

    if (tokens.size() != 3)
        return false;

    _xSize = stoi(tokens[0].c_str());
    _ySize = stoi(tokens[1].c_str());
    _cellWidth = stoi(tokens[2].c_str());
    _origin = Vector3();

	_cells.resize(_xSize * _ySize);

	if (lines.size() < _ySize + 1)
		return false;

	bool success = true;
	for (size_t j = 0; j < _ySize; ++j)
	{
		Vector<string> cell = SplitToken(lines[j + 1]);
		if (cell.size() < _xSize)
		{
			return false;
		}

		for (size_t cellIdx = 0; cellIdx < cell.size(); ++cellIdx)
		{
			_cells[j * _xSize + cellIdx] = stoi(cell[cellIdx]);
		}
	}
	return true;
}

bool GridMap::FindPath(const Vector3& from, const Vector3& to, Vector<Vector3>& outPath)
{
    outPath.clear();
    if (_cells.empty()) return false;

    int32 startXIdx, startYIdx;
    int32 goalXIdx, goalYIdx;
    ToCell(from, &startXIdx, &startYIdx);
    ToCell(to, &goalXIdx, &goalYIdx);

    if (!CellWalkable(startXIdx, startYIdx) || !CellWalkable(goalXIdx, goalYIdx)) return false;

    const int32 cols = _xSize;
    const int32 startCell = startYIdx * cols + startXIdx;
    const int32 goalCell = goalYIdx * cols + goalXIdx;

    if (startCell == goalCell)
    {
        outPath.push_back(from);
        outPath.push_back(to);
        return true;
    }

    auto heuristic = [&](int32 x, int32 y) -> float
        {
            float dx = static_cast<float>(std::abs(x - goalXIdx));
            float dy = static_cast<float>(std::abs(y - goalYIdx));
            return (dx + dy) + (ROOT2 - 2.f) * (std::min)(dx, dy);
        };

    std::unordered_map<int32, float> gScore;
    std::unordered_map<int32, int32> cameFrom;

    using PQItem = std::pair<float, int32>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> open;

    gScore[startCell] = 0.f;
    open.push({ heuristic(startXIdx, startYIdx), startCell });

    static const int32 DX[8] = { 1, -1, 0, 0,  1,  1, -1, -1 };
    static const int32 DZ[8] = { 0, 0, 1, -1,  1, -1,  1, -1 };

    constexpr int32 MAX_EXPAND = 20000;
    int32 expanded = 0;
    bool  found = false;

    while (!open.empty())
    {
        auto [f, cur] = open.top();
        open.pop();

        if (cur == goalCell) { found = true; break; }
        if (++expanded > MAX_EXPAND) break;

        const int32 curentX = cur % cols;
        const int32 currentY = cur / cols;
        const float curG = gScore[cur];

        for (int i = 0; i < 8; ++i)
        {
            const int32 nx = curentX + DX[i];
            const int32 nz = currentY + DZ[i];
            if (!CellWalkable(nx, nz)) continue;

            if (i >= 4 && (!CellWalkable(curentX + DX[i], currentY) || !CellWalkable(curentX, currentY + DZ[i])))
                continue;

            const float stepCost = (i >= 4) ? ROOT2 : 1.f;
            const int32 nCell = nz * cols + nx;
            const float tentative = curG + stepCost;

            auto it = gScore.find(nCell);
            if (it == gScore.end() || tentative < it->second)
            {
                gScore[nCell] = tentative;
                cameFrom[nCell] = cur;
                open.push({ tentative + heuristic(nx, nz), nCell });
            }
        }
    }

    if (!found) return false;

    Vector<int32> cellPath;
    for (int32 c = goalCell; ; c = cameFrom[c])
    {
        cellPath.push_back(c);
        if (c == startCell) break;
    }

    Vector<int32> fwd;
    fwd.reserve(cellPath.size());
    for (int32 i = static_cast<int32>(cellPath.size()) - 1; i >= 0; --i)
        fwd.push_back(cellPath[i]);

    // 꺾이는 지점을 알아내고, 직선구간의 경로를 하나로 묶는다
    Vector<int32> corners;
    corners.push_back(fwd.front());
    int32 anchor = 0;
    for (int32 i = 2; i < static_cast<int32>(fwd.size()); ++i)
    {
        int32 ax = fwd[anchor] % cols, ay = fwd[anchor] / cols;
        int32 bx = fwd[i] % cols, by = fwd[i] / cols;
        if (!LineOfSight(ax, ay, bx, by))
        {
            corners.push_back(fwd[i - 1]);
            anchor = i - 1;
        }
    }
    corners.push_back(fwd.back());

    outPath.push_back(from);
    for (int32 i = 1; i < static_cast<int32>(corners.size()) - 1; ++i)
        outPath.push_back(CellCenter(corners[i] % cols, corners[i] / cols));
    outPath.push_back(to);
    return true;
}

bool GridMap::CanGo(Vector3& to)
{
    int32 xIdx = 0;
    int32 yIdx = 0;
    ToCell(to, &xIdx, &yIdx);    

    return CellWalkable(xIdx, yIdx);
}

void GridMap::ToCell(Vector3& location, int32* xIdx, int32* yIdx)
{
    *xIdx = static_cast<int32>(std::floor((location.x - _origin.x) / _cellWidth));
    *yIdx = static_cast<int32>(std::floor((location.y - _origin.y) / _cellWidth));
}

Vector3 GridMap::CellCenter(int32 xIdx, int32 yIdx)
{
    return Vector3{
        _origin.x + (static_cast<float>(xIdx) + 0.5f) * _cellWidth,
        _origin.y + (static_cast<float>(yIdx) + 0.5f) * _cellWidth,
        0.f
    };
}


bool GridMap::LineOfSight(int32 x0, int32 y0, int32 x1, int32 y1)
{
    int32 dx = std::abs(x1 - x0);
    int32 dy = std::abs(y1 - y0);
    int32 sx = (x0 < x1) ? 1 : -1;
    int32 sy = (y0 < y1) ? 1 : -1;
    int32 err = dx - dy;
    int32 x = x0, z = y0;

    while (true)
    {
        if (!CellWalkable(x, z)) return false;
        if (x == x1 && z == y1)  return true;

        int32 e2 = 2 * err;
        bool steppedX = false, steppedZ = false;
        if (e2 > -dy) { err -= dy; x += sx; steppedX = true; }
        if (e2 < dx) { err += dx; z += sy; steppedZ = true; }

        if (steppedX && steppedZ &&
            (!CellWalkable(x - sx, z) || !CellWalkable(x, z - sy)))
            return false;
    }
}

Vector<string> GridMap::SplitLine(string str)
{
	if (str.size() >= 3 &&
		static_cast<unsigned char>(str[0]) == 0xEF &&
		static_cast<unsigned char>(str[1]) == 0xBB &&
		static_cast<unsigned char>(str[2]) == 0xBF)
	{
		str.erase(0, 3);
	}

	Vector<string> ret;

	istringstream stream(str);
	string line;

	while (std::getline(stream, line))
	{
		const size_t first = line.find_first_not_of("\t\r\n");
		if (first == std::string::npos)
		{
			continue;
		}

		const size_t last = line.find_last_not_of("\t\r\n");
		ret.push_back(line.substr(first, last - first + 1));
	}


	return ret;
}

Vector<string> GridMap::SplitToken(string str)
{
	Vector<string> Tokens;

	std::istringstream Stream(str);
	std::string Token;

	while (Stream >> Token)
	{
		Tokens.push_back(Token);
	}

	return Tokens;
}
