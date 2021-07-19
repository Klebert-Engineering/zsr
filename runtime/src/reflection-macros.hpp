/* clang-format off */

/**
 * Expose the current objects `type` member for further
 * use by `ZSERIO_REFLECT_TYPE_REF`.
 */
#define CUR_TYPE(var)                           \
    zsr::TypeRef& tr = (var).type.emplace();

#define ZSERIO_REFLECT_TYPE_REF(ZTYPE, BIT_SIZE, IS_ARRAY, PKG_NAME, NAME) \
    {                                                                      \
        tr.package = #PKG_NAME;                                            \
        tr.ident = #NAME;                                                  \
                                                                           \
        tr.ztype.type = zsr::ZType:: ZTYPE ;                               \
        tr.ztype.size = BIT_SIZE;                                          \
        tr.ztype.array = IS_ARRAY;                                         \
    }

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

#define ZSERIO_REFLECT_PACKAGE_BEGIN(NAME, SNAME, NS)    \
    {                                                    \
    namespace PkgNamespace = :: NS;                      \
                                                         \
    zsr::Package& p = pkgs.emplace_back();               \
    p.ident = #NAME;

#define ZSERIO_REFLECT_PACKAGE_END()            \
    }

/**
 * Subtype
 *
 * Structure:
 *   SUBTYPE_BEGIN
 *   SUBTYPE_END
 */

#define ZSERIO_REFLECT_SUBTYPE_BEGIN(NAME, PACKAGE, TYPE)  \
    {                                                      \
        using SubTypeType =                                \
            std::decay_t<                                  \
                std::remove_const_t<                       \
                    PkgNamespace:: NAME>>;                 \
                                                           \
        zsr::SubType& t = p.subTypes.emplace_back(&p);     \
        t.ident = #NAME;                                   \
                                                           \
        CUR_TYPE(t);                                       \
        tr.package = #PACKAGE;                             \
        tr.ident = #TYPE;                                  \
        zsr::CTypeTraits< SubTypeType >::set(tr.ctype);

#define ZSERIO_REFLECT_SUBTYPE_END() \
    }

/**
 * Constant
 *
 * Structure:
 *   CONST_BEGIN
 *     TYPE_REF?
 *   CONST_END
 */

#define ZSERIO_REFLECT_CONST_BEGIN(NAME)                 \
    {                                                    \
        using ConstType =                                \
            std::decay_t<                                \
                std::remove_const_t<                     \
                    decltype(PkgNamespace:: NAME)>>;     \
                                                         \
        zsr::Constant& c = p.constants.emplace_back(&p); \
        c.ident = #NAME;                                 \
        c.value.set(PkgNamespace:: NAME);                \
                                                         \
        CUR_TYPE(c);                                     \
        zsr::CTypeTraits<ConstType>::set(tr.ctype);

#define ZSERIO_REFLECT_CONST_END()       \
    }


