
import sys
import glob
import os
import re

zs_pkg_path = sys.argv[1]
zs_target_subpkg = sys.argv[2]
target_basename = sys.argv[3]

sub_pkg_path = os.path.join(zs_pkg_path, zs_target_subpkg)

zs_files = glob.glob(os.path.join(sub_pkg_path, "**/*.zs"), recursive="True")
zs_sub_pkg_src = ""
for zs_file_path in zs_files:
    with open(zs_file_path) as zs_file:
        zs_sub_pkg_src += zs_file.read() + "\n"

if os.path.isfile(sub_pkg_path+".zs"):
    with open(sub_pkg_path+".zs") as zs_file:
        zs_sub_pkg_src += zs_file.read() + "\n"

pattern = re.compile(r"\n\s*import ([^.]+)\..*")
# From the docs of findall: If one or more groups are present in the pattern, return a list of groups;
# this will be a list of tuples if the pattern has more than one group.
matches = set(match.strip() for match in pattern.findall(zs_sub_pkg_src))
print(";".join(f"{target_basename}--{match}" for match in matches if match != zs_target_subpkg))
