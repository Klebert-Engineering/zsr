package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import zserio.emit.common.ZserioEmitException;
import zserio.emit.common.DefaultEmitter;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.Collection;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.lang.Integer;


public class TraitsEmitter extends EmitterBase
{
    public TraitsEmitter(BufferedWriter writer, Parameters extensionParameters)
    {
        super(writer, extensionParameters);
    }

    @Override
    public void beginStructure(StructureType structureType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            structureType.getName(),
            packageNameToNamespace(structureType.getPackage().getPackageName())
        });

        reflect("IS_COMPOUND_TRAIT", args);
    }

    @Override
    public void beginChoice(ChoiceType choiceType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            choiceType.getName(),
            packageNameToNamespace(choiceType.getPackage().getPackageName())
        });

        reflect("IS_COMPOUND_TRAIT", args);
    }

    @Override
    public void beginUnion(UnionType unionType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            unionType.getName(),
            packageNameToNamespace(unionType.getPackage().getPackageName())
        });

        reflect("IS_COMPOUND_TRAIT", args);
    }

    @Override
    public void beginEnumeration(EnumType enumType) throws ZserioEmitException
    {
        List<String> args = Arrays.asList(new String[] {
            enumType.getName(),
            packageNameToNamespace(enumType.getPackage().getPackageName())
        });

        reflect("IS_ENUMERATION_TRAIT", args);
    }
}
