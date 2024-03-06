from math import sqrt
from math import hypot
import xml.etree.ElementTree as ET
from shapely.geometry import Polygon
from shapely.geometry.polygon import orient
from mpi4py.futures.aplus import catch
from matplotlib.projections import geo
from pip._vendor.pygments.formatters import other
from sympy.logic.boolalg import false, true
from fontTools.varLib.models import allNone
from vtkmodules.numpy_interface.algorithms import area
from sympy.polys.polytools import nroots
from pip._vendor.typing_extensions import Self

class Mesh:

    def __init__(self):
        self.walls=list()
        self.cellWalls=list()
        self.nodes=list()
        self.cells=list()
        self.nodeNr=0
        self.wallNr=0
        self.cellNr=0
        self.mul=0.75
        self.pixelScale=10.
        
    def setScale(self, onePersentScale):
        self.mul=onePersentScale*self.pixelScale;
        
    def getNode(self,x,y):
        for existing_node in self.nodes:
            if existing_node.distance(x,y) < self.mul:
                return existing_node 
        node = Node(x,y,self.nodeNr)
        self.nodeNr+=1
        self.nodes.append(node)
        return node
    
    def getWall(self,node1,node2):
        if node1==node2:
            print("error")
        for existing_wall in self.walls:
            if existing_wall.isBetween(node1,node2):
                return existing_wall 
        wall = Wall(node1,node2,self.wallNr)
        self.wallNr+=1
        self.walls.append(wall)
        return wall
    
    def getCell(self):
        cell = Cell(self,self.cellNr)
        self.cellNr+=1
        self.cells.append(cell)
        return cell

    def addWall(self, wall):
        self.walls.append(wall)

    
    def defineInnerCells(self):
        for wall in self.walls:
            if wall.isBorder():
                cell = self.getCell()
                cell.setType("#ffffff")
                cell.defineInnerCell(wall)
        another=False;
        for cell in self.cells:
            if cell.retryDefineInnerCell() == None:
                another=True
        if another:
            for cell in self.cells:
                if cell.retryDefineInnerCell() == None:
                    print("not successful, defining an inner cell!")
        self.removeCircumverence()
        
    def wallsToSplit(self):
        wallsToSplit=list()
        for wall in self.walls:
            connectedWalls=set()
            connectedNodes=set()
            for otherWall in self.walls:
                if wall != otherWall and otherWall.isConnected(wall):
                    connectedWalls.add(otherWall)
                    connectedNodes.add(otherWall.node1)
                    connectedNodes.add(otherWall.node2)
            connectedNodes.remove(wall.node1)
            connectedNodes.remove(wall.node2)
            # now all remaining nodes are outside of this wall so if they are shared by two walls we have a triangle
            for tiangleNode in connectedNodes:
                nodeWalls=list(connectedWalls.intersection(tiangleNode.walls))
                if len(nodeWalls) == 2 and nodeWalls[0].length() < wall.length() and nodeWalls[1].length() < wall.length():
                    #triagle
                    area = nodeWalls[0].areaOfTriangle(nodeWalls[1])
                    if area < 4.:
                        nodeWalls.append(wall)
                        wallsToSplit.append(nodeWalls)
        return wallsToSplit
    
    def splitParallelWall(self, wall):
           for cell in self.cells:
               if (wall[2] in cell.walls):
                   index = cell.walls.index(wall[2])
                   after=(index+1)%len(cell.walls);
                   if wall[1] in cell.walls[after].node1.walls:
                       cell.walls.remove(wall[2])
                       wall[2].cells.remove(cell)
                       cell.walls.insert(index,wall[1])
                       wall[1].cells.append(cell)
                       cell.walls.insert(index,wall[0])
                       wall[0].cells.append(cell)
                   else:
                       cell.walls.remove(wall[2])
                       wall[2].cells.remove(cell)
                       cell.walls.insert(index,wall[0])
                       wall[0].cells.append(cell)
                       cell.walls.insert(index,wall[1])
                       wall[1].cells.append(cell)
           wall[2].node1.walls.remove(wall[2])
           wall[2].node2.walls.remove(wall[2])
           self.walls.remove(wall[2])
                         
    def splitParallelWalls(self, wallsToSplit):
        for wall in wallsToSplit:
            self.splitParallelWall(wall)
                          
    def reduceParallelWalls(self):
        stopLoop = False
        while (not stopLoop):
            wallsToSplit=self.wallsToSplit()
            if len(wallsToSplit) > 0:
                self.splitParallelWalls(wallsToSplit)
            else:
                stopLoop=True
        
             
    def numberWalls(self):
        nr = 0
        for wall in self.walls:
            wall.nr = nr
            nr=nr+1
        nr = 0
        for wall in self.cellWalls:
            wall.nr = nr
            nr=nr+1
                        
                        
    def removeCircumverence(self):
        longest = 0.0
        self.boundary_polygon = None
        for cell in self.cells:
            size=cell.circumverence()
            if size>longest:
                longest=size
                self.boundary_polygon=cell
        if self.boundary_polygon in self.cells:
            self.cells.remove(self.boundary_polygon)
            for wall in self.boundary_polygon.getWalls():
                wall.getCells().remove(self.boundary_polygon)
            
    def toXml(self):
        docLeaf = ET.Element("leaf")
        docNodes = ET.SubElement(docLeaf, "nodes", n = str(len(self.nodes)))
        for node in self.nodes:
            node.toXml(docNodes)
        docCells = ET.SubElement(docLeaf, "cells" \
                                 , n = str(len(self.cells)) \
                                 , magnfication="1" \
                                 , nchem="0" \
                                 , offsetx="0" \
                                 , offsety="0" \
                                 , base_area="0" \
                                 )
        for cell in self.cells:
            cell.toXml(docCells)
        self.boundary_polygon.toBoundaryBolygon(docCells)
        docWalls = ET.SubElement(docLeaf, "walls", n = str(len(self.walls)))
