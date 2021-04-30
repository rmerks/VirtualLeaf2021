/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

/*** TISSUEGENERATOR.H ***/

#include <vector>
#include <QString>
#include <QtXml>
#include <canvas.h>
/* #include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>*/

using namespace std;

#ifndef __TISSUEGENERATOR_H
#define __TISSUEGENERATOR_H

#ifndef NULL
#define NULL 0
#endif

#define DELETED -2
class TissueGenerator {

public:
    /* methods */
    TissueGenerator();
    QString GenerateTissue(Main *m);
    int Dialog(void);
    void GenerateParams(Main *m);
    void AddHexagons(void);
    void AddRectangles(void);
    void AddNodeToTissue(QDomElement&, double, double, const char*, const char*, const char*);
    void AddCellToTissue(QDomElement&, int, double, const char*, int);
    vector<int> ConstructHexagonalBoundary(vector<int>, vector<int>, vector<int>, vector<int>, vector<int>, vector<int>);
    void AddBoundaryToTissue(QDomElement&, vector<int>, double);
    void AddWallToTissue(QDomElement&, int, int, int, int, int, double);
    void FinishTissue(void);
private:
     QDomDocument doc;
     QDomElement root_node;

     QString hFileName;
     QDir tissuedir;

     int polygon;
     int rows;
     int cols;
     double edgeL;
     double edgeW;
      QLocale standardlocale;
};
#endif  


