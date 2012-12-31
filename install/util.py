from gzip import open
from os import (
    X_OK,
    access,
    chdir,
    environ,
    getcwd,
    makedirs,
    pardir,
    pathsep,
    sep
)
from os.path import abspath, dirname, isabs, isdir, isfile, join
from platform import mac_ver, win32_ver
from string import Template
from subprocess import PIPE, Popen

MAJOR_VERSION = 0
MINOR_VERSION = 1
REVISION = 0
VERSION = "%d.%d.%d" % (MAJOR_VERSION, MINOR_VERSION, REVISION)

PLATFORM_MACX = 1
PLATFORM_UNIX = 2
PLATFORM_WIN32 = 3

_PLATFORM = (mac_ver()[0] and PLATFORM_MACX) or \
    (win32_ver()[0] and PLATFORM_WIN32) or PLATFORM_UNIX

def createSourcePackage(path):
    gzipStream = open(filename=path, mode="wb")
    try:
        gitArgs = ["git", "archive", "--format=tar",
                   "--prefix=midisnoop-%s%s" % (VERSION, sep), "HEAD"]
        oldDirectory = getcwd()
        chdir(getRootDirectory())
        try:
            process = Popen(gitArgs, stdout=PIPE, bufsize=-1)
            try:
                processOut = process.stdout
                while True:
                    data = processOut.read(8192)
                    if not data:
                        break
                    gzipStream.write(data)
            finally:
                result = process.wait()
            if result:
                raise Exception("git archive process failed")
        finally:
            chdir(oldDirectory)
    finally:
        gzipStream.close()

def findExecutable(command):
    if not isabs(command):
        path = environ.get("PATH")
        if path is not None:
            for path in path.split(pathsep):
                commandPath = abspath(join(path.strip('\"'), command))
                if isfile(commandPath) and access(commandPath, X_OK):
                    return commandPath
    commandPath = abspath(command)
    if isfile(commandPath) and access(commandPath, X_OK):
        return commandPath
    return None

def getPlatform():
    return _PLATFORM

def getResourcesDirectory():
    return join(getRootDirectory(), "resources")

def getRootDirectory():
    return abspath(join(dirname(__file__), pardir))

def getTemplatesDirectory():
    return join(getRootDirectory(), "templates")

def writeTemplate(destination, source, data):
    destination = abspath(destination)
    destinationDir = dirname(destination)
    if not isdir(destinationDir):
        makedirs(destinationDir)
    source = abspath(source)
    sourceDir = dirname(source)
    if not isdir(sourceDir):
        makedirs(sourceDir)
    fp = file(source)
    try:
        s = fp.read()
    finally:
        fp.close()
    s = Template(s).substitute(data)
    fp = file(destination, 'w')
    try:
        fp.write(s)
    finally:
        fp.close()
