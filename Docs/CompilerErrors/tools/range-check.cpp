struct Lane { int index; int *calls; int getPhysicalLaneIndex() const { ++*calls; return index; } };
bool valid(int register_Keyword) {return register_Keyword >= 0 && register_Keyword <= 31;}
bool lane(Lane issueLane) {const auto laneIndex = issueLane.getPhysicalLaneIndex(); return laneIndex >= 4 && laneIndex < 8;}
int main(){ if(valid(-1) || !valid(0) || !valid(31) || valid(32)) return 1; for(int i=0;i<10;++i){int c=0; if(lane({i,&c}) != (i>=4 && i<8) || c!=1) return 2;} return 0; }