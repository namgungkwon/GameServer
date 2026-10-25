#pragma once

class GridMap
{
public:
	bool Init(const char* path);

public:
	bool FindPath(Vector3& from, Vector3& to, Vector<Vector3>& outPath);
	bool CanGo(Vector3& to);


private:
	bool Validation(int32 xIdx, int32 yIdx) { return xIdx >= 0 && xIdx < _xSize && yIdx >= 0 && yIdx < _ySize; }
	void ToCell(Vector3& location, int32* xIdx, int32* yIdx);
	bool CellWalkable(int32 xIdx, int32 yIdx) {	return Validation(xIdx, yIdx) && (_cells[yIdx * _xSize + xIdx] == false); }
	Vector3 CellCenter(int32 cx, int32 cz);
	bool LineOfSight(int32 x0, int32 y0, int32 x1, int32 y1);

private:
	Vector<string> SplitLine(string str);
	Vector<string> SplitToken(string str);

private:
	int32 _xSize = 0;
	int32 _ySize = 0;
	int32 _cellWidth = 0;
	Vector<bool> _cells;
	Vector3 _origin = Vector3();
};
