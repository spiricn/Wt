import subprocess, glob, os, pickle

CACHE_PATH = 'timestamps.dat'

try:
    timestampCache = pickle.load(open(CACHE_PATH, 'rb'))
except Exception as e:
    print('Creating new timestamp cache (%s)' % e)
    timestampCache = {}
    
files = []

numSkipped = 0
for i in glob.glob('./*.proto'):
    name = os.path.basename(i)
    timeStamp = os.path.getmtime(i)
    
    if name in timestampCache:
        if timeStamp == timestampCache[name]:
            numSkipped += 1
            continue
        else:
            timestampCache[name] = timeStamp
    else:
        timestampCache[name] = timeStamp
    files.append(i)

pickle.dump(timestampCache, open(CACHE_PATH, 'wb'))

if len(files) == 0:
    print('nothing to be done')
    input('Press any key to exit...')
    exit(0)

outDir = '../'
args = ['protoc', '--cpp_out=%s' % outDir] + files

print('building %d files (%d up-to-date)...' % (len(files), numSkipped))

proc = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

out, err = proc.communicate()

if proc.returncode != 0:
    print('build FAILED:')
    print(err)
else:
    print('build ok')

ccFiles = [os.path.join(outDir, os.path.basename(i.split('.proto')[0]+'.pb.cc')) for i in files]

for i in ccFiles:
    src = open(i, 'r').read()
    src = '#include "wt/stdafx.h"\n' + src
    open(i, 'w').write(src)
    
input('Press any key to exit...')
exit(0)
