#include <rapidjson/document.h>
#include <string>

using namespace std;
using namespace rapidjson;

struct FFillContentTask
{
	void RunTask(Value::ConstValueIterator& InDoc, string& InOutContent);
};