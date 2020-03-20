#include "zsr/types.hpp"

#include <iostream>
#include <algorithm>

template <class Collection>
static std::ostream& join_list(std::ostream& s, const Collection& c)
{
    s << "[";
    std::for_each(c.begin(), c.end(),
                  [&, i = 0](const auto &t) mutable {
                      if (i++ != 0)
                          s << ", ";
                      s << "{" << *t << "}";
                  });
    return s << "]";
}

static void print_variant(std::ostream&, const zsr::Variant&, int = 0);

static std::ostream& operator<<(std::ostream& s, const zsr::TypeRef& t)
{
    return s << "package: " << t.package << " ident: " << t.ident
             << " z-type: {" << (int)t.ztype.type << " is-array: " << (int)t.ztype.array << "}";
}

static std::ostream& operator<<(std::ostream& s, const zsr::SubType& t)
{
    return s << "ident: " << t.ident;
}

static std::ostream& operator<<(std::ostream& s, const zsr::Constant& c)
{
    s << "ident: " << c.ident << ", value: ";
    print_variant(s, c.value);

    s << " type: {" << *c.type << "}";

    return s;
}

static std::ostream& operator<<(std::ostream& s, const zsr::Field& f)
{
    s << "ident: " << f.ident
      << " type: {" << *f.type << "}";
    return s;
}

static std::ostream& operator<<(std::ostream& s, const zsr::Parameter& p)
{
    return s << "ident: " << p.ident;
}

static std::ostream& operator<<(std::ostream& s, const zsr::Function& f)
{
    s << "ident: " << f.ident
      << " type: {" << *f.type << "}";
    return s;
}

static std::ostream& operator<<(std::ostream& s, const zsr::Compound& t)
{
    s << "ident: " << t.ident;
    if (!t.parameters.empty())
        join_list(s << ", parameters: ", t.parameters);
    if (!t.fields.empty())
        join_list(s << ", fields: ", t.fields);
    if (!t.functions.empty())
        join_list(s << ", functions: ", t.functions);
    return s;
}

static std::ostream& operator<<(std::ostream& s, const zsr::Package& p)
{
    s << "ident: " << p.ident;
    if (!p.subTypes.empty())
        join_list(s << ", subtypes: ", p.subTypes);
    if (!p.constants.empty())
        join_list(s << ", constants: ", p.constants);
    if (!p.compounds.empty())
        join_list(s << ", compounds: ", p.compounds);

    return s;
}

static void print_variant(std::ostream& s, const zsr::Variant& val, int indent)
{
    std::string prefix;
    for (auto i = 0; i < indent; ++i)
       prefix += "  ";

    if (auto v = val.get<int64_t>())
        s << prefix << *v << " [s]";
    if (auto v = val.get<uint64_t>())
        s << prefix << *v << " [u]";
    if (auto v = val.get<double>())
        s << prefix << *v << " [f]";
    if (auto v = val.get<std::string>())
        s << prefix << *v << " [str]";
    if (auto v = val.get<zsr::Introspectable>()) {
        if (!v->meta) {
            s << "[unknown-structure]\n";
            return;
        }
       
        s << prefix << "[obj] " << v->meta->ident << "\n";

        if (!v->meta->parameters.empty()) {
            s << prefix << "parameters:\n";
            for (auto p : v->meta->parameters) {
                s << prefix << "  - " << p->ident << "\n";
                //print_variant(s << prefix << "  - " << f->ident << ":\n", f->get(*v), indent + 2);
                //s << "\n";
            }
        }

        if (!v->meta->fields.empty()) {
            s << prefix << "fields:\n";
            for (auto f : v->meta->fields) {
                print_variant(s << prefix << "  - " << f->ident << ":\n", f->get(*v), indent + 2);
                s << "\n";
            }
        }

        if (!v->meta->functions.empty()) {
            s << prefix << "functions:\n";
            for (auto f : v->meta->functions) {
                print_variant(s << prefix << "  - " << f->ident << ":\n", f->call(*v), indent + 2);
                s << "\n";
            }
        }
    }
    if (auto v = val.get<std::vector<zsr::Introspectable>>()) {
        auto idx = 0;
        for (auto i : *v) {
            s << prefix << "  - " << idx++ << "\n";
            print_variant(s, i, indent + 2);
        }
    }
}

static auto findStructure(const std::string& package, const std::string& ident) -> const zsr::Compound*
{
    const auto& p = zsr::registry().packages;
    auto p_iter = std::find_if(p.begin(), p.end(), [&](const auto& p){ return p->ident == package; });
    if (p_iter != p.end()) {
        auto& s = (*p_iter)->compounds;
        auto s_iter = std::find_if(s.begin(), s.end(), [&](const auto& s){ return s->ident == ident; });
        if (s_iter != s.end()) {
            return *s_iter;
        }
    }
    return nullptr;
}

static auto findField(const zsr::Compound* s, const std::string& ident) -> const zsr::Field*
{
    auto iter = std::find_if(s->fields.begin(), s->fields.end(),
                             [&](auto field) {
                                 return field->ident == ident;
                             });
    if (iter != s->fields.end()) {
        return *iter;
    }
    return nullptr;
}

