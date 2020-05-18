package zserio.emit.cpp_reflect;

import zserio.ast.*;
import zserio.ast.Package;
import zserio.emit.common.ZserioEmitException;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
import java.nio.file.Path;
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
    public IncludeEmitter(Path outputFilename, Parameters extensionParameters)
    {
        super(outputFilename, extensionParameters);

        this.topLevelPackageList = extensionParameters.getTopLevelPackageNameList();
    }

    private Iterable<String> topLevelPackageList;
}
