#include "../../CloseToHSL/Core/Architecture/Common/MemoryStreamCompat.h"
#include <cassert>
int main() {
 System::IO::MemoryStream s; std::vector<unsigned char> a{1,2,3}, b(4);
 s.Write(a,0,3); assert(s.getLength()==3); s.Seek(0,System::IO::SeekOrigin::Begin);
 assert(s.Read(b,0,4)==3 && b[0]==1 && b[2]==3 && s.ReadByte()==-1);
 s.setPosition(5); s.WriteByte(9); auto x=s.ToArray(); assert(x.size()==6 && x[3]==0 && x[5]==9);
 s.SetLength(2); assert(s.getLength()==2 && s.getPosition()==2);
}