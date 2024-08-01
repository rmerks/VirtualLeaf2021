import re

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


class Path:
    
    def __init__(self,line):
        self.line=line
        self.splitValues()
        self.addImplicits()
        self.simplify()

    def splitValues(self):
        self.commandElements=list()
        lastChar='0'
        currentCommand = ''
        self.commandElements = re.split(r',|\s', self.line)
            
    def addImplicits(self):
        index = 0
        while index < len(self.commandElements):
            command = self.commandElements[index]
            if is_number(command):
                self.commandElements.insert(index, implicitCommand)
                command = implicitCommand
            implicitCommand=command
            if command == 'L' or command == 'l' :
                index += 2
            if command == 'M' or command == 'm' :
                if command.isupper():
                    implicitCommand = 'L'
                else:
                    implicitCommand = 'l'
                index += 2
            if command == 'C' or command == 'c' :
                index += 6
            if command == 'S' or command == 's' :
                index += 4
            if command == 'Q' or command == 'q' :
                index += 4
            if command == 'T' or command == 't' :
                index += 2
            if command == 'A' or command == 'a' :
                index += 6
            if command == 'Z' or command == 'z' :
                index += 0
            if command == 'H' or command == 'h' :
                index += 1
            if command == 'V' or command == 'v' :
                index += 1
            index += 1 #the command itself
            


    def simplify(self):
        index = 0
        length = len(self.commandElements)
        self.simpleCommandElements=list()
        while index < length:
            command = self.commandElements[index]
            if command == 'H' or command == 'h' :
                self.simpleCommandElements.append(command)
                self.simpleCommandElements.append(self.commandElements[index+1])
                index += 1
            if command == 'V' or command == 'v' :
                self.simpleCommandElements.append(command)
                self.simpleCommandElements.append(self.commandElements[index+1])
                index += 1
            if command == 'L' or command == 'l' :
                self.simpleCommandElements.append(command)
                self.simpleCommandElements.append(self.commandElements[index+1])
                self.simpleCommandElements.append(self.commandElements[index+2])
                index += 2
            if command == 'M' or command == 'm' :
                self.simpleCommandElements.append(command)
                self.simpleCommandElements.append(self.commandElements[index+1])
                self.simpleCommandElements.append(self.commandElements[index+2])
                index += 2
            if command == 'C' or command == 'c' :
                if command == 'C':
                    self.simpleCommandElements.append('L')
                else:
                    self.simpleCommandElements.append('l')
                self.simpleCommandElements.append(self.commandElements[index+5])
                self.simpleCommandElements.append(self.commandElements[index+6])
                index += 6
            if command == 'S' or command == 's' :
                if command == 'S':
                    self.simpleCommandElements.append('L')
                else:
                    self.simpleCommandElements.append('l')
                self.simpleCommandElements.append(self.commandElements[index+3])
                self.simpleCommandElements.append(self.commandElements[index+4])
                index += 4
            if command == 'Q' or command == 'q' :
                if command == 'Q':
                    self.simpleCommandElements.append('L')
                else:
                    self.simpleCommandElements.append('l')
                self.simpleCommandElements.append(self.commandElements[index+3])
                self.simpleCommandElements.append(self.commandElements[index+4])
                index += 4
            if command == 'T' or command == 't' :
                if command == 'T':
                    self.simpleCommandElements.append('L')
                else:
                    self.simpleCommandElements.append('l')
                self.simpleCommandElements.append(self.commandElements[index+1])
                self.simpleCommandElements.append(self.commandElements[index+2])
                index += 2
            if command == 'A' or command == 'a' :
                if command == 'A':
                    self.simpleCommandElements.append('L')
                else:
                    self.simpleCommandElements.append('l')
                self.simpleCommandElements.append(self.commandElements[index+5])
                self.simpleCommandElements.append(self.commandElements[index+6])
                index += 6
            if command == 'Z' or command == 'z' :
                self.simpleCommandElements.append(command)
                index += 0
            index += 1 #the command itself
                        
    def commands(self):
        print(self.commandElements)
        print(self.simpleCommandElements)
        return self
        
    def makeSimpleAbsolute(self,lastCoord):
        index = 0
        length = len(self.simpleCommandElements)
        while index < length:
            command = self.simpleCommandElements[index]
            if command == 'H':
                lastCoord[0] = float(self.simpleCommandElements[index+1])
# unchanged                lastCoord[1] 
                index += 1
            if command == 'V':
                lastCoord[1] = float(self.simpleCommandElements[index+1])
# unchanged                lastCoord[0] 
                index += 1
            if command == 'L':
                lastCoord[0] = float(self.simpleCommandElements[index+1])
                lastCoord[1] = float(self.simpleCommandElements[index+2])
                index += 2
            if command == 'M':
                lastCoord[0] = float(self.simpleCommandElements[index+1])
                lastCoord[1] = float(self.simpleCommandElements[index+2])
                index += 2
            if command == 'Z':
                index += 0
            if command == 'l' :
                lastCoord[0] = lastCoord[0] + float(self.simpleCommandElements[index+1])
                lastCoord[1] = lastCoord[1] + float(self.simpleCommandElements[index+2])
                self.simpleCommandElements[index]='L'
                self.simpleCommandElements[index+1]=str(lastCoord[0])
                self.simpleCommandElements[index+2]=str(lastCoord[1])
                index += 2
            if command == 'm' :
                lastCoord[0] = lastCoord[0] + float(self.simpleCommandElements[index+1])
                lastCoord[1] = lastCoord[1] + float(self.simpleCommandElements[index+2])
                self.simpleCommandElements[index]='M'
                self.simpleCommandElements[index+1]=str(lastCoord[0])
                self.simpleCommandElements[index+2]=str(lastCoord[1])
                index += 2
            if command == 'z' :
                self.simpleCommandElements[index]='Z'
                index += 0
            index += 1 #the command itself
        return self
        
    def getAbsCoordinates(self):
        result = list()
        index = 0
        length = len(self.simpleCommandElements)
        while index < length:
            command = self.simpleCommandElements[index]
            if command == 'L':
                point = [float(self.simpleCommandElements[index+1]),float(self.simpleCommandElements[index+2])]
                result.append(point);
                index += 2
            if command == 'M':
                point = [float(self.simpleCommandElements[index+1]),float(self.simpleCommandElements[index+2])]
                result.append(point);
                index += 2
            if command == 'Z':
                index += 0
            index += 1 #the command itself
        return result
        





