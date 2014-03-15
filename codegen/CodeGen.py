import sys

class Generator:
    def __init__(self):
        self.workspace = {}
    
    def doEvalBlock(self, code):
        try:
            return str( eval(code, self.workspace) )
        except Exception as e:
            print('Error evaluating expression "%s"; reason: %s' % (code, e))
            raise
    
    def doCodeBlock(self, code):
        try:
            exec(code, self.workspace)
        except Exception as e:
            print('Error executing expression "%s"; reason %s' % (code, e))
            raise
    
    def matchEnd(self, code, start):
        for i in range(len(code[start:])):
            if code[start+i] == '%' and code[start+i+1] == '>':
                return start+i+1
        return -1
    
    def doCode(self, code):
        result = ''
    
        numErrors = 0    
        currIdx = 0
        
        # Iterate over entire template
        while currIdx < len(code):
            # Find openning bracket (i.e. "<%")
            if code[currIdx] == '<' and code[currIdx+1] == '%':
                if code[currIdx+2] == '=' and code[currIdx+3] == ' ':
                    # It's an evaluation block
                    parseFnc = self.doEvalBlock
                    start = currIdx + 4
                else:
                    # It's a code block
                    start = currIdx + 3
                    parseFnc = self.doCodeBlock;
                    
                # Match the previously openned bracket
                end = self.matchEnd(code, start)
                    
                # No matching bracket found
                if end == -1:
                    raise Exception('Unmatched bracket found, aborting')
    
                # Process either expression evaluation or execution
                res = None
                try:
                    res = parseFnc( code[start:end-2] )
                except:
                    numErrors += 1
                    
                if res:
                    result += res
    
                # Skip over the processed code
                currIdx = end+1
                    
            else:
                # Plain text, just append it to the result and continiue with parsing
                result += code[currIdx]
                currIdx += 1
                
        print('Code parsed with %d errors.' % numErrors)
        # All done
        return result
            
    def doFile(self, path):
        code = open(path, 'r').read()
        
        return self.doCode(code)
    
    def getArg(self, num):
        args = sys.argv[2:]
        return args[num]
        
    def convertFile(self, inputPath, outputPath):
        res = self.doFile(inputPath)
        open(outputPath, 'w').write(res)
        
    def convertSource(self, code, outputPath):
        res = self.doCode(code)
        open(outputPath, 'w').write(res)
        
    def doGenerator(self, src):
        self.workspace = {
            'CG_convertFile' : lambda src, dst: self.convertFile(src, dst),
            'CG_convertSource' :  lambda src, dst: self.convertSource(src, dst),
            'CG_getArg' : lambda arg: self.getArg(arg),
            'CG_getNumArgs' : lambda : len(sys.argv[2:])
        }
        
        code = open(src, 'r').read()
        exec(code, self.workspace)
        
if __name__ == '__main__':
    if len(sys.argv) <= 1:
        print('Usage: python CodeGen.py <generator_file>')
        exit()
        
    generatorPath = sys.argv[1]
    
    ws = Generator();
    
    try:
        ws.doGenerator(generatorPath)
        print('Generator executed OK')
    except Exception as e:
        print('Error executing generator: ' + e)
    
    
