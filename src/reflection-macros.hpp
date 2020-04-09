
#define ZSERIO_REFLECT_IS_COMPOUND_TRAIT(NAME, NS)          \
    namespace zsr {                                         \
        /* Note: Base template defined in lib-prefix.cpp */ \
                                                            \
        template <>                                         \
        struct is_compound<:: NS :: NAME>                   \
            : std::true_type {};                            \
                                                            \
        template <>                                         \
        struct meta_for_compound<:: NS :: NAME>             \
        {                                                   \
            static Compound const* ptr;                     \
        };                                                  \
                                                            \
        Compound const* meta_for_compound<:: NS :: NAME>    \
            ::ptr{nullptr};                                 \
    }

/**
 * Expose the current objects `type` member for further
 * use by `ZSERIO_REFLECT_TYPE_REF`.
 */
#define CUR_TYPE(var)                           \
    static zsr::TypeRef tr;                     \
    var.type = &tr;

#define ZSERIO_REFLECT_TYPE_REF(ZTYPE, BIT_SIZE, IS_ARRAY, PKG_NAME, NAME) \
    {                                                                      \
        tr.package = #PKG_NAME;                                            \
        tr.ident = #NAME;                                                  \
                                                                           \
        tr.ztype.type = zsr::ZType:: ZTYPE ;                               \
        tr.ztype.size = BIT_SIZE;                                          \
        tr.ztype.array = IS_ARRAY;                                         \
    }


/* C does not expand symbols used with # or ##
 * so we need to use this common workaround. */
#define UID2(prefix, x) prefix ## x
#define UID(prefix, x) UID2(prefix, x)

/**
 * Package
 *
 * Structure:
 *   PACKAGE_BEGIN
 *     Subtype*
 *     Constant*
 *     Compound*
 *     Enumeration*
 *     Bitmask*
 *   PACKAGE_END
 */

#define ZSERIO_REFLECT_PACKAGE_BEGIN(NAME, NS)  \
    namespace NS {}                             \
                                                \
    static const bool UID(init, __COUNTER__) =  \
    []() {                                      \
        namespace PkgNamespace = :: NS;         \
                                                \
        static zsr::Package p;                  \
        zsr::registry().packages.push_back(&p); \
                                                \
        p.ident = #NAME;

#define ZSERIO_REFLECT_PACKAGE_END() \
        return true;                 \
    }();

/**
 * Subtype
 *
 * Structure:
 *   SUBTYPE_BEGIN
 *   SUBTYPE_END
 */

#define ZSERIO_REFLECT_SUBTYPE_BEGIN(NAME, PACKAGE, TYPE) \
    {                                                     \
        static zsr::SubType t;                            \
        t.ident = #NAME;                                  \
                                                          \
        CUR_TYPE(t);                                      \
        tr.package = #PACKAGE;                            \
        tr.ident = #TYPE;

#define ZSERIO_REFLECT_SUBTYPE_END() \
        p.subTypes.push_back(&t);    \
    }

/**
 * Constant
 *
 * Structure:
 *   CONST_BEGIN
 *     TYPE_REF?
 *   CONST_END
 */

#define ZSERIO_REFLECT_CONST_BEGIN(NAME)             \
    {                                                \
        using ConstType =                            \
            std::decay_t<                            \
                std::remove_const_t<                 \
                    decltype(PkgNamespace:: NAME)>>; \
                                                     \
        static zsr::Constant c;                      \
        c.ident = #NAME;                             \
        c.type = nullptr;                            \
        c.value.set(PkgNamespace:: NAME);            \
                                                     \
        CUR_TYPE(c);                                 \
        zsr::CTypeTraits<ConstType>::set(tr.ctype);

#define ZSERIO_REFLECT_CONST_END()       \
        p.constants.push_back(&c);       \
    }