static auto findFunction(const zsr::Compound* s, const std::string& ident) -> const zsr::Function*
{
    auto iter = std::find_if(s->functions.begin(), s->functions.end(),
                             [&](auto function) {
                                 return function->ident == ident;
                             });
    if (iter != s->functions.end()) {
        return *iter;
    }
    return nullptr;
}

static auto ask(std::string prompt) -> std::string
{
    std::cout << prompt << "> " << std::flush;
    std::string val;
    std::getline(std::cin, val);
    return val;
}

static auto set_variant(const std::string& type, const std::string& val) -> zsr::Variant
{
    if (type == "str")  return {val};
    if (type == "bool") return {(bool)std::atoi(val.c_str())};
    if (type == "u")    return {(uint64_t)std::atoi(val.c_str())};
    if (type == "s")    return {(int64_t)std::atoi(val.c_str())};
    if (type == "f")    return {(double)std::atof(val.c_str())};

    std::cout << "Unknown type.\n";
    return {};
}

int main(int, const char**) {
    for (const auto& p : zsr::registry().packages) {
        std::cout << "package: {" << *p << "}\n";
    }

    std::cout << "---\n";

    std::optional<zsr::Introspectable> object;
    for (;;) {
        auto cmd = ask("command");
        if (cmd == "?") {
            std::cout << "Commands:\n"
                      << "  create\n"
                      << "  set\n"
                      << "  get\n"
                      << "  call\n"
                      << "  print\n"
                      << "  list\n"
                      << "\n";
        }

        if (cmd == "l" || cmd == "list") {
            auto what = ask("what");
            if (what == "p" || what == "package") {
                for (const auto& p : zsr::registry().packages)
                    std::cout << "  - " << p->ident << "\n";
            }
            else if (what == "s" || what == "structure") {
                for (const auto& p : zsr::registry().packages) {
                    std::cout << "  - " << p->ident << "\n";
                    for (const auto& s : p->compounds) {
                        std::cout << "    - " << s->ident << "\n";
                    }
                }
            } else if (what == "e" || what == "enumeration") {
                for (const auto& p : zsr::registry().packages) {
                    if (!p->enumerations.empty())
                        std::cout << "  - " << p->ident << "\n";

                    for (const auto& e : p->enumerations) {
                        std::cout << "    - " << e->ident << "\n";
                        for (const auto& ei : e->items) {
                            std::cout << "      - " << ei->ident << " = ";
                            print_variant(std::cout, ei->value);
                            std::cout << "\n";
                        }
                    }
                }
            } else if (what == "c" || what == "constant") {
                for (const auto& p : zsr::registry().packages) {
                    if (!p->constants.empty())
                        std::cout << "  - " << p->ident << "\n";

                    for (const auto& c : p->constants) {
                        std::cout << "    - " << c->ident << " = ";
                        print_variant(std::cout, c->value);
                        std::cout << "\n";
                    }
                }
            } else {
                std::cout << "Options:\n"
                          << "  - p|package\n"
                          << "  - s|structure\n"
                          << "  - e|enumeration\n"
                          << "  - c|constant\n";
            }
        }

        if (cmd == "create") {
            auto pkg = ask("package");
            auto str = ask("structure");

            if (auto* structure = findStructure(pkg, str)) {
                object = structure->alloc();
                if (structure->initialize) {
                    zsr::ParameterList params;
                    for (auto parameter : structure->parameters) {
                        auto typ = ask(std::string("type [") + parameter->ident + "] (std, bool, s, u, f)");
                        auto val = ask(std::string("value [") + parameter->ident + "]");

                        parameter->set(params, set_variant(typ, val));
                    }

                    try {
                        structure->initialize(*object, std::move(params));
                    } catch (const std::bad_any_cast& e) {
                        std::cout << "Type error.\n" << e.what() << "\n";
                        object.reset();
                    }
                }
            } else {
                std::cout << "Could not find '" << pkg << str << "'.\n";
            }
        }

        if (cmd == "get") {
            if (object) {
                auto ident = ask("field");
                auto field = findField(object->meta, ident);
                if (field) {
                    std::cout << "  = ";
                    print_variant(std::cout, field->get(*object));
                    std::cout << "\n";
                } else {
                    std::cout << "Could not find field '" << ident << "\n";
                }
            } else {
                std::cout << "No object. Call create first.\n";
            }
        }

        if (cmd == "set") {
            if (object) {
                auto ident = ask("field");
                auto field = findField(object->meta, ident);
                if (field) {
                    auto typ = ask("type (str, bool, u, s, f)");
                    auto val = ask("value");

                    field->set(*object, set_variant(typ, val));
                } else {
                    std::cout << "Could not find field '" << ident << "\n";
                }
            } else {
                std::cout << "No object. Call create first.\n";
            }
        }

        if (cmd == "call") {
            if (object) {
                auto ident = ask("function");
                auto function = findFunction(object->meta, ident);
                if (function) {
                    std::cout << "  = ";
                    print_variant(std::cout, function->call(*object));
                    std::cout << "\n";
                } else {
                    std::cout << "Could not find function." << ident << "\n";
                }
            } else {
                std::cout << "No object. Call create first.\n";
            }
        }

        if (cmd == "print") {
            if (object) {
                print_variant(std::cout, {*object});
            } else {
                std::cout << "No object.\n";
            }
        }
    }

    return 0;
}