/**
 * Stucture/Choice/Union
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
 *       STRUCTURE_CHOICE_CASE*
 *       STRUCTURE_CHOICE_DEFAULT?
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

#define ZSERIO_REFLECT_STRUCTURE_NO_PARAMETERS()                   \
    s.read = [s = &s](::zserio::BitStreamReader& r,                \
                      zsr::ParameterList l)                        \
        -> zsr::Introspectable                                     \
    {                                                              \
        return zsr::Introspectable{                                \
            s,                                                     \
            zsr::impl::readUniqueInstance<CompoundType>(r)         \
        };                                                         \
    };                                                             \

#define ZSERIO_REFLECT_STRUCTURE_BEGIN(NAME, TYPE)                   \
    {                                                                \
        using CompoundType = PkgNamespace::NAME;                     \
                                                                     \
        zsr::Compound& s = p.compounds.emplace_back(&p);             \
        t2c[std::type_index(typeid(CompoundType))] =                 \
            &s;                                                      \
                                                                     \
        s.ident = #NAME;                                             \
        s.type = zsr::Compound::Type::TYPE;                          \
                                                                     \
        s.alloc = GEN_STRUCTURE_ALLOC();                             \
                                                                     \
        s.compare = [&](const zsr::Introspectable& a,                \
                        const zsr::Introspectable& b) {              \
            return zsr::introspectable_cast<CompoundType>(a, t2c) == \
                   zsr::introspectable_cast<CompoundType>(b, t2c);   \
        };                                                           \
                                                                     \
        s.hash = [&](const zsr::Introspectable& i) -> int {          \
            return zsr::introspectable_cast<CompoundType>(i, t2c).   \
                hashCode();                                          \
        };                                                           \
                                                                     \
        s.bitSize = [&](const zsr::Introspectable& i) -> size_t {    \
            return zsr::introspectable_cast<CompoundType>(i, t2c).   \
                bitSizeOf();                                         \
        };                                                           \
                                                                     \
        s.write = [&](zsr::Introspectable& i,                        \
                      ::zserio::BitStreamWriter& w) {                \
            return zsr::introspectable_cast<CompoundType>(i, t2c).   \
                write(w);                                            \
        };

#define ZSERIO_REFLECT_STRUCTURE_END() \
    }

#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_BEGIN()       \
    using ParameterTupleType =                            \
        zsr::parameterlist::add_shared_ptr_if_compound_t< \
            zsr::argument_tuple_t<                        \
                decltype(&CompoundType::initialize)>>;

#define GEN_INIT_PARAMETER_LIST_SET(IDX)                                \
    [&](zsr::ParameterList& l, zsr::Variant v) {                        \
        if (!l.list.has_value()) {                                      \
            l.list = ParameterTupleType{}; /* Empty init unset lists */ \
        }                                                               \
                                                                        \
        auto* tuple = std::any_cast<ParameterTupleType>(&l.list);       \
        if (tuple) {                                                    \
            zsr::parameterlist::set_parameter<IDX>(*tuple,              \
                                                   v,                   \
                                                   t2c);                \
        } else {                                                        \
            throw zsr::ParameterListTypeError::listTypeMissmatch(       \
                typeid(ParameterTupleType).name(),                      \
                l.list.type().name());                                  \
        }                                                               \
    };

#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_PARAMETER_BEGIN(IDX, NAME, GETTER) \
    {                                                                          \
        using MemberType =                                                     \
            std::decay_t<                                                      \
                std::remove_reference_t<                                       \
                    decltype(std::declval<CompoundType>(). GETTER ())>>;       \
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
        zsr::Parameter& param = s.parameters.emplace_back(&s);                 \
        param.ident = #NAME;                                                   \
                                                                               \
        param.set = GEN_INIT_PARAMETER_LIST_SET(IDX);                          \
                                                                               \
        zsr::Field& f = s.fields.emplace_back(&s);                             \
        param.field = &f;                                                      \
        f.ident = #NAME;                                                       \
                                                                               \
        f.get = Helper::getFun<CompoundType>(fieldGetter, &f, t2c);            \
        f.set = {}; /* Read-only */                                            \
                                                                               \
        CUR_TYPE(f);                                                           \
        zsr::CTypeTraits<                                                      \
            zsr::parameterlist::remove_shared_ptr_t<                           \
                std::tuple_element_t<IDX, ParameterTupleType>>>::set(tr.ctype);\
        param.type = &tr;


#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_PARAMETER_END()        \
    }

#define ZSERIO_REFLECT_STRUCTURE_INITIALIZE_END()                       \
    s.initialize = [&](zsr::Introspectable& i,                          \
                       zsr::ParameterList l)                            \
    {                                                                   \
        if (!l.list.has_value())                                        \
            throw zsr::ParameterListTypeError::listEmpty();             \
                                                                        \
        if (!std::any_cast<ParameterTupleType>(&l.list))                \
            throw zsr::ParameterListTypeError::listTypeMissmatch(       \
                typeid(ParameterTupleType).name(),                      \
                l.list.type().name());                                  \
                                                                        \
        /* Introspectable keeps alive parameters */                     \
        auto& params = i.obj->parameters = l.list;                      \
                                                                        \
        auto args = std::any_cast<ParameterTupleType>(&params);         \
        if (args) {                                                     \
            std::apply([&i, &t2c](auto&&... vals) {                     \
                zsr::introspectable_cast<CompoundType>(i, t2c)          \
                    .initialize(                                        \
                        zsr::parameterlist::deref_if_shared(vals)...);  \
            }, *args);                                                  \
        }                                                               \
    };                                                                  \
                                                                        \
    s.read = [s = &s](::zserio::BitStreamReader& r,                     \
                      zsr::ParameterList l)                             \
        -> zsr::Introspectable                                          \
    {                                                                   \
        if (!l.list.has_value())                                        \
            throw zsr::ParameterListTypeError::listEmpty();             \
                                                                        \
        auto args = std::any_cast<ParameterTupleType>(&l.list);         \
        if (args) {                                                     \
            auto instance = std::apply([s, &r](auto&&... vals) {        \
                return zsr::Introspectable{                             \
                    s,                                                  \
                    zsr::impl::readUniqueInstance<CompoundType>(        \
                        r,                                              \
                        zsr::parameterlist::deref_if_shared(vals)...    \
                    )                                                   \
                };                                                      \
            }, *args);                                                  \
                                                                        \
            /* Introspectable keeps alive parameters */                 \
            instance.obj->parameters = l.list;                          \
                                                                        \
            return instance;                                            \
        }                                                               \
                                                                        \
        throw zsr::ParameterListTypeError::listTypeMissmatch(           \
            typeid(ParameterTupleType).name(),                          \
            l.list.type().name());                                      \
    };

