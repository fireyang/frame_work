
#include "common_util.h"

using namespace std;

int map_value_greater(const pair<int, int>& left, const pair<int, int>& right)
{
	return left.second > right.second;
}

int map_value_less(const pair<int, int>& left, const pair<int, int>& right)
{
	return left.second < right.second;
}

