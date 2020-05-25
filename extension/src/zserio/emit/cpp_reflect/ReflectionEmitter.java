package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import zserio.emit.common.ZserioEmitException;
import zserio.emit.common.DefaultEmitter;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Collection;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;
import java.lang.Integer;


public class ReflectionEmitter extends EmitterBase
{
    /**
     * Compound types.
     *
     * Strings match zsr::Compound::Type enum items.
     */
    final String COMPOUND_TYPE_STRUCTURE = "Structure";
    final String COMPOUND_TYPE_CHOICE    = "Choice";
    final String COMPOUND_TYPE_UNION     = "Union";

    public ReflectionEmitter(Path outputDir, Parameters extensionParameters)
    {
        super(outputDir, extensionParameters);
    }
   
    @Override
    public void beginPackage(Package pkg) throws ZserioEmitException
    {
        super.beginPackage(pkg);

        packageData.namespaces.add(packageNameToNamespace(pkg.getPackageName()));

        List<String> args = Arrays.asList(new String[] {
            pkg.getPackageName().toString(),
            packageData.packageInitializer,
            packageNameToNamespace(pkg.getPackageName())
        });

        beginReflect("PACKAGE", args);
    }
    
    @Override
    public void endPackage(Package pkg) throws ZserioEmitException
    {
        endReflect("PACKAGE");

        super.endPackage(pkg);
    }

    @Override
    public void beginSubtype(Subtype subtype) throws ZserioEmitException
    {
        writeInclude(subtype.getPackage(), subtype.getName());

        final PackageName pkgName = subtype.getTypeReference().getReferencedPackageName();
        final String typeName = subtype.getTypeReference().getReferencedTypeName();

        List<String> args = Arrays.asList(new String[] {
            subtype.getName(),
            pkgName.toString(),
            typeName,
        });

        beginReflect("SUBTYPE", args);
        reflectTypeReference(subtype.getTypeReference());
        endReflect("SUBTYPE");
    }

    @Override
    public void beginConst(Constant constType) throws ZserioEmitException
    {
        writeInclude(constType.getPackage(), constType.getName());

        List<String> args = Arrays.asList(new String[] {
            constType.getName()
        });

        beginReflect("CONST", args);
        reflectTypeInstantiation(constType.getTypeInstantiation());
        endReflect("CONST");
    }

    public void reflectTypeReference(TypeReference type)
    {
        // TODO: :/
    }

    public void reflectTypeInstantiation(TypeInstantiation type)
    {
        class TypeVisitor extends ZserioAstDefaultVisitor {
            private void reflectTypeRef(String typeName, int bitSize)
            {
                List<String> args = Arrays.asList(new String[] {
                    typeName,
                    Integer.toString(bitSize),
                    "false",
                    "",
                    ""
                });

                reflect("TYPE_REF", args);
            }

            @Override
            public void visitStringType(StringType type)
            {
                reflectTypeRef("String", 0);
            }

            @Override
            public void visitBitmaskType(BitmaskType type)
            {
                reflectTypeRef("Bitmask", 0);
            }

            @Override
            public void visitStdIntegerType(StdIntegerType type)
            {
                if (type.isSigned()) {
                    reflectTypeRef("Int", type.getBitSize());
                } else {
                    reflectTypeRef("UInt", type.getBitSize());
                }
            }

            @Override
            public void visitVarIntegerType(VarIntegerType type)
            {
                if (type.isSigned()) {
                    reflectTypeRef("VarInt", type.getMaxBitSize());
                } else {
                    reflectTypeRef("VarUInt", type.getMaxBitSize());
                }
            }

            @Override
            public void visitFixedBitFieldType(FixedBitFieldType type)
            {
                if (type.isSigned()) {
                    reflectTypeRef("Bitfield", type.getBitSize());
                } else {
                    reflectTypeRef("UBitfield", type.getBitSize());
                }
            }

            @Override
            public void visitDynamicBitFieldType(DynamicBitFieldType type)
            {
                if (type.isSigned()) {
                    reflectTypeRef("VarBitfield", type.getMaxBitSize());
                } else {
                    reflectTypeRef("VarUBitfield", type.getMaxBitSize());
                }
            }

            @Override
            public void visitBooleanType(BooleanType type)
            {
                reflectTypeRef("Bool", 1);
            }

            @Override
            public void visitFloatType(FloatType type)
            {
                reflectTypeRef("Float", type.getBitSize());
            }

            @Override
            public void visitStructureType(StructureType type)
            {
                List<String> args = Arrays.asList(new String[] {
                    "Structure",
                    "0",
                    "false",
                    type.getPackage().getPackageName().toString(),
                    type.getName()
                });

                reflect("TYPE_REF", args);
            }

            @Override
            public void visitArrayType(ArrayType arrayType)
            {
                // TODO: We've got no information about the array type
                List<String> args = Arrays.asList(new String[] {
                    "",
                    //type.getPackage().getPackageName().toString(),
                    //type.getName()
                });

                //reflect("TYPE_REF", args);
            }
        }

        type.getType().accept(new TypeVisitor());
    }

