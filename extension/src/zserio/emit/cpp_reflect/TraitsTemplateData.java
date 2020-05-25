package zserio.emit.cpp_reflect;

import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

public class TraitsTemplateData
{
    TraitsTemplateData()
    {
        this.compoundTypes = new ArrayList<String>();
        this.enumTypes = new ArrayList<String>();
        this.includes = new ArrayList<String>();
    }

    public List<String> getCompoundTypes()
    {
        return compoundTypes;
    }

    public List<String> getEnumTypes()
    {
        return enumTypes;
    }

    public List<String> getIncludes()
    {
        return includes;
    }

    public List<String> compoundTypes;
    public List<String> enumTypes;
    public List<String> includes;
}
