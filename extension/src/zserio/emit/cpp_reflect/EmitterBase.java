package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import zserio.emit.common.ZserioEmitException;
import zserio.emit.common.DefaultEmitter;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.File;
import java.nio.file.Path;
import java.util.Collection;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;
import java.util.stream.Stream;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;
import java.lang.Integer;
import zserio.emit.common.FreeMarkerUtil;

public class EmitterBase extends DefaultEmitter
{
    public EmitterBase(Path outputDir, Parameters extensionParameters)
    {
        this.outputDir = outputDir;
        this.topLevelPackageList = extensionParameters.getTopLevelPackageNameList();

        this.libData = new LibraryTemplateData();
    }

    public void processPackage(Package pkg) throws ZserioEmitException
    {
        final String pkgName = pkg.getPackageName().toString(".");
        final String cppFilename = outputDir.resolve(pkgName + ".cpp").toString();
        final String hppFilename = outputDir.resolve(pkgName + ".hpp").toString();

        FreeMarkerUtil.processTemplate("package.cpp.ftl",
                                        packageData,
                                        new File(cppFilename),
                                        false);
        FreeMarkerUtil.processTemplate("package.hpp.ftl",
                                        packageData,
                                        new File(hppFilename),
                                        false);

        libData.packageFilenames.add(packageData.filename);
        libData.packageInitializers.add(packageData.packageInitializer);
    }

    public void process()
    {
        try {
            final String cppFilename = outputDir.resolve("reflection-main.cpp")
                .toString();

            FreeMarkerUtil.processTemplate("reflection-main.cpp.ftl",
                                           libData,
                                           new File(cppFilename),
                                           false);
        } catch (ZserioEmitException e) {
            System.out.println("Failed to generate reflection-main.cpp.\n" +
                               "Error: " + e);
        }
    }

    @Override
    public void beginPackage(Package pkg) throws ZserioEmitException
    {
        packageData = new PackageTemplateData(
            pkg.getPackageName().toString("."),
            pkg.getPackageName().toString("_"));
    }

    @Override
    public void endPackage(Package pkg) throws ZserioEmitException
    {
        try {
            processPackage(pkg);
        } catch (ZserioEmitException e) {
            System.out.println("Failed to generate package template.\n" +
                               "Error: " + e);
        }
    }

    protected String reflectString(String what, Collection<String> with)
    {
        return writeReflectMacroString(null, what, with);
    }

    protected void reflect(String what, Collection<String> with)
    {
        writeReflectMacro(null, what, with);
    }

    protected void beginReflect(String what, Collection<String> with)
    {
        writeReflectMacro("BEGIN", what, with);
        ++indentation;
    }

    protected void endReflect(String what)
    {
        --indentation;
        writeReflectMacro("END", what, null);
    }

    private String writeReflectMacroString(String suffix, String what, Collection<String> with)
    {
        String str = "ZSERIO_REFLECT_" + what +
            (suffix != null ? "_" + suffix : "") + "(";
        if (with != null) {
            str += with.stream()
                .collect(Collectors.joining(", "));
        }
        str += ")";

        // Prepend indentation spaces
        if (indentation > 0)
            str = new String(new char[indentation]).replace("\0", "    ") + str;
        return str;
    }

    private void writeReflectMacro(String suffix, String what, Collection<String> with)
    {
        writeln(writeReflectMacroString(suffix, what, with));
    }

    protected void writeln(String str)
    {
        packageData.macros.add(str);
    }

    protected String packageNameToNamespace(PackageName name)
    {
        return Stream.concat(StreamSupport.stream(topLevelPackageList.spliterator(), false),
                             name.getIdList().stream())
            .collect(Collectors.joining("::"));
    }

    protected void writeInclude(Package pkg, String objectName)
    {
        String prefix = StreamSupport.stream(topLevelPackageList.spliterator(), false)
            .collect(Collectors.joining("/"));

        if (prefix.length() > 0)
            prefix += "/";

        packageData.includes.add(prefix
                                 + pkg.getPackageName().toFilesystemPath()
                                 + "/"
                                 + objectName
                                 + ".h");
    }

    protected int indentation = 0;
    protected Iterable<String> topLevelPackageList;

    protected Path outputDir;
    protected LibraryTemplateData libData;
    protected PackageTemplateData packageData;
}
