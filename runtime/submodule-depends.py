
import sys
import glob
import os
import re

root = sys.argv[1] # Root directory to scan for dependencies
top_level_pkg = sys.argv[2] # ZSerio top-level-package name
package_depth = int(sys.argv[3]) # Sub-Package split depth (see add_zserio_module SUBDIR_DEPTH)

zs_files = glob.glob(os.path.join(root, "**/*.zs"), recursive="True")

zs_sub_pkg_src = ""
for zs_file_path in zs_files:
    with open(zs_file_path) as zs_file:
        zs_sub_pkg_src += zs_file.read() + "\n"

if os.path.isfile(root+".zs"):
    with open(root+".zs") as zs_file:
        zs_sub_pkg_src += zs_file.read() + "\n"

pattern = re.compile(r"\n\s*import ([^.]+)(\.[^.]+){"+str(package_depth-1)+"}\..*")
# From the docs of findall: If one or more groups are present in the pattern, return a list of groups;
# this will be a list of tuples if the pattern has more than one group.
matches = set("".join(match) for match in pattern.findall(zs_sub_pkg_src))
matches = set(match.replace(".", "--") for match in matches)

print(";".join(f"{top_level_pkg}--{match}" for match in matches))