#define ZSERIO_REFLECT_STRUCTURE_CHILD_INITIALIZATION()                 \
    s.initializeChildren = [&](zsr::Introspectable& i)                  \
    {                                                                   \
        zsr::introspectable_cast<CompoundType>(i, t2c)                  \
            .initializeChildren();                                      \
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
    f.get = Helper::getFun<CompoundType>(getter, &f, t2c);              \
    f.set = Helper::setFun<CompoundType>(setter, &f, t2c);

#define ZSERIO_REFLECT_STRUCTURE_FIELD_OPTIONAL(HASFUN, RESETFUN) \
    f.has = [&](const zsr::Introspectable& i) -> bool             \
    {                                                             \
        return zsr::introspectable_cast<CompoundType>(i, t2c)     \
            .HASFUN();                                            \
    };                                                            \
                                                                  \
    f.reset = [&](zsr::Introspectable& i) -> void                 \
    {                                                             \
        zsr::introspectable_cast<CompoundType>(i, t2c)            \
            .RESETFUN();                                          \
    };

#define ZSERIO_REFLECT_STRUCTURE_FIELD_BEGIN(NAME, GETTER, SETTER)      \
    {                                                                   \
        using MemberType =                                              \
            std::decay_t<                                               \
                std::remove_reference_t<                                \
                    decltype(std::declval<CompoundType>(). GETTER ())>>;\
                                                                        \
        zsr::Field& f = s.fields.emplace_back(&s);                      \
        f.ident = #NAME;                                                \
                                                                        \
        GEN_FIELD_ACCESSORS(GETTER, SETTER)                             \
                                                                        \
        CUR_TYPE(f);                                                    \
        zsr::CTypeTraits<MemberType>::set(tr.ctype);

#define ZSERIO_REFLECT_STRUCTURE_FIELD_END() \
    }

#define ZSERIO_REFLECT_STRUCTURE_FUNCTION_BEGIN(NAME, FUNNAME)            \
    {                                                                     \
        using ReturnType =                                                \
            std::decay_t<                                                 \
                std::remove_reference_t<                                  \
                    decltype(std::declval<CompoundType>(). FUNNAME ())>>; \
                                                                          \
        zsr::Function& f = s.functions.emplace_back(&s);                  \
        f.ident = #NAME;                                                  \
                                                                          \
        f.call = [&](const zsr::Introspectable& i) -> zsr::Variant {      \
            return zsr::variant_helper<ReturnType>::pack(                 \
                zsr::introspectable_cast<CompoundType>(i, t2c)            \
                    . FUNNAME (),                                         \
                t2c                                                       \
            );                                                            \
        };                                                                \
                                                                          \
        CUR_TYPE(f);                                                      \
        zsr::CTypeTraits<ReturnType>::set(tr.ctype);

#define ZSERIO_REFLECT_STRUCTURE_FUNCTION_END() \
    }

/**
 * Enumeration
 *
 * Structure:
 *   ENUMERATION_BEGIN
 *     ENUMERATION_ITEM
 *   ENUMERATION_END
 */

#define ZSERIO_REFLECT_ENUMERATION_BEGIN(NAME)                 \
    {                                                          \
        using EnumType =                                       \
            PkgNamespace:: NAME ;                              \
                                                               \
        zsr::Enumeration& e = p.enumerations.emplace_back(&p); \
        e.ident = #NAME;

#define ZSERIO_REFLECT_ENUMERATION_END() \
    }

#define ZSERIO_REFLECT_ENUMERATION_ITEM(NAME)                \
    {                                                        \
        zsr::EnumerationItem& ei = e.items.emplace_back(&e); \
        ei.ident = #NAME;                                    \
        ei.value.set(EnumType:: NAME);                       \
    }

/**
 * Bitmask
 *
 * Structure:
 *   BITMASK_BEGIN
 *     BITMASK_VALUE
 *   BITMASK_END
 */

#define ZSERIO_REFLECT_BITMASK_BEGIN(NAME)             \
    {                                                  \
        using BitmaskType =                            \
            PkgNamespace:: NAME ;                      \
                                                       \
        zsr::Bitmask& b = p.bitmasks.emplace_back(&p); \
        b.ident = #NAME;

#define ZSERIO_REFLECT_BITMASK_END() \
    }

#define ZSERIO_REFLECT_BITMASK_VALUE(NAME)                 \
    {                                                      \
        zsr::BitmaskValue& bv = b.values.emplace_back(&b); \
        bv.ident = #NAME;                                  \
        bv.value.set(BitmaskType::Values:: NAME);          \
    }

/**
 * Service
 *
 * Structure:
 *   SERVICE_BEGIN
 *     SERVICE_METHOD_BEGIN*
 *       SERVICE_METHOD_REQUEST_BEGIN
 *       SERVICE_METHOD_REQUEST_END
 *       SERVICE_METHOD_RESPONSE_BEGIN
 *       SERVICE_METHOD_RESPONSE_END
 *     SERVICE_METHOD_END
 *   SERVICE_END
 */

#define ZSERIO_REFLECT_SERVICE_BEGIN(NAME)                  \
    {                                                       \
        namespace ServiceNamespace = PkgNamespace :: NAME ; \
                                                            \
        auto& s = p.services.emplace_back(&p);              \
        s.ident = #NAME;

#define ZSERIO_REFLECT_SERVICE_END()            \
    }

#define ZSERIO_REFLECT_SERVICE_METHOD_BEGIN(NAME, IDENT)                \
    {                                                                   \
        auto& sm = s.methods.emplace_back(&s);                          \
        sm.ident = #NAME;                                               \
                                                                        \
        using ArgTupleType = zsr::argument_tuple_t<                     \
            decltype(&ServiceNamespace::Client:: IDENT)>;               \
        using RequestType = std::tuple_element_t<                       \
            0u, ArgTupleType>;                                          \
        using ResponseType = zsr::member_function_traits<               \
            decltype(&ServiceNamespace::Client:: IDENT)>::return_type;  \
                                                                        \
        sm.call = [&t2c, &s, &sm](::zserio::IService& service,          \
                                  const zsr::Variant& request)          \
            -> zsr::Variant                                             \
        {                                                               \
            ServiceNamespace::Client client(service);                   \
            zsr::ServiceMethod::Context ctx{s, sm, request};            \
                                                                        \
            /* Temporary copy because of zserio requiring non-const */  \
            /* ref request object. */                                   \
            auto unpackedRq = zsr::variant_helper<RequestType>::unpack( \
                request, t2c);                                          \
            auto response = client. IDENT (unpackedRq,                  \
                                           &ctx);                       \
                                                                        \
            return zsr::variant_helper<ResponseType>::pack(             \
                std::move(response), t2c);                              \
        };

#define ZSERIO_REFLECT_SERVICE_METHOD_END()            \
    }

#define ZSERIO_REFLECT_SERVICE_METHOD_REQUEST_BEGIN()   \
    {                                                   \
        auto& tr = sm.requestType;                      \
        zsr::CTypeTraits<RequestType>::set(tr.ctype);

#define ZSERIO_REFLECT_SERVICE_METHOD_REQUEST_END()    \
    }

#define ZSERIO_REFLECT_SERVICE_METHOD_RESPONSE_BEGIN() \
    {                                                  \
        auto& tr = sm.responseType;                    \
        zsr::CTypeTraits<ResponseType>::set(tr.ctype);

#define ZSERIO_REFLECT_SERVICE_METHOD_RESPONSE_END()   \
    }

/* clang-format on */
