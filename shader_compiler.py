import os, glob, itertools, sys, datetime

def processSource(inSource):
    outSrc = ''

    inSource = inSource.replace('\r', '')

    return inSource

def getShaderName(path):
    name = os.path.basename(i)
    name = name.split('.')
    return name[0] + '_' + name[1]

def getShaderFunctionName(path):
    name = getShaderName(path)

    return 'const char* getShaderSource_%s()' % name

filesHeader = '''\
/*********************************************
 * AUTO-GENERATED FILE.  DO NOT MODIFY. 
 *
 * %s
 *
 * This file was automatically generated by the '%s' script
 * It should not be modified by hand.
 *********************************************/
''' % ( str(datetime.datetime.now()), os.path.basename(sys.argv[0]) )

# Directory containing the shaders
sourceDir = './shaders'

# Destination name for the source/header file
destFile = 'CompiledShaders'

destSourceFile = 'src/wt/%s.cpp' % destFile
destHeaderFile = 'include/wt/%s.h' % destFile

# Supported shader source file extensions
extensions = ['vp', 'fp', 'glsl']

# Find all the shader files in the given directory
files = [ glob.glob( os.path.join(sourceDir, '*.' + i) ) for i in extensions ]

files = list(itertools.chain(*files))

print('Compiling %d files..' % len(files))


######################################################################

# Generate header file
destHdr = filesHeader + '\n\n'

# Header guards

destHdr += '#ifndef WT_%s_H\n' % destFile.upper()
destHdr += '#define WT_%s_H\n\n' % destFile.upper()

# Namespace

destHdr += 'namespace wt\n{\n\n'

destHdr += 'const char* getShaderSource(const char* uri);\n\n'

# End namespace
destHdr += '} // </wt>\n\n'

# End header guard
destHdr += '#endif // </WT_%s_H>\n' % destFile.upper()

open(destHeaderFile, 'wb').write(bytes(destHdr, 'ASCII'))

######################################################################

# Generate source file
destSrc = filesHeader + '\n\n'

destSrc += '#include "wt/stdafx.h"\n'
destSrc += '#include "wt/%s.h"\n' % destFile
destSrc += '#include <map>\n\n'

destSrc += '#define TD_TRACE_TAG "CompiledShaders"\n\n'

destSrc += 'namespace wt\n{\n\n'

destSrc += 'typedef std::map<std::string, const char*> SourceMap;\n\n'

destSrc += 'static SourceMap gSourceMap;\n'

destSrc += 'static bool gMapInitialized = false;\n\n'

for i in files:
    destSrc += '/* Generated from "%s" */\n' % i
    destSrc += 'static const char* %s = \n' % getShaderName(i)

    source = processSource(open(i, 'rb').read().decode('ascii'))
    for j in source.split('\n'):
        destSrc += '\t\"'
        destSrc += j
        destSrc += '\\n"\n'
        
    destSrc += '; // </%s>\n\n' % getShaderName(i)

destSrc += 'const char* getShaderSource(const char* uri){\n'
destSrc += '\tif(!gMapInitialized){\n\t\tLOGV("Initializing shader map");\n\n'

for i in files:
    fileName = os.path.basename(i)
    destSrc += '\t\tgSourceMap.insert( std::make_pair("%s", %s) );\n' % (fileName, getShaderName(i))

destSrc += '\n\t\tLOGV("Shader map initialized with %d shaders");\n' % len(files)

destSrc += '\n\t\tgMapInitialized = true;\n\t}\n\n'

destSrc += '\tSourceMap::iterator res = gSourceMap.find(uri);\n\n'
destSrc += '\treturn res == gSourceMap.end() ? NULL : res->second;\n'
destSrc += '}\n\n'


# End namespace
destSrc += '} // </wt>\n'

open(destSourceFile, 'wb').write(bytes(destSrc, 'ascii'))

######################################################################

print('Done')
