import xml.etree.ElementTree as ET
from xml.dom import minidom
import re
import cellmodel
from math import sqrt

svgFileName= '/home/ritchie/git/gallpaper/gall_vl'
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


def readNodesFromSvg(svgFileName, mesh):
    curr = [0.0, 0.0]
    mytree=parseSvg(svgFileName)
    myroot = mytree.getroot()
    for x in myroot.findall('svg:g',namespaces):
        for y in x.findall('svg:path',namespaces):
            mcell=mesh.getCell()
            mcell.setType(getStrokeFromStyle(y.attrib['style']))
            d = y.attrib['d']
            coordiv = getCoordsFromPath(d)
            reset=[curr[0],curr[1]]
            for count in range(2,len(coordiv)):
                curr=parseCoordinates(curr,coordiv[count-1],d.startswith('M') or count == 2);
                mcell.addNode(curr[0],curr[1])
            mcell.addClosingWall()
            curr=[reset[0],reset[1]]

      
mesh = cellmodel.Mesh() 
readNodesFromSvg(svgFileName, mesh)
     
mesh.defineInnerCells()

tree = mesh.toXml()
pretty=minidom.parseString(ET.tostring(tree, encoding='utf8', method='xml')).toprettyxml(indent = "   ")
print(pretty)
