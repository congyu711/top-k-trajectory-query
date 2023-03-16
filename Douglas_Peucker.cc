#ifndef __Dou
#define __Dou
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
// #include "function_declaration.h"
using namespace std;

struct Point
{
    double time;
    double x;
    double y;
    Point(double _time = 0.0, double _x = 0.0, double _y = 0.0) : time(_time), x(_x), y(_y) {}
    bool operator==(Point a)
    {
        if (a.x == this->x && a.y == this->y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

class Douglas_Peucker
{
private:
    float distanceofPointtoLine(const Point &p, const vector<Point> &line);
    void condense(const vector<Point> &pointList, vector<int> &keyPointIndexList, int firstIndex, int endIndex, const float &thrDistance);

public:
    bool execute(const vector<Point> &pointList, vector<Point> &resultPointList, const float &thrdistance);
};

bool Douglas_Peucker::execute(const vector<Point> &pointList, vector<Point> &resultPointList, const float &thrdistance)
{
    try
    {
        if (pointList.size() <= 2)
        {
            // cerr << "no need to compress.\n";
            throw "no need to compress";
            return false;
        }
        vector<int> keyPointIndexList;
        int firstIndex = 0;
        int endIndex = pointList.size() - 1;
        keyPointIndexList.push_back(firstIndex);
        keyPointIndexList.push_back(endIndex);
        resultPointList.clear();
        this->condense(pointList, keyPointIndexList, firstIndex, endIndex, thrdistance);
        sort(keyPointIndexList.begin(), keyPointIndexList.end());
        for (vector<int>::iterator iter = keyPointIndexList.begin(); iter != keyPointIndexList.end(); iter++)
        {
            resultPointList.push_back(pointList[*iter]);
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}

float Douglas_Peucker::distanceofPointtoLine(const Point &p, const vector<Point> &line)
{
    Point a(0, line[1].x - line[0].x, line[1].y - line[0].y);
    Point b(0, p.x - line[1].x, p.y - line[1].y);
    float area = abs(a.x * b.y - a.y * b.x);
    float distance = sqrt(pow(a.x, 2) + pow(a.y, 2));
    return area / distance;
}

void Douglas_Peucker::condense(const vector<Point> &pointList, vector<int> &keyPointIndexList, int firstIndex, int endIndex, const float &thrDistance)
{
    vector<Point> line;
    line.push_back(pointList[firstIndex]);
    line.push_back(pointList[endIndex]);
    int maxDistanceIndex = -1;
    float maxDistance = -1;
    for (int i = firstIndex; i <= endIndex; i++)
    {
        float distance = this->distanceofPointtoLine(pointList[i], line);
        if (distance > maxDistance)
        {
            maxDistanceIndex = i;
            maxDistance = distance;
        }
    }
    if (maxDistance > thrDistance)
    {
        keyPointIndexList.push_back(maxDistanceIndex);
        this->condense(pointList, keyPointIndexList, firstIndex, maxDistanceIndex, thrDistance);
        this->condense(pointList, keyPointIndexList, maxDistanceIndex, endIndex, thrDistance);
    }
}

#ifdef _Douglas_Peucker_test_
int main()
{
    Douglas_Peucker DP;
    vector<Point> pointList;
    vector<Point> resultPointList;
    pointList.push_back(Point(0, 0, 0));
    pointList.push_back(Point(0, 1, 2));
    pointList.push_back(Point(0, 2, 1));
    pointList.push_back(Point(0, 3, 1));
    pointList.push_back(Point(0, 5, 0));
    DP.execute(pointList, resultPointList, 0.5);
    int resultLength = resultPointList.size();
    for (int i = 0; i < resultLength; ++i)
    {
        std::cout << resultPointList[i].x << ',' << resultPointList[i].y << std::endl;
    }
}
#endif
#endif