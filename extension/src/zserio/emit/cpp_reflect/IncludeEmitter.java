package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import zserio.emit.common.ZserioEmitException;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
import java.util.Collection;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;


/**
 * Emitter for generating include instructions for zserio generated c++ headers.
 */
public class IncludeEmitter extends EmitterBase
{
    public IncludeEmitter(BufferedWriter writer, Parameters extensionParameters)
    {
        super(writer, extensionParameters);

        this.topLevelPackageList = extensionParameters.getTopLevelPackageNameList();
    }

    @Override
    public void beginSubtype(Subtype subtype) throws ZserioEmitException
    {
        writeInclude(subtype.getPackage(), subtype.getName());
    }

    @Override
    public void beginConst(Constant constant) throws ZserioEmitException
    {
        writeInclude(constant.getPackage(), constant.getName());
    }

    @Override
    public void beginStructure(StructureType structureType) throws ZserioEmitException
    {
        writeInclude(structureType.getPackage(), structureType.getName());
    }

    @Override
    public void beginUnion(UnionType unionType) throws ZserioEmitException
    {
        writeInclude(unionType.getPackage(), unionType.getName());
    }

    @Override
    public void beginChoice(ChoiceType choiceType) throws ZserioEmitException
    {
        writeInclude(choiceType.getPackage(), choiceType.getName());
    }

    @Override
    public void beginEnumeration(EnumType enumType) throws ZserioEmitException
    {
        writeInclude(enumType.getPackage(), enumType.getName());
    }

    @Override
    public void beginBitmask(BitmaskType bitmaskType) throws ZserioEmitException
    {
        writeInclude(bitmaskType.getPackage(), bitmaskType.getName());
    }

    private void writeInclude(Package pkg, String objectName)
    {
        String prefix = StreamSupport.stream(topLevelPackageList.spliterator(), false)
            .collect(Collectors.joining("/"));

        if (prefix.length() > 0)
            prefix += "/";

        writeln("#include \""
                + prefix
                + pkg.getPackageName().toFilesystemPath()
                + "/"
                + objectName
                + ".h\"");
    }

    private Iterable<String> topLevelPackageList;
}
