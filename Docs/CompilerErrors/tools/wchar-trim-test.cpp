#include "../../tangible_string_helper.h"
#include <cassert>
int main(){ assert(StringHelper::trim(L"  a  ")==L"a"); assert(StringHelper::trim(L"   ")==L""); assert(StringHelper::trim(L"")==L""); assert(StringHelper::trimStart(L"xyhelloxy",L"xy")==L"helloxy"); assert(StringHelper::trimEnd(L"xyhelloxy",L"xy")==L"xyhello"); assert(StringHelper::trim(L"abc",L"")==L"abc"); }
