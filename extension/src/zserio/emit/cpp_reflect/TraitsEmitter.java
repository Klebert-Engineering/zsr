package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import zserio.emit.common.ZserioEmitException;
import zserio.emit.common.DefaultEmitter;
import zserio.tools.Parameters;
import java.io.IOException;
import java.nio.file.Path;
import java.io.File;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;
import zserio.emit.common.FreeMarkerUtil;

public class TraitsEmitter extends EmitterBase
{
    public TraitsEmitter(Path outputDir, Parameters extensionParameters)
    {
        super(outputDir, extensionParameters);

        this.data = new TraitsTemplateData();
    }

    public void process()
    {
        try {
            final String hppFilename =
                outputDir.resolve("reflection-traits.hpp").toString();

            FreeMarkerUtil.processTemplate("reflection-traits.hpp.ftl",
                                           data,
                                           new File(hppFilename),
                                           false);
        } catch (ZserioEmitException e) {
            System.out.println("Failed to generate reflection-traits.\n" +
                               "Error: " + e);
        }
    }

    @Override
    public void beginStructure(StructureType type)
    {
        writeInclude(type.getPackage(), type.getName());

        addCompound(type);
    }

    @Override
    public void beginChoice(ChoiceType type)
    {
        writeInclude(type.getPackage(), type.getName());

        addCompound(type);
    }

    @Override
    public void beginUnion(UnionType type)
    {
        writeInclude(type.getPackage(), type.getName());

        addCompound(type);
    }

    @Override
    public void beginEnumeration(EnumType type)
    {
        writeInclude(type.getPackage(), type.getName());

        addEnum(type);
    }

    protected void addCompound(CompoundType type)
    {
        final String ident = packageNameToNamespace(
            type.getPackage().getPackageName()) + "::" +
            type.getName();

        data.compoundTypes.add(ident);
    }

    protected void addEnum(EnumType type)
    {
        final String ident = packageNameToNamespace(
            type.getPackage().getPackageName()) + "::" +
            type.getName();

        data.enumTypes.add(ident);
    }

    @Override
    protected void writeInclude(Package pkg, String objectName)
    {
        String prefix = StreamSupport.stream(topLevelPackageList.spliterator(), false)
            .collect(Collectors.joining("/"));

        if (prefix.length() > 0)
            prefix += "/";

        data.includes.add(prefix
                          + pkg.getPackageName().toFilesystemPath()
                          + "/"
                          + objectName
                          + ".h");
    }

    public TraitsTemplateData data;
}
