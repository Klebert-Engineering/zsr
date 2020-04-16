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
import java.util.stream.Stream;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;
import java.lang.Integer;

public class EmitterBase extends DefaultEmitter
{
    public EmitterBase(BufferedWriter writer, Parameters extensionParameters)
    {
        this.writer = writer;
        this.topLevelPackageList = extensionParameters.getTopLevelPackageNameList();
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

    private void writeReflectMacro(String suffix, String what, Collection<String> with)
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

        writeln(str);
    }

    protected void writeln(String str)
    {
        try {
            writer.write(str + "\n");
        } catch (IOException e) {
            System.out.println("Error: " + e.toString());
        }
    }

    protected String packageNameToNamespace(PackageName name)
    {
        return Stream.concat(StreamSupport.stream(topLevelPackageList.spliterator(), false),
                             name.getIdList().stream())
            .collect(Collectors.joining("::"));
    }

    private int indentation = 0;
    private BufferedWriter writer;
    private Iterable<String> topLevelPackageList;
}
