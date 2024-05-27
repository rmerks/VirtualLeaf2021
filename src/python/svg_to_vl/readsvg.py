import xml.etree.ElementTree as ET
from xml.dom import minidom
import re
import cellmodel
import path
from math import sqrt
import argparse

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
        if mcell.addNode(coord[0]*mesh.pixelScale,coord[1]*mesh.pixelScale):
            break
    mcell.addClosingWall()
    curr[0]=reset[0]
    curr[1]=reset[1]


def readNodesFromSvg(svgFileName, mesh):
    curr = [0.0, 0.0]
    mytree=parseSvg(svgFileName)
    myroot = mytree.getroot()
    viewBox = myroot.get("viewBox","0 0 100 100").split(' ');
    mesh.setScale(((float(viewBox[2]) - float(viewBox[0])) / 120 + (float(viewBox[3]) - float(viewBox[1])) / 120)/2.)

    for y in myroot.findall('svg:path',namespaces):
        readNodesFromPath(mesh,y,curr)
    for x in myroot.findall('svg:g',namespaces):
        for y in x.findall('svg:path',namespaces):
            readNodesFromPath(mesh,y,curr)

#template = '/home/ritchie/Desktop/leaf.blob.xml'
#svgFileName= '/home/ritchie/Desktop/gall_temp/root_draw_plain'
#svgFileName= '/home/ritchie/Desktop/test'

def convertSVG(svgFileName,template,scaleFactor,colormap):
    mesh = cellmodel.Mesh()
    if not (scaleFactor is None):
        mesh.pixelScale = float(scaleFactor)
    if not (colormap is None):
        mesh.setColormap(colormap)
    readNodesFromSvg(svgFileName, mesh)
    mesh.reduceParallelWalls()
    mesh.defineInnerCells()
    mesh.defineCellWalls()
    tree = mesh.toXml()
    templateTree=ET.parse(template).getroot()
    for node in templateTree:
        for newNode in tree:
            if node.tag == newNode.tag:
                node.clear()
                node.attrib = newNode.attrib
                for child in newNode:
                    node.append(child)
    pretty=minidom.parseString(ET.tostring(templateTree, encoding='utf8', method='xml')).toprettyxml(indent = "   ")
    with open(svgFileName+'.xml', "w") as f:
        f.write(pretty)
    print ("virtual-leaf-file = "+svgFileName+'.xml', end='\n')


parser=argparse.ArgumentParser(prog='readsvg',
                    description='converting cells drawn in svg files to VirtualLeaf xml start files. The svg file should be specified without extension, the resulting xml file will be stored next to the svg file.',
                    epilog='For more information see our recent publication.')
parser.add_argument("-i","--svg-file")
parser.add_argument("-t","--template-file")
parser.add_argument("-s","--scale-factor")
parser.add_argument("-c","--color-map")

#ffffff,1,2,3,4:
args=parser.parse_args()
if args.svg_file is None or args.template_file is None:
    parser.print_help()
else:
    print ("svg-file = ", args.svg_file, end='\n')
    print ("template-file = ", args.template_file, end='\n')
    print ("scale = ", args.scale_factor, end='\n')
    print ("colormap = ", args.color_map, end='\n')
    convertSVG(args.svg_file, args.template_file, args.scale_factor, args.color_map)