    public void reflectField(Field field)
    {
        String name = field.getName();
        String accessorName = name.substring(0, 1).toUpperCase() + name.substring(1);
        String getName = "get" + accessorName;
        String setName = "set" + accessorName;
        String hasName = "has" + accessorName;
        String resetName = "reset" + accessorName;

        List<String> args = Arrays.asList(new String[] {
            name,
            getName,
            setName
        });

        beginReflect("STRUCTURE_FIELD", args);
        reflectTypeInstantiation(field.getTypeInstantiation());

        if (field.getOptionalClauseExpr() != null) {
            List<String> optArgs = Arrays.asList(new String[] {
                hasName,
                resetName
            });

            reflect("STRUCTURE_FIELD_OPTIONAL", optArgs);
        }

        endReflect("STRUCTURE_FIELD");
    }

    public void reflectParameter(Parameter parameter, int index)
    {
        String name = parameter.getName();
        String getName = "get" + name.substring(0, 1).toUpperCase() + name.substring(1);

        List<String> args = Arrays.asList(new String[] {
            Integer.toString(index),
            name,
            getName
        });

        beginReflect("STRUCTURE_INITIALIZE_PARAMETER", args);
        // TODO: Type
        endReflect("STRUCTURE_INITIALIZE_PARAMETER");
    }

    public void reflectFunction(Function parameter)
    {
        String name = parameter.getName();
        String funcName = "func" + name.substring(0, 1).toUpperCase() + name.substring(1);

        List<String> args = Arrays.asList(new String[] {
            name,
            funcName
        });

        beginReflect("STRUCTURE_FUNCTION", args);
        // TODO: Type
        endReflect("STRUCTURE_FUNCTION");
    }

    private void reflectCompoundType(CompoundType type) throws ZserioEmitException
    {
        writeInclude(type.getPackage(), type.getName());

        /* Only structures with one or more parameters have
         * an `initialze` function. */
        if (!type.getTypeParameters().isEmpty()) {
            beginReflect("STRUCTURE_INITIALIZE", null);

            int index = 0;
            for (Parameter p : type.getTypeParameters()) {
                reflectParameter(p, index++);
            }

            endReflect("STRUCTURE_INITIALIZE");
        }

        if (type.needsChildrenInitialization()) {
            reflect("STRUCTURE_CHILD_INITIALIZATION", null);
        }

        for (Field f : type.getFields()) {
            reflectField(f);
        }

        for (Function f : type.getFunctions()) {
            reflectFunction(f);
        }
    }

    @Override
    public void beginStructure(StructureType structureType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            structureType.getName(),
            COMPOUND_TYPE_STRUCTURE
        });

        beginReflect("STRUCTURE", args);
        reflectCompoundType(structureType);
        endReflect("STRUCTURE");
    }

    @Override
    public void beginChoice(ChoiceType choiceType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            choiceType.getName(),
            COMPOUND_TYPE_CHOICE
        });

        beginReflect("STRUCTURE", args);
        reflectCompoundType(choiceType);
        endReflect("STRUCTURE");
    }

    @Override
    public void beginUnion(UnionType unionType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            unionType.getName(),
            COMPOUND_TYPE_UNION
        });

        beginReflect("STRUCTURE", args);
        reflectCompoundType(unionType);
        endReflect("STRUCTURE");
    }

    private void reflectEnumItem(EnumItem i)
    {
        List<String> args = Arrays.asList(new String[] {
            i.getName()
        });

        reflect("ENUMERATION_ITEM", args);
    }

    @Override
    public void beginEnumeration(EnumType enumType) throws ZserioEmitException
    {
        writeInclude(enumType.getPackage(), enumType.getName());

        List<String> args = Arrays.asList(new String[] {
            enumType.getName(),
        });

        beginReflect("ENUMERATION", args);

        for (EnumItem i : enumType.getItems()) {
            reflectEnumItem(i);
        }

        endReflect("ENUMERATION");
    }

    private void reflectBitmaskValue(BitmaskValue bv)
    {
        List<String> args = Arrays.asList(new String[] {
            bv.getName()
        });

        reflect("BITMASK_VALUE", args);
    }

    @Override
    public void beginBitmask(BitmaskType bitmaskType) throws ZserioEmitException
    {
        writeInclude(bitmaskType.getPackage(), bitmaskType.getName());

        List<String> args = Arrays.asList(new String[] {
            bitmaskType.getName(),
        });

        beginReflect("BITMASK", args);

        for (BitmaskValue i : bitmaskType.getValues()) {
            reflectBitmaskValue(i);
        }

        endReflect("BITMASK");
    }

    @Override

    public void beginSqlTable(SqlTableType sqlTableType) throws ZserioEmitException
    {
        /* TODO */
    }

    @Override
    public void beginSqlDatabase(SqlDatabaseType sqlDatabaseType) throws ZserioEmitException
    {
        /* TODO */
    }

    @Override
    public void beginService(ServiceType service) throws ZserioEmitException
    {
        /* TODO */
    }

    /* TODO
    @Override
    public void beginPubsub(PubsubType pubsub) throws ZserioEmitException
    {
    }
    */
}
