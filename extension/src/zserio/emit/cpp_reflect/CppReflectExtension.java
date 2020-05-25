package zserio.emit.cpp_reflect;

import org.apache.commons.cli.Option;

import zserio.ast.Root;
import zserio.emit.common.ZserioEmitException;
import zserio.tools.Extension;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
import java.nio.file.Path;
import java.nio.file.FileSystems;
import java.io.FileWriter;
import java.io.IOException;

/**
 * The extension which generates C++ API Reflection sources.
 */
public class CppReflectExtension implements Extension
{
    @Override
    public String getName()
    {
        return "C++ Reflection Generator";
    }

    @Override
    public String getVersion()
    {
        return CppReflectExtensionVersion.VERSION_STRING;
    }

    @Override
    public void registerOptions(org.apache.commons.cli.Options options)
    {
        if (!options.hasOption(OptionCpp))
        {
            Option option = new Option(OptionCpp, true, "generate C++ reflection info");
            option.setArgName("outputDir");
            option.setRequired(false);
            options.addOption(option);
        }
    }

    @Override
    public boolean isEnabled(Parameters parameters)
    {
        return parameters.argumentExists(OptionCpp);
    }

    @Override
    public void generate(Parameters parameters, Root rootNode) throws ZserioEmitException
    {
        final Path outputDir = FileSystems.getDefault()
            .getPath(parameters.getCommandLineArg(OptionCpp));

        TraitsEmitter traitsEmitter = new TraitsEmitter(outputDir, parameters);
        rootNode.emit(traitsEmitter);
        traitsEmitter.process();

        ReflectionEmitter emitter = new ReflectionEmitter(outputDir, parameters);
        rootNode.emit(emitter);
        emitter.process();
    }

    private final static String OptionCpp = "cpp_reflect";
}