#        for wall in self.walls:
        for wall in self.cellWalls:
            wall.toXml(docWalls)
        return docLeaf
    
    def defineCellWalls(self):
        toDo = list(self.walls)
        while len(toDo) > 0:
            startWall = toDo[0]
            if len(startWall.cells) < 2:
                toDo.remove(startWall)
                continue
            firstCellWall = CellWall(startWall.cells[0],startWall.cells[1],startWall.node1)
            if (len(firstCellWall.walls)>1):
                print(len(firstCellWall.walls))
            for wall in firstCellWall.walls:
                if wall in toDo:
                    toDo.remove(wall)
                else:
                    print()
            self.cellWalls.append(firstCellWall)
        for wall in self.cellWalls:
            wall.addToCells()
        self.numberWalls()
        
class Node:
    def __init__(self,x,y,nr):
        self.nr= nr
        self.x=x
        self.y=y
        self.walls=list()
        
    def distance(self, x,y):
        return sqrt(pow(self.x-x,2) + pow(self.y-y,2))

    def distanceO(self, other):
        return sqrt(pow(self.x-other.x,2) + pow(self.y-other.y,2))
            
    def addWall(self, wall):
        self.walls.append(wall)
        
    def getNr(self):
        return self.nr
        
    def getNextBorderNode(self, existing_walls):
        potential_next_borders = [value for value in self.walls if (value not in existing_walls) and value.isBorder()]
        if len(potential_next_borders) == 1:
            wall_to_add = potential_next_borders[0]
            if wall_to_add.getNode(1) == self:
                return wall_to_add.getNode(2)
            else:
                return wall_to_add.getNode(1)
            return 
        else:
            return None    
            
    def toXml(self,docNodes):
        border = False
        fixed = False
        for wall in self.walls:
            border = border or wall.isBorder();
            for cell in wall.getCells():
                fixed = fixed or cell.getType() == "#000000"
        docNode = ET.SubElement(docNodes, "node", nr=str(self.nr), x = str(self.x), y = str(self.y), sam="false", boundary=str(border).lower(), fixed=str(fixed).lower())

    def getTuple(self):
        return (self.x, self.y)

def findEndOfCellWall(currentNode,sharedWalls,walls):
    nextNode = None
    while nextNode != currentNode:
        nextNode = currentNode
        for wall in sharedWalls:
            if wall.node1 == currentNode:
                currentNode = wall.node2
                sharedWalls.remove(wall)
                walls.append(wall)
                break
            elif wall.node2 == currentNode:
                currentNode = wall.node1
                sharedWalls.remove(wall)
                walls.append(wall)
                break
    return currentNode
            

class CellWall:
    def __init__(self, cell1, cell2, startNode):
        self.cell1 = cell1
        self.cell2 = cell2
        self.startNode = startNode
        self.walls=list()
        sharedWalls = [value for value in self.cell1.walls if value in self.cell2.walls]
        currentNode=startNode
        self.endNode = findEndOfCellWall(currentNode,sharedWalls,self.walls)
        if len(sharedWalls) > 0:
            # now check if the startNode can be pushed back
            otherEnd = findEndOfCellWall(currentNode,sharedWalls,self.walls) 
            if otherEnd != currentNode:
                self.startNode = otherEnd
                
    def addToCells(self):
        self.cell1.addCellWall(self)
        self.cell2.addCellWall(self)
        
    def sortCellWall(self):
        minNr = 10000
        for wall in self.walls:
            minNr=min(minNr,wall.nr)
        return minNr
        
    def toXml(self,wallNodes):
        wallNode = ET.SubElement(wallNodes, "wall" \
                                 , length=str(self.length()) \
                                 , c1=str(self.cell1.getNr()) \
                                 , c2=str(self.cell2.getNr()) \
                                 , index=str(self.nr) \
                                 , n1=str(self.startNode.getNr()) \
                                 , n2=str(self.endNode.getNr()) \
                                 , wall_type="normal"  \
                                 , viz_flux="0" \
                                 )
        ET.SubElement(wallNode, "transporters1")
        ET.SubElement(wallNode, "transporters2")        
        

    def getNr(self):
        return self.nr
    
    def length(self):
        total = 0
        for wall in self.walls:
            total=total+wall.length()
        return total

class Wall:
    def __init__(self, node1, node2,nr):
        self.nr = nr
        self.node1 = node1
        self.node2 = node2
        self.node1.addWall(self)
        self.node2.addWall(self)
        self.cells=list()
        
    def getNr(self):
        return self.nr
    
    def areaOfTriangle(self, otherWall):
        allNodes = set()
        allNodes.add(self.node1)
        allNodes.add(self.node2)
        allNodes.add(otherWall.node1)
        allNodes.add(otherWall.node2)
        assert len(allNodes) == 3
        corners = list(allNodes)
        return (0.5)*abs(corners[0].x*(corners[1].y - corners[2].y) + \
                         corners[1].x*(corners[2].y - corners[0].y) + \
                         corners[2].x*(corners[0].y - corners[1].y))
    
    def isConnected(self, other):
        return self.node1==other.node1 or self.node2==other.node1 or self.node1==other.node2 or self.node2==other.node2
    
    def isBetween(self, node1, node2):
        return (self.node1 == node1 and self.node2 == node2) or \
              (self.node1 == node2 and self.node2 == node1)
    
    def addCell(self, cell):
        if cell not in self.cells:
            self.cells.append(cell)
        
    def isBorder(self):
        return len(self.cells) < 2
                
    def getNode(self,nr):
        if nr==1: 
            return self.node1 
        else: 
            return self.node2
        
    def getCells(self):
        return self.cells  
    
    def length(self):
        return self.node1.distanceO(self.node2)
    
    def toXml(self,wallNodes):
        if len(self.cells) == 1:
            c2nr=-1
        else:        
            c2nr=self.cells[1].getNr()
        wallNode = ET.SubElement(wallNodes, "wall" \
                                 , length=str(self.length()) \
                                 , c1=str(self.cells[0].getNr()) \
                                 , c2=str(c2nr) \
                                 , index=str(self.nr) \
                                 , n1=str(self.node1.getNr()) \
                                 , n2=str(self.node2.getNr()) \
                                 , wall_type="normal"  \
                                 , viz_flux="0" \
                                 )
        ET.SubElement(wallNode, "transporters1")
        ET.SubElement(wallNode, "transporters2")
        

    
    

class Cell:
    def __init__(self, mesh,nr):
        self.nr=nr
        self.walls = list()
        self.cellWalls=list()
        self.mesh = mesh
        self.firstNode = None
        self.lastNode = None
        self.type = None
    
    def circumverence(self):
        size = 0.0
        for wall in self.walls:
            size += wall.length()
        return size
        
    def setType(self,type):
        self.type=type
        
    def getNr(self):
        return self.nr
    
    def getWalls(self):
        return self.walls
        
    def getType(self):
        return self.type
    
    def addCellWall(self, cellWall):
        self.cellWalls.append(cellWall)
        self.cellWalls.sort(key=lambda x: x.sortCellWall())
        
    def addNode(self, x, y):
        return self.addNodeO(self.mesh.getNode(x,y)) 

    def addNodeO(self, node):
        if self.lastNode is None:
            self.lastNode = node
            self.firstNode = node
            return False
        else:
            if len(self.walls)>1 and node==self.firstNode: 
                return True
            if self.lastNode == node:
                return False
            wall = self.mesh.getWall(self.lastNode,node)
            self.appendWall(wall)
            wall.addCell(self)
            self.lastNode = node
            return False
             
    def addClosingWall(self):
             wall = self.mesh.getWall(self.lastNode,self.firstNode)
             self.appendWall(wall)
             wall.addCell(self)
             self.lastNode = None

    def appendWall(self,wall):
        if wall not in self.walls:
            self.walls.append(wall)
            
    def appendFrontWall(self,wall):
        if wall not in self.walls:
            self.walls.insert(0, wall)

    def defineInnerCell(self,wall):
        self.appendWall(wall)
        wall.addCell(self)
        self.firstNode = wall.getNode(1)
        self.lastNode = wall.getNode(2)
        self.retryDefineInnerCell()    
            
    def retryDefineInnerCell(self):
        if self.lastNode == None:
            return self
        nextNode = self.lastNode.getNextBorderNode(self.walls)
        while nextNode != None and nextNode != self.firstNode:
            self.addNodeO(nextNode)
            nextNode = nextNode.getNextBorderNode(self.walls)
        if nextNode == self.firstNode:
            self.addClosingWall()
        else:
            nextNode = self.firstNode.getNextBorderNode(self.walls)
            while nextNode != None and nextNode != self.lastNode:
                wall = self.mesh.getWall(self.firstNode,nextNode)
                self.appendFrontWall(wall)
                self.firstNode = nextNode; 
                wall.addCell(self)
                nextNode = nextNode.getNextBorderNode(self.walls)
            if nextNode == self.lastNode:
                self.addClosingWall()
            else:
                return None
        return self
    
    def toXml(self,cellNodes):
        border = 0
        boundary = 0
        for wall in self.walls:
            if wall.isBorder():
                border = 1
        cellType = "0"
        boundary = 0 # None
        if self.type == "#ffffff":
            cellType = "1"
            boundary = 2 # SOURCESINK 
        if self.type == "#0000f8":
            cellType = "2"
        if self.type == "#009000":
            cellType = "3"
        self.toXmlI("cell",cellNodes,border,boundary,cellType,self.nr,self.type == "#000000")

    def toXmlI(self,nodeName,cellNodes,border,boundary,cellType,nr,fixed):  
        geo = self.getGeometry()   
        if not geo:
            return  
        if geo[2]:
            self.walls.reverse()
        cellNode = ET.SubElement(cellNodes, nodeName \
                                 , boundary=str(boundary) \
                                 , cell_type=cellType \
                                 , target_area=str(geo[0]) \
                                 , lambda_celllength="0" \
                                 , at_boundary=str(border==0).lower() \
                                 , dead="false" \
                                 , target_length=str(geo[1]) \
                                 , stiffness="0" \
                                 , index=str(nr) \
                                 , source="false" \
                                 , pin_fixed=str(fixed).lower() \
                                 , area=str(geo[0]) \
                                 , fixed=str(fixed).lower() \
                                 , div_counter="0" \
                                 )
        nextNode = self.firstNode
        ET.SubElement(cellNode, "node" , n=str(nextNode.getNr()))
        for wall in self.walls:
            if wall.getNode(1) == nextNode:
                nextNode=wall.getNode(2)
            else:
                nextNode=wall.getNode(1)
            if  nextNode != self.firstNode:
                ET.SubElement(cellNode, "node" , n=str(nextNode.getNr()))
#        for wall in self.walls:
        for wall in self.cellWalls:
            ET.SubElement(cellNode, "wall" , w=str(wall.getNr()))

    def toBoundaryBolygon(self,cellNodes):
        self.toXmlI("boundary_polygon",cellNodes,0,0,"0",-1,False)

    def getGeometry(self):
        nextNode = self.firstNode
        coords = list()
        coords.append(nextNode.getTuple())
        for wall in self.walls:
            if wall.getNode(1) == nextNode:
                nextNode=wall.getNode(2)
            else:
                nextNode=wall.getNode(1)
            if  nextNode != self.firstNode:
                coords.append(nextNode.getTuple())
        try:
            polygon = Polygon(coords)
        except Exception as e:
            return ()
        counterclock = orient(polygon, sign=1.0)
        reverse = polygon.exterior.coords[1] != counterclock.exterior.coords[1]
        minx, miny, maxx, maxy = polygon.minimum_rotated_rectangle.bounds
        width = maxx - minx
        height = maxy - miny
        diagonal = hypot(width, height)*1.2
        return (polygon.area, diagonal, reverse)
