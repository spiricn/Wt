import subprocess, glob, os, pickle

# Path to the file storing .proto timestamps
CACHE_PATH = 'timestamps.dat'

# Attempt to load timestamps from file, if the file doesn't exist create an empty timestamp cache
try:
    timestampCache = pickle.load(open(CACHE_PATH, 'rb'))
except Exception as e:
    timestampCache = {}


# Generate a list of files to be compiled
files = []
numSkipped = 0
for i in glob.glob('./*.proto'):
    name = os.path.basename(i)
    timeStamp = os.path.getmtime(i)

    # Check to see if there were any changes made to the file
    if name in timestampCache and timeStamp == timestampCache[name]:
        # No changes made, skip the file
        numSkipped += 1
    else:
        # File was altered or created since the last build
        files.append(i)

# Anything to compile?
if len(files) == 0:
    print('nothing to be done')
    exit(0)

# Output directory
outDir = '../'

# Generate protoc args
args = ['protoc', '--cpp_out=%s' % outDir] + files

# Run the compiler
print('building %d files (%d up-to-date)...' % (len(files), numSkipped))
proc = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

out, err = proc.communicate()

if proc.returncode != 0:
    print('build FAILED:')
    print(err)
else:
    for file in files:
        name = os.path.basename(file)
        timeStamp = os.path.getmtime(file)	
        timestampCache[name] = timeStamp
		
    # Save the recorded timestamps
    pickle.dump(timestampCache, open(CACHE_PATH, 'wb'))
    print('build ok')

# Get a list of all compiled files
ccFiles = [os.path.join(outDir, os.path.basename(i.split('.proto')[0]+'.pb.cc')) for i in files]

for i in ccFiles:
    # Add the precompiled header include to each file
    src = open(i, 'r').read()
    src = '#include "wt/stdafx.h"\n' + src
    open(i, 'w').write(src)
