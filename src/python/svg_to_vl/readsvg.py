import xml.etree.ElementTree as ET
from xml.dom import minidom
import re
import cellmodel
import path
from math import sqrt

svgFileName= '/home/ritchie/git/gallpaper/gall_vl'
#svgFileName= '/home/ritchie/Desktop/test'
namespaces = {'inkscape': 'http://www.inkscape.org/namespaces/inkscape','svg': 'http://www.w3.org/2000/svg'}


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


def readNodesFromPath(mesh,node,curr):
    mcell=mesh.getCell()
    mcell.setType(getStrokeFromStyle(node.attrib['style']))
    d = node.attrib['d']
    reset=[curr[0],curr[1]]
    coordiv = path.Path(d).makeSimpleAbsolute(curr).getAbsCoordinates();
    for coord in coordiv:
        # check if we already circular then we stop
        if mcell.addNode(coord[0],coord[1]):
            break
    mcell.addClosingWall()
    curr[0]=reset[0]
    curr[1]=reset[1]
    

def readNodesFromSvg(svgFileName, mesh):
    curr = [0.0, 0.0]
    mytree=parseSvg(svgFileName)
    myroot = mytree.getroot()
    for y in myroot.findall('svg:path',namespaces):
        readNodesFromPath(mesh,y,curr)
    for x in myroot.findall('svg:g',namespaces):
        for y in x.findall('svg:path',namespaces):
            readNodesFromPath(mesh,y,curr)

      
mesh = cellmodel.Mesh() 
readNodesFromSvg(svgFileName, mesh)
     
mesh.defineInnerCells()

tree = mesh.toXml()
pretty=minidom.parseString(ET.tostring(tree, encoding='utf8', method='xml')).toprettyxml(indent = "   ")
print(pretty)
