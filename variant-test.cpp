#include "zsr/variant.hpp"

#include <iostream>

using namespace zsr;

template <class _T>
std::ostream& operator<<(std::ostream& o, const std::vector<_T>& v)
{
    std::for_each(v.begin(), v.end(), [&, i = 0](const auto& v) mutable {
        if (i++ > 0)
            o << ", ";
        o << v;
    });
    return o;
}

int main(int argc, char** argv) {
    Variant var1(std::bitset<21>(0));

    Variant var(std::vector<uint8_t>{1, 2, 3, 4, 5});

    if (auto v = var.get<int>())
        std::cout << "As sint = " << *v << "\n";
    if (auto v = var.get<std::vector<int>>())
        std::cout << "As sint[] = " << *v << "\n";
    if (auto v = var.get<unsigned>())
        std::cout << "As uint = " << *v << "\n";
    if (auto v = var.get<std::bitset<32>>())
        std::cout << "As bit32 = " << *v << "\n";
    if (auto v = var.get<std::vector<unsigned>>())
        std::cout << "As uint[] = " << *v << "\n";
    if (auto v = var.get<std::vector<std::bitset<32>>>())
        std::cout << "As bit32[] = " << *v << "\n";
    if (auto v = var.get<std::string>())
        std::cout << "As string = " << *v << "\n";
    if (auto v = var.get<std::vector<std::string>>())
        std::cout << "As string[] = " << *v << "\n";

    return 0;
}
