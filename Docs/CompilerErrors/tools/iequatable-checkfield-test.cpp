#include <concepts>
#include <string>
#include <vector>
struct DifferentialTraceDiscrepancy { std::wstring a,b; DifferentialTraceDiscrepancy(int,unsigned long long,const std::wstring&,const std::wstring& x,const std::wstring& y):a(x),b(y){} };
struct Compare {
template<typename T>
            requires (std::integral<T> || requires(T value) {
                { value.Equals(value) } -> std::convertible_to<bool>;
                { value.ToString() } -> std::convertible_to<std::wstring>;
            })
        static void CheckField(int index, unsigned long long pc, const std::wstring &fieldName, T legacyValue, T clusterValue, std::vector<DifferentialTraceDiscrepancy> &discrepancies)
        {
            bool equal;
            if constexpr (std::integral<T>) {
                equal = legacyValue == clusterValue;
            } else {
                equal = legacyValue.Equals(clusterValue);
            }
            if (!equal) {
                auto stringify = [](const T &value) -> std::wstring {
                    if constexpr (std::same_as<T, bool>) return value ? L"True" : L"False";
                    else if constexpr (std::integral<T>) return std::to_wstring(value);
                    else return value.ToString();
                };
                discrepancies.emplace_back(index, pc, fieldName, stringify(legacyValue), stringify(clusterValue));
            }
        }
}; struct Value { int n; bool Equals(Value v) const{return n==v.n;} std::wstring ToString() const{return std::to_wstring(n);} }; int main(){ std::vector<DifferentialTraceDiscrepancy> d; Compare::CheckField(0,0,L"b",true,false,d); Compare::CheckField(0,0,L"i",4,7,d); Compare::CheckField(0,0,L"u",(unsigned char)255,(unsigned char)0,d); Compare::CheckField(0,0,L"v",Value{1},Value{2},d); Compare::CheckField(0,0,L"same",Value{2},Value{2},d); return !(d.size()==4 && d[0].a==L"True" && d[0].b==L"False" && d[1].a==L"4" && d[1].b==L"7" && d[2].a==L"255" && d[3].a==L"1" && d[3].b==L"2"); }