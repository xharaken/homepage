import glob, re, os, subprocess

def run(command):
    print(command)
    os.system(command)

files = []
files.extend(glob.glob("../cooking/*"))
files.extend(glob.glob("../europe/*"))
for file in files:
    dir_name = os.path.dirname(file)
    old_name = os.path.basename(file)
    result = re.match('(.*)(?:_)?(20\d\d)(_\d+)?.jpg', old_name)
    assert(result)
    new_name = result.group(2) + "_" + result.group(1) + (result.group(3) if result.group(3) else "") + ".jpg"
    print(dir_name + "/" + old_name, dir_name + "/" + new_name)
    os.rename(dir_name + "/" + old_name, dir_name + "/" + new_name)
    run("find ../html/ -type f | xargs sed -i 's/" + old_name + "/" + new_name + "/g'")
