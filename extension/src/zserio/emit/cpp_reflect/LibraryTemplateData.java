package zserio.emit.cpp_reflect;

import java.util.ArrayList;
import java.util.List;

public class LibraryTemplateData
{
    public LibraryTemplateData()
    {
        this.packageFilenames = new ArrayList<String>();
        this.packageInitializers = new ArrayList<String>();
    }

    public List<String> getPackageFilenames()
    {
        return packageFilenames;
    }

    public List<String> getPackageInitializers()
    {
        return packageInitializers;
    }

    public List<String> packageInitializers;
    public List<String> packageFilenames;
}
