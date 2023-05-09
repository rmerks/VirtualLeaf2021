import xml.etree.ElementTree as ET
import re
import cellmodel
from math import sqrt

svgFileName= '/home/ritchie/git/gallpaper/gall_vl'
namespaces = {'inkscape': 'http://www.inkscape.org/namespaces/inkscape','svg': 'http://www.w3.org/2000/svg'}

def distance(tuple1, tuple2):
    return sqrt(pow(tuple1[0]-tuple2[0],2)+ pow(tuple1[1]-tuple2[1],2))

def addIfNotExists(existing, tupleToAdd, mul):
    for current in existing:
        if distance(current,tupleToAdd) < mul:
             return current;
    existing.add(tupleToAdd)
    return tupleToAdd

def getNodeNear(existing, tupleToAdd, mul,cell):
    for count, current in enumerate(existing):
        if distance(current,tupleToAdd) < mul:
            cell.append(count)
            return current;
    raise Exception("No near node found") 

def parseSvg(svgFileName):
    mytree = ET.parse(svgFileName+'.svg')
  
    return mytree

def parseCoordinates(curr, coord, absolute):
    xy = re.split(',', coord)
    xy = list(map(float, xy))
    if absolute:
        return [xy[0],xy[1]]
    else:
        return [curr[0]+xy[0],curr[1]+xy[1]]
    
def getStrokeFromStyle(style):
    style = re.split(';|:', style)
    return style[style.index('stroke')+1]

    
def getCoordsFromPath(d):
    return  re.split('m |M | z| Z| ', d)

def readNodesFromSvg(svgFileName,mul):
    mytree=parseSvg(svgFileName)
    myroot = mytree.getroot()
    allC = set()
    curr = [0.0, 0.0]
    for x in myroot.findall('svg:g',namespaces):
        for y in x.findall('svg:path',namespaces):
            strokev=getStrokeFromStyle(y.attrib['style'])
            d = y.attrib['d']
            coordiv = getCoordsFromPath(d)
            reset=[curr[0],curr[1]]
            for count in range(2,len(coordiv)):
                curr=parseCoordinates(curr,coordiv[count-1],d.startswith('M') or count == 2);
                addIfNotExists(allC,tuple(curr),mul);
            curr=[reset[0],reset[1]]
    return list(allC)   

def readNodesFromSvgAndReduce(svgFileName, allC, mul,mesh):
    curr = [0.0, 0.0]
    cells = list();
    mytree=parseSvg(svgFileName)
    myroot = mytree.getroot()
    for x in myroot.findall('svg:g',namespaces):
        for y in x.findall('svg:path',namespaces):
            mcell=mesh.getCell()
            strokev=getStrokeFromStyle(y.attrib['style'])
            d = y.attrib['d']
            coordiv = getCoordsFromPath(d)
            str1 = 'M ';
            reset=[curr[0],curr[1]]
            cell=list()
            for count in range(2,len(coordiv)):
                curr=parseCoordinates(curr,coordiv[count-1],d.startswith('M') or count == 2);
                xy = getNodeNear(allC,tuple(curr),mul,cell);
                mcell.addNode(xy[0],xy[1])
                str1= str1 + str(xy[0]) + ',' + str(xy[1]) + ' '
            str1= str1 + 'Z'
            mcell.addClosingWall()
            cells.append(cell)
            curr=[reset[0],reset[1]]
            y.set('d',str1)
    mytree.write(svgFileName + '-out.svg')
    return cells
    
mul=0.75
allC = readNodesFromSvg(svgFileName, mul)
    
print(len(allC))  
      
mesh = cellmodel.Mesh() 
cells = readNodesFromSvgAndReduce(svgFileName, allC, mul,mesh)

print(cells)
      
mesh.defineInnerCells()

print(mesh)
