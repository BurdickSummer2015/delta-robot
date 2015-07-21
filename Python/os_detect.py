from sys import platform as _platform

isWindows = False
if _platform == "win32":
    isWindows = True

isUnix = False
if _platform == "linux" or _platform == "linux2" or _platform == "darwin":
    isUnix = True 

isLinux = False
if _platform == "linux" or _platform == "linux2":
    isLinux = True

isIOS = False
if _platform == "darwin":
    isIOS = True

