from math import sqrt
import xml.etree.ElementTree as ET

class Mesh:

    def __init__(self):
        self.walls=list()
        self.nodes=list()
        self.cells=list()
        self.nodeNr=0
        self.wallNr=0
        self.cellNr=0
        self.mul=0.75
        
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
                    print("not successful!")
        self.removeCircumverence()
                    
        
    def removeCircumverence(self):
        longest = 0.0
        self.boundary_polygon = None
        for cell in self.cells:
            size=cell.circumverence()
            if size>longest:
                longest=size
                self.boundary_polygon=cell
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
        for wall in self.walls:
            wall.toXml(docWalls)
        return docLeaf
        
        
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
        
    def addNode(self, x, y):
        self.addNodeO(self.mesh.getNode(x,y)) 

    def addNodeO(self, node):
        if self.lastNode is None:
               self.lastNode = node
               self.firstNode = node
        else:
             wall = self.mesh.getWall(self.lastNode,node)
             self.appendWall(wall)
             wall.addCell(self)
             self.lastNode = node
             
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
        for wall in self.walls:
            if wall.isBorder():
                border = 1
        cellType = "0"
        if self.type == "#ffffff":
            cellType = "1"
        if self.type == "#0000f8":
            cellType = "2"
        if self.type == "#009000":
            cellType = "3"
        self.toXmlI("cell",cellNodes,border,cellType,self.nr,self.type == "#000000")

    def toXmlI(self,nodeName,cellNodes,border,cellType,nr,fixed):        
        cellNode = ET.SubElement(cellNodes, nodeName \
                                 , boundary=str(border) \
                                 , cell_type=cellType \
                                 , target_area="0" \
                                 , lambda_celllength="0" \
                                 , at_boundary=str(border==0).lower() \
                                 , dead="false" \
                                 , target_length="0" \
                                 , stiffness="0" \
                                 , index=str(nr) \
                                 , source="false" \
                                 , pin_fixed=str(fixed).lower() \
                                 , area="0" \
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
        for wall in self.walls:
            ET.SubElement(cellNode, "wall" , w=str(wall.getNr()))

    def toBoundaryBolygon(self,cellNodes):
        self.toXmlI("boundary_polygon",cellNodes,0,"0",-1,False)