/**
 * Stucture/Choice
 *
 * Structure:
 *   STRUCTURE_BEGIN
 *     STRUCTURE_INITIALIZE_BEGIN?
 *       STRUCTURE_INITIALIZE_PARAMETER_BEGIN*
 *         TYPE_REF?
 *       STRUCTURE_INITIALIZE_PARAMETER_END
 *     STRUCTURE_INITIALIZE_END
 *     STRUCTURE_CHILD_INITIALIZATION?
 *     STRUCTURE_FIELD_BEGIN*
 *       TYPE_REF?
 *       STRUCTURE_FIELD_OPTIONAL?
 *     STRUCTURE_FIELD_END
 *     STRUCTURE_FUNCTION_BEGIN*
 *     STRUCTURE_FUNCTION_END
 *     STRUCTURE_CHOICE_BEGIN?
 *       STRUCTURE_CHOICE_CASE_BEGIN*
 *       STRUCTURE_CHOICE_CASE_END
 *       STRUCTURE_CHOICE_DEFAULT_BEGIN?
 *       STRUCTURE_CHOICE_DEFAULT_END
 *     STRUCTURE_CHOICE_END
 *   STRUCTURE_END
 */

#define GEN_STRUCTURE_ALLOC()                             \
    [s = &s]() -> zsr::Introspectable {                   \
        return zsr::Introspectable{                       \
            s,                                            \
            zsr::impl::makeUniqueInstance<CompoundType>() \
        };                                                \
    };

#define ZSERIO_REFLECT_STRUCTURE_BEGIN(name)                    \
    {                                                           \
        using CompoundType = PkgNamespace::name;                \
                                                                \
        static zsr::Compound s;                                 \
        zsr::meta_for_compound<CompoundType>::ptr = &s;         \
        s.ident = #name;                                        \
                                                                \
        s.alloc = GEN_STRUCTURE_ALLOC();                        \
                                                                \
        s.compare = [](const zsr::Introspectable& a,            \
                       const zsr::Introspectable& b) {          \
            return zsr::introspectable_cast<CompoundType>(a) == \
                   zsr::introspectable_cast<CompoundType>(b);   \
        };                                                      \
                                                                \
        s.read = [](zsr::Introspectable& i,                     \
                    ::zserio::BitStreamReader& r) {             \
            return zsr::introspectable_cast<CompoundType>(i).   \
                read(r);                                        \
        };                                                      \
                                                                \
        s.write = [](zsr::Introspectable& i,                    \
                     ::zserio::BitStreamWriter& w) {            \
            return zsr::introspectable_cast<CompoundType>(i).   \
                write(w);                                       \
        };

#define ZSERIO_REFLECT_STRUCTURE_END() \
        p.compounds.push_back(&s);     \
    }

#define ZSERIO_REFLECT_STRUCTURE_CHOICE_BEGIN()
#define ZSERIO_REFLECT_STRUCTURE_CHOICE_END()

