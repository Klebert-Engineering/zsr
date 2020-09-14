package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import java.util.Arrays;
import java.util.List;

class TypeRefVisitor extends ZserioAstDefaultVisitor
{
    TypeRefVisitor(EmitterBase emitter, boolean inArray)
    {
        this.emitter = emitter;
        this.inArray = inArray;
    }

    protected void reflectTypeRef(String typeName, int bitSize, Package pkg, String ident)
    {
        List<String> args = Arrays.asList(new String[] {
            typeName,
            Integer.toString(bitSize),
            this.inArray ? "true" : "false",
            pkg != null ? pkg.getPackageName().toString() : "",
            ident != null ? ident : ""
        });

        emitter.reflect("TYPE_REF", args);
    }

    private void reflectTypeRef(String typeName, int bitSize)
    {
        reflectTypeRef(typeName, bitSize, null, null);
    }

    @Override
    public void visitStringType(StringType type)
    {
        reflectTypeRef("String", 0);
    }

    @Override
    public void visitBitmaskType(BitmaskType type)
    {
        reflectTypeRef("Bitmask", 0, type.getPackage(), type.getName());
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
    public void visitEnumType(EnumType type)
    {
        reflectTypeRef("Enum", 0, type.getPackage(), type.getName());
    }

    @Override
    public void visitChoiceType(ChoiceType type)
    {
        reflectTypeRef("Structure", 0, type.getPackage(), type.getName());
    }

    @Override
    public void visitUnionType(UnionType type)
    {
        reflectTypeRef("Structure", 0, type.getPackage(), type.getName());
    }

    @Override
    public void visitStructureType(StructureType type)
    {
        reflectTypeRef("Structure", 0, type.getPackage(), type.getName());
    }

    @Override
    public void visitSubtype(Subtype type)
    {
        class SubtypeVisitor extends TypeRefVisitor
        {
            SubtypeVisitor(EmitterBase emitter, boolean inArray, Subtype subtype)
            {
                super(emitter, inArray);

                this.subtype = subtype;
            }

            protected void reflectTypeRef(String typeName, int bitSize, Package pkg, String ident)
            {
                /* Overwrite pkg & ident with subtype values */
                pkg = subtype.getPackage();
                ident = subtype.getName();

                super.reflectTypeRef(typeName, 0, pkg, ident);
            }

            private Subtype subtype;
        }

        type.getBaseTypeReference().getType().accept(new SubtypeVisitor(emitter, inArray, type));
    }

    @Override
    public void visitTypeInstantiation(TypeInstantiation type)
    {
        if (type instanceof ArrayInstantiation) {
            try {
                inArray = true;
                ((ArrayInstantiation)type).getElementTypeInstantiation().accept(this);
            } finally {
                inArray = false;
            }
        } else {
            type.getType().accept(this);
        }
    }

    private EmitterBase emitter;
    private boolean inArray;
}
