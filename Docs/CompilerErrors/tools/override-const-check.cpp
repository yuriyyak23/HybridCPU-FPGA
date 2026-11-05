struct Base { virtual bool Equals(int) const = 0; };
struct Derived : Base { bool Equals(int) const override; };
