package zserio.emit.cpp_reflect;

import org.apache.commons.cli.Option;

import zserio.ast.Root;
import zserio.emit.common.ZserioEmitException;
import zserio.tools.Extension;
import zserio.tools.Parameters;
import java.io.BufferedWriter;
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
        final String outputDir = parameters.getCommandLineArg(OptionCpp);
        final String outputFile = outputDir + "/" + "reflection-defs.cpp";

        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(outputFile));

            try {
                final int VERSION = 1;

                writer.write("#if defined(ZSR_VERSION) && ZSR_VERSION != " + Integer.toString(VERSION) + "\n" +
                             "    #error \"zsr version does not match version of the generate code.\"\n" +
                             "#endif\n\n");
            } catch (IOException e) {
                System.out.println("Error: " + e.toString());
            }

            rootNode.emit(new IncludeEmitter(writer, parameters));
            rootNode.emit(new TraitsEmitter(writer, parameters));
            rootNode.emit(new ReflectionEmitter(writer, parameters));

            writer.flush();
            writer.close();
        } catch (IOException e) {
            System.out.println("Error opening file for writing.");
        }
    }

    private final static String OptionCpp = "cpp_reflect";
}