/* TODO: Add more metadata to choice cases */
#define ZSERIO_REFLECT_STRUCTURE_CHOICE_CASE_BEGIN(FIELD_NAME) \
    {                                                          \
        static zsr::ChoiceCase cc;                             \
        cc.field = zsr::find<zsr::Field>(&s, #FIELD_NAME);

#define ZSERIO_REFLECT_STRUCTURE_CHOICE_CASE_END() \
        s.cases.push_back(&cc);                    \
    }

/* TODO: Distinguish between choice-case and choice-default */
#define ZSERIO_REFLECT_STRUCTURE_CHOICE_DEFAULT_BEGIN(FIELD_NAME) \
    ZSERIO_REFLECT_STRUCTURE_CHOICE_CASE_BEGIN(FIELD_NAME)

#define ZSERIO_REFLECT_STRUCTURE_CHOICE_DEFAULT_END() \
    ZSERIO_REFLECT_STRUCTURE_CHOICE_CASE_END()

#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_BEGIN()       \
    using ParameterTupleType =                            \
        zsr::parameterlist::add_shared_ptr_if_compound_t< \
            zsr::argument_tuple_t<                        \
                decltype(&CompoundType::initialize)>>;

#define GEN_INIT_PARAMETER_LIST_SET(IDX)                                \
    [](zsr::ParameterList& l, zsr::Variant v) {                         \
        if (!l.list.has_value()) {                                      \
            l.list = ParameterTupleType{}; /* Empty init unset lists */ \
        }                                                               \
                                                                        \
        auto* tuple = std::any_cast<ParameterTupleType>(&l.list);       \
        if (tuple) {                                                    \
            zsr::parameterlist::set_parameter<IDX>(*tuple, v);          \
        } else {                                                        \
            /* TODO: Throw! */                                          \
        }                                                               \
    };

#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_PARAMETER_BEGIN(IDX, NAME, GETTER) \
    {                                                                          \
        using MemberType =                                                     \
            std::decay_t<                                                      \
                std::remove_reference_t<                                       \
                    decltype(((CompoundType*)0)-> GETTER ())>>;                \
                                                                               \
        constexpr bool IsStruct =                                              \
            zsr::is_compound<                                                  \
                zsr::remove_vector_t<MemberType>>::value;                      \
                                                                               \
        using Helper =                                                         \
            zsr::GenFieldAccessorHelper<MemberType>;                           \
                                                                               \
        auto fieldGetter = [](const auto& obj)                                 \
            -> std::conditional_t<IsStruct, const MemberType&, MemberType>     \
        {                                                                      \
            return obj.GETTER();                                               \
        };                                                                     \
                                                                               \
        static zsr::Parameter p;                                               \
        p.ident = #NAME;                                                       \
        p.type = nullptr;                                                      \
                                                                               \
        p.set = GEN_INIT_PARAMETER_LIST_SET(IDX);                              \
                                                                               \
        static zsr::Field f;                                                   \
        f.ident = #NAME;                                                       \
        f.type = nullptr;                                                      \
                                                                               \
        f.get = Helper::getFun<CompoundType>(fieldGetter, &f);                 \
        f.set = {}; /* Read-only */                                            \
                                                                               \
        CUR_TYPE(p);                                                           \
        zsr::CTypeTraits<                                                      \
            zsr::parameterlist::remove_shared_ptr_t<                           \
                std::tuple_element_t<IDX, ParameterTupleType>>>::set(tr.ctype);\
        f.type = &tr; /* parameter & field share type pointer */


#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_PARAMETER_END()        \
        s.parameters.push_back(&p);                                \
        s.fields.push_back(&f);                                    \
    }

#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_END()                       \
    s.initialize = [s = &s](zsr::Introspectable& i,                     \
                            zsr::ParameterList l)                       \
    {                                                                   \
        if (!std::any_cast<ParameterTupleType>(&l.list))                \
            throw zsr::ParameterListTypeError{};                        \
                                                                        \
        /* Introspectable keeps alive parameters */                     \
        auto& params = i.obj->parameters = l.list;                      \
                                                                        \
        auto args = std::any_cast<ParameterTupleType>(&params);         \
        if (args) {                                                     \
            std::apply([&i](auto&&... vals) {                           \
                zsr::introspectable_cast<CompoundType>(i).initialize(   \
                    zsr::parameterlist::deref_if_shared(vals)...);      \
            }, *args);                                                  \
        }                                                               \
    };

#define ZSERIO_REFLECT_STRUCTURE_CHILD_INITIALIZATION()                 \
    s.initializeChildren = [](zsr::Introspectable& i)                   \
    {                                                                   \
        zsr::introspectable_cast<CompoundType>(i).initializeChildren(); \
    };

#define GEN_FIELD_ACCESSORS(GETTER, SETTER)                             \
    constexpr bool IsStruct =                                           \
        zsr::is_compound<                                               \
            zsr::remove_vector_t<MemberType>>::value;                   \
    using Helper =                                                      \
        zsr::GenFieldAccessorHelper<MemberType>;                        \
                                                                        \
    auto getter = [](const auto& obj)                                   \
        -> std::conditional_t<IsStruct, const MemberType&, MemberType>  \
    {                                                                   \
        return obj.GETTER();                                            \
    };                                                                  \
                                                                        \
    auto setter = [](auto& obj, auto&& val)                             \
        -> void                                                         \
    {                                                                   \
        return obj.SETTER(val);                                         \
    };                                                                  \
                                                                        \
    f.get = Helper::getFun<CompoundType>(getter, &f);                   \
    f.set = Helper::setFun<CompoundType>(setter, &f);

#define ZSERIO_REFLECT_STRUCTURE_FIELD_OPTIONAL(HASFUN, RESETFUN)   \
    f.has = [](const zsr::Introspectable& i) -> bool                \
    {                                                               \
        return zsr::introspectable_cast<CompoundType>(i).HASFUN();  \
    };                                                              \
                                                                    \
    f.reset = [](zsr::Introspectable& i) -> void                    \
    {                                                               \
        zsr::introspectable_cast<CompoundType>(i).RESETFUN();       \
    };

#define ZSERIO_REFLECT_STRUCTURE_FIELD_BEGIN(NAME, GETTER, SETTER)    \
    {                                                                 \
        using MemberType =                                            \
            std::decay_t<                                             \
                std::remove_reference_t<                              \
                    decltype(((CompoundType*)0)-> GETTER ())>>;       \
                                                                      \
        static zsr::Field f;                                          \
        f.ident = #NAME;                                              \
        f.type = nullptr;                                             \
                                                                      \
        GEN_FIELD_ACCESSORS(GETTER, SETTER)                           \
                                                                      \
        CUR_TYPE(f);                                                  \
        zsr::CTypeTraits<MemberType>::set(tr.ctype);

#define ZSERIO_REFLECT_STRUCTURE_FIELD_END() \
        s.fields.push_back(&f);              \
    }

#define ZSERIO_REFLECT_STRUCTURE_FUNCTION_BEGIN(NAME, FUNNAME)    \
    {                                                             \
        using ReturnType =                                        \
            std::decay_t<                                         \
                std::remove_reference_t<                          \
                    decltype(((CompoundType*)0)-> FUNNAME ())>>;  \
                                                                  \
        static zsr::Function f;                                   \
        f.ident = #NAME;                                          \
                                                                  \
        f.call = [](const zsr::Introspectable& i) -> zsr::Variant {\
            return {                                              \
                zsr::introspectable_cast<CompoundType>(i)         \
                    . FUNNAME ()                                  \
            };                                                    \
        };                                                        \
                                                                  \
        CUR_TYPE(f);                                              \
        zsr::CTypeTraits<ReturnType>::set(tr.ctype);

#define ZSERIO_REFLECT_STRUCTURE_FUNCTION_END() \
        s.functions.push_back(&f);              \
    }

/**
 * Enumeration
 *
 * Structure:
 *   ENUMERATION_BEGIN
 *     ENUMERATION_ITEM
 *   ENUMERATION_END
 */

#define ZSERIO_REFLECT_ENUMERATION_BEGIN(NAME) \
    {                                          \
        using EnumType =                       \
            PkgNamespace:: NAME ;              \
                                               \
        static zsr::Enumeration e;             \
        e.ident = #NAME;

#define ZSERIO_REFLECT_ENUMERATION_END() \
        p.enumerations.push_back(&e);    \
    }

#define ZSERIO_REFLECT_ENUMERATION_ITEM(NAME) \
    {                                         \
        static zsr::EnumerationItem ei;       \
        ei.ident = #NAME;                     \
        ei.value.set(EnumType:: NAME);        \
                                              \
        e.items.push_back(&ei);               \
    }

/**
 * Bitmask
 *
 * Structure:
 *   BITMASK_BEGIN
 *     BITMASK_VALUE
 *   BITMASK_END
 */

#define ZSERIO_REFLECT_BITMASK_BEGIN(NAME) \
    {                                      \
        using BitmaskType =                \
            PkgNamespace:: NAME ;          \
                                           \
        static zsr::Bitmask b;             \
        b.ident = #NAME;

#define ZSERIO_REFLECT_BITMASK_END() \
        p.bitmasks.push_back(&b);    \
    }

#define ZSERIO_REFLECT_BITMASK_VALUE(NAME)        \
    {                                             \
        static zsr::BitmaskValue bv;              \
        bv.ident = #NAME;                         \
        bv.value.set(BitmaskType::Values:: NAME); \
                                                  \
        b.values.push_back(&bv);                  \
    }
