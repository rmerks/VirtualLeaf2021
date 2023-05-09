from math import sqrt

class Mesh:

    def __init__(self):
        self.walls=list()
        self.nodes=list()
        self.cells=list()
        self.nodeNr=1
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
        wall = Wall(node1,node2)
        self.walls.append(wall)
        return wall
    
    def getCell(self):
        cell = Cell(self)
        self.cells.append(cell)
        return cell

    def addWall(self, wall):
        self.walls.append(wall)

    
    def defineInnerCells(self):
        for wall in self.walls:
            if wall.isBorder():
                cell = self.getCell()
                cell.defineInnerCell(wall)
        print("first")
        for cell in self.cells:
            if cell.retryDefineInnerCell() == None:
                print(cell)
        print("next")
        for cell in self.cells:
            if cell.retryDefineInnerCell() == None:
                print(cell)

class Node:
    def __init__(self,x,y,nr):
        self.id= nr
        self.x=x
        self.y=y
        self.walls=list()
        
    def distance(self, x,y):
        return sqrt(pow(self.x-x,2) + pow(self.y-y,2))
        
    def addWall(self, wall):
        self.walls.append(wall)
        
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
        
class Wall:
    def __init__(self, node1, node2):
        self.node1 = node1
        self.node2 = node2
        self.node1.addWall(self)
        self.node2.addWall(self)
        self.cells=list()
        
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

class Cell:
    def __init__(self, mesh):
        self.walls = list()
        self.mesh = mesh
        self.firstNode = None
        self.lastNode = None
        
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
        