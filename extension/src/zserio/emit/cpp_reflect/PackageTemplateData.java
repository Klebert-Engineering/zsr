package zserio.emit.cpp_reflect;

import java.util.ArrayList;
import java.util.List;

public class PackageTemplateData
{
    public PackageTemplateData(String filename, String initializer)
    {
        this.includes = new ArrayList<String>();
        this.namespaces = new ArrayList<String>();
        this.macros = new ArrayList<String>();

        this.filename = filename;
        this.packageInitializer = initializer;
    }

    public List<String> getIncludes()
    {
        return includes;
    }

    public List<String> getNamespaces()
    {
        return namespaces;
    }

    public List<String> getMacros()
    {
        return macros;
    }

    public String getFilename()
    {
        return filename;
    }

    public String getPackageInitializer()
    {
        return packageInitializer;
    }

    public List<String> includes;
    public List<String> namespaces;
    public List<String> macros;
    public String filename;
    public String packageInitializer;
}
