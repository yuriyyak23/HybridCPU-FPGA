#include "../../Guid.h"
#include <cassert>
int main(){ auto a=Guid::NewGuid(), b=Guid::NewGuid(); assert(!(a==Guid::Empty) && !(a==b)); auto d=a.ToString(), n=a.ToString(L"N"); assert(d.size()==36 && n.size()==32 && d[8]==L'-' && d[13]==L'-' && d[18]==L'-' && d[23]==L'-' && n[12]==L'4' && (n[16]==L'8'||n[16]==L'9'||n[16]==L'a'||n[16]==L'b')); assert(Guid::Empty.ToString(L"N")==std::wstring(32,L'0')); }
