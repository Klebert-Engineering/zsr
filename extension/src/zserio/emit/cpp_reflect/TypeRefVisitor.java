package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import java.util.Arrays;
import java.util.List;

class TypeRefVisitor extends ZserioAstDefaultVisitor {
    TypeRefVisitor(EmitterBase emitter)
    {
        this.emitter = emitter;
    }

    private void reflectTypeRef(String typeName, int bitSize)
    {
        List<String> args = Arrays.asList(new String[] {
            typeName,                  // Type
            Integer.toString(bitSize), // Bit size
            "false",                   // Is array
            "",                        // Package
            ""                         // Ident
        });

        emitter.reflect("TYPE_REF", args);
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

        emitter.reflect("TYPE_REF", args);
    }

    @Override
    public void visitArrayType(ArrayType arrayType)
    {
        List<String> args = Arrays.asList(new String[] {
            "Int", // FIXME: How to get to the arrays element type?
            "0",
            "true",
            "",
            ""
        });

        emitter.reflect("TYPE_REF", args);
    }

    private EmitterBase emitter;
}
