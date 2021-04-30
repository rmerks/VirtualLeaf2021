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

/*** TISSUEGENERATOR.CPP ***/

#include <QFile>
#include <vector>
#include <QVector>
#include <string>
#include <cstring>
#include <QString>
#include <QStringList>
#include <QtEndian>
#include <QByteArray>
#include <QDir>
#include <QLinkedList>
#include <QMap>
#include <QPrinter>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <qstatusbar.h>

#include <QInputDialog>

/*#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlstring.h>*/

#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <parameter.h>
#include <canvas.h>

#include "tissuegenerator.h"

using namespace std;

TissueGenerator::TissueGenerator() {

    polygon = 6;
    rows = 1;
    cols = 1;
    edgeL = 1.;
    edgeW = 1.;
    standardlocale = QLocale(QLocale::C);

}
/* store generated tissue in an xml file */
QString TissueGenerator::GenerateTissue(Main *m) {
    if (Dialog()){
    	return "";
    }
    GenerateParams(m);
    if (polygon == 4) { AddRectangles(); }
    if (polygon == 6) { AddHexagons(); }
    FinishTissue();
    return(hFileName);
}

/* popupscreens used to change parameters and store the tissue */
int TissueGenerator::Dialog(void) {
    filedialog:
    QFileDialog *fd = new QFileDialog( 0, "Save new file as:", QString(tissuedir.dirName()), QString("LeafML files (*.xml)"));
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setAcceptMode(QFileDialog::AcceptSave);
  //  fd->setDir( tissuedir );

    if ( fd->exec() == QDialog::Accepted ) {
        QStringList files = fd->selectedFiles();
        if (!files.isEmpty())
            hFileName = files[0];
       // hFileName = fd->selectedFile();
        if (hFileName.isEmpty()) { goto filedialog; }
        // extract extension from rFileName
        QFileInfo fi(hFileName);
        QString extension = fi.suffix();

        if (extension.isEmpty()) {
            extension = "xml";
            hFileName += ".";
            hFileName += extension;
        }

        if ( QFile::exists( hFileName )) {
            if ( QMessageBox::question( 0, QString("Overwrite File? -- Tissue Growth"), QString("A file called %1 already exists."
            " Do you want to overwrite it?").arg( hFileName ), QString("&Yes"), QString("&No")) ) { goto filedialog; }
        }

        tissuedialog:
        bool ok;
        polygon = QInputDialog::getInt(0, QString("Hexagonal or Rectangular tissue"), QString("type of polygon"),0,4,6,2,&ok);
        if (!ok || (polygon != 4 && polygon != 6)) {
            QMessageBox::warning(0, QString("Warning"), QString("choose 4 or 6"),QMessageBox::Ok,0);
            goto tissuedialog;
        }
        rows = QInputDialog::getInt(0, QString("Determine the amount of rows of cells"), QString("amount of rows"),1,10,1000,1,&ok);
        if (!ok) {
            QMessageBox::warning(0, QString("Warning"), QString("rows should have a value between 1 and 100"),QMessageBox::Ok,0);
            goto tissuedialog;
        }
        cols = QInputDialog::getInt(0, QString("Determine the amount of columns of cells"), QString("amount of columns"),1,10,1000,1,&ok);
        if (!ok) {
            QMessageBox::warning(0, QString("Warning"), QString("columns should have a value between 1 and 100"),QMessageBox::Ok,0);
            goto tissuedialog;
        }
        edgeL = QInputDialog::getDouble(0, QString("Determine the length of the edges"), QString("edge length"),0,1,100.00,2,&ok);
        if (!ok) {
            QMessageBox::warning(0, QString("Warning"), QString("The edge length should have a value between 0.1 and 100"),QMessageBox::Ok);
            goto tissuedialog;
        }
        if (polygon == 4) {
            edgeW = QInputDialog::getDouble(0, QString("Determine the width of the edges"), QString("edge width"),0,1,100.00,2,&ok);
            if (!ok) {
                QMessageBox::warning(0, QString("Warning"), QString("The edge width should have a value between 0.1 and 100"),QMessageBox::Ok);
                goto tissuedialog;
            }
        }
    	return 0;
    }
    return 1;
}

/* adds some parameters to the file */
void TissueGenerator::GenerateParams(Main *m) {

    /* Creates a new document, a node and set it as a root node */
    doc = QDomDocument("leaf");
    root_node = doc.createElement("leaf");
    doc.appendChild(root_node);

    /* stores Tissue attributes */
    //xmlNewProp(root_node, BAD_CAST "name", BAD_CAST hFileName.toStdString().c_str());
    root_node.setAttribute("name",hFileName);
    time_t t;
    std::time(&t);
    QString tstring(asctime(localtime(&t)));
 //   char *tstring = strdup(asctime(localtime(&t))); // but this does replace "end of line character by '\0'
   // char *eol=strchr(tstring,'\n');
    //if (eol!=NULL) * eol='\0';
    //xmlNewProp(root_node, BAD_CAST "date", BAD_CAST tstring);
    root_node.setAttribute("date",tstring);
   // free(tstring);

    //ostringstream text;
    //text << 0;
    //xmlNewProp(root_node, BAD_CAST "simtime", BAD_CAST text.str().c_str());
    root_node.setAttribute("simtime","0");
    extern Parameter par;
    par.outlinewidth=0.1; // set outlinewidth to a decent value matching with the default settings
    par.XMLAdd(doc,root_node);

    // add Settings and unset walls by default
    m->setCheckedShowFluxes(false);
   m->setCheckedShowTransporters(false);
    QDomElement settings=m->XMLSettingsTree();
}

void TissueGenerator::AddHexagons(void) {

    /* stores nodes */

   // QDomElement xmlnodes = doc.createElement("nodes",NULL);
    QDomElement xmlnodes = doc.createElement("nodes");
    root_node.appendChild(xmlnodes);
    { //ostringstream text;
      //text << (2*rows + 2*cols + 2*rows*cols);
      //xmlNewProp(xmlnodes, BAD_CAST "n", BAD_CAST text.str().c_str());
        xmlnodes.setAttribute("n",standardlocale.toString(2*rows + 2*cols + 2*rows*cols));
    }

    { //ostringstream text;
      //text << 3;
      //(xmlnodes, BAD_CAST "target_length", BAD_CAST text.str().c_str());
        xmlnodes.setAttribute("target_length","3");
    }


    /* loops through nodes */
    // vectors for storing boundary nodes
    int NodeIndex = 0;
    vector<int> BoundaryTop1;
    vector<int> BoundaryTop2;
    vector<int> BoundaryLeft;
    vector<int> BoundaryRight;
    vector<int> BoundaryBottom1;
    vector<int> BoundaryBottom2;
    double factor_x = 0.866 * edgeL; // approximation of root of 3 divided by 2
    double coordY = 0;

    for (int n = 1; n != (cols+1); n++) { // first row
        AddNodeToTissue(xmlnodes, ((n*2)*factor_x), coordY, "false", "true", "false");
        BoundaryTop1.push_back(NodeIndex++);
    }
    coordY += 0.5 * edgeL;
    for (int n = 0; n != (cols+1); n++) { // second row
        AddNodeToTissue(xmlnodes, ((n*2+1)*factor_x), coordY, "false", "true", "false");
        BoundaryTop2.push_back(NodeIndex++);
    }
    for (int r = 1; r != (rows*2-1); r++) {
        if (r%4 == 0 || r%4 == 1) {
            if (r%4 == 0) { coordY += 0.5 * edgeL; } else { coordY += edgeL; } // update coordY
            for (int n = 0; n != (cols+1); n++) {
                if (n != 0 && n != cols) { AddNodeToTissue(xmlnodes, ((n*2+1)*factor_x), coordY, "false", "false", "false");
                } if (n == 0) { // left boundary
                    AddNodeToTissue(xmlnodes, ((n*2+1)*factor_x), coordY, "false", "true", "false");
                    BoundaryLeft.push_back(NodeIndex);
                } if (n == cols) { // right boundary
                    AddNodeToTissue(xmlnodes, ((n*2+1)*factor_x), coordY, "false", "true", "false");
                    BoundaryRight.push_back(NodeIndex);
                }
                NodeIndex++;
            }
        } else {
            if (r%4 == 2) { coordY += 0.5 * edgeL; } else { coordY += edgeL; } // update coordY
            for (int n = 0; n != (cols+1); n++) {
                if (n != 0 && n != cols) { AddNodeToTissue(xmlnodes, ((n*2)*factor_x), coordY, "false", "false", "false");
                } if (n == 0) { // left boundary
                    AddNodeToTissue(xmlnodes, ((n*2)*factor_x), coordY, "false", "true", "false");
                    BoundaryLeft.push_back(NodeIndex);
                } if (n == cols) { // right boundary
                    AddNodeToTissue(xmlnodes, ((n*2)*factor_x), coordY, "false", "true", "false");
                    BoundaryRight.push_back(NodeIndex);
                }
                NodeIndex++;
            }
        }
    }
    // second-to-last row
    coordY += edgeL;
    for (int n = 0; n != (cols+1); n++) {
        if (rows%2 == 0) { AddNodeToTissue(xmlnodes, ((n*2)*factor_x), coordY, "false", "true", "false");
        } else { AddNodeToTissue(xmlnodes, ((n*2+1)*factor_x), coordY, "false", "true", "false");
        } BoundaryBottom1.push_back(NodeIndex++);
    }
    // last row
    coordY += 0.5 * edgeL;
    if (rows%2 == 0) {
        for (int n = 0; n != (cols); n++) {
            AddNodeToTissue(xmlnodes, ((n*2+1)*factor_x), coordY, "false", "true", "false");
            BoundaryBottom2.push_back(NodeIndex++);
        }
    } else {
        for (int n = 1; n != (cols+1); n++) {
            AddNodeToTissue(xmlnodes, ((n*2)*factor_x), coordY, "false", "true", "false");
            BoundaryBottom2.push_back(NodeIndex++);
        }
    }


    /* stores cells */
    double area = 2.598 * edgeL * edgeL;
    QDomElement xmlcells = doc.createElement("cells");
    root_node.appendChild(xmlcells);
   // QDomElement xmlcells = xmlNewChild(root_node, NULL, BAD_CAST "cells",NULL);
    {
      //ostringstream text;
     // text << (rows * cols);
      //xmlNewProp(xmlcells, BAD_CAST "n", BAD_CAST text.str().c_str());
        xmlcells.setAttribute("n",standardlocale.toString(rows * cols));
    }{
      //ostringstream text;
      //text << 0;
      //xmlNewProp(xmlcells, BAD_CAST "offsetx", BAD_CAST text.str().c_str());
        xmlcells.setAttribute("offsetx","0");
    }{
     /* ostringstream text;
      text << 0;
      xmlNewProp(xmlcells, BAD_CAST "offsety", BAD_CAST text.str().c_str());*/
      xmlcells.setAttribute("offsety","0");
    }/*{
      ostringstream text;
      text << 1;
      xmlNewProp(xmlcells, BAD_CAST "magnification", BAD_CAST text.str().c_str());
    }*/
     xmlcells.setAttribute("magnification","1");

   /*
    {
      ostringstream text;
      text << area;
      xmlNewProp(xmlcells, BAD_CAST "base_area", BAD_CAST text.str().c_str());
    }
*/
     xmlcells.setAttribute("base_area",standardlocale.toString(area));
    /* {
      ostringstream text;
      text << 1;
      xmlNewProp(xmlcells, BAD_CAST "nchem", BAD_CAST text.str().c_str());
    }*/
     xmlcells.setAttribute("nchem","1");

    for (int cell=0; cell != (rows*cols); cell++) {
        if (cell < cols || cell > ((rows-1)*cols) || cell%cols == 0 || cell%cols == (cols-1)) {
            AddCellToTissue(xmlcells, cell, area, "true", 0);
        } else { AddCellToTissue(xmlcells, cell, area, "false", 0); }
    }
    vector<int> BoundaryVector = ConstructHexagonalBoundary(BoundaryTop1, BoundaryTop2, BoundaryLeft, BoundaryRight, BoundaryBottom1,BoundaryBottom2);
    AddBoundaryToTissue(xmlcells,BoundaryVector, (rows*cols*area));


    /* stores walls */
    if (!(rows == 1 && cols == 1)) { // more than one cell
        int walls = (3*rows*cols - 3);
        if (rows == 1 || cols == 1) { walls -= 2; }

        //xmlNodePtr xmlwalls = xmlNewChild(root_node, NULL, BAD_CAST "walls",NULL);
       QDomElement xmlwalls = doc.createElement("walls");
       root_node.appendChild(xmlwalls);
       /* {
          ostringstream text;
          text << walls;
          xmlNewProp(xmlwalls, BAD_CAST "n", BAD_CAST text.str().c_str());
        }*/
       xmlwalls.setAttribute("n",standardlocale.toString(walls));
       root_node.appendChild(xmlwalls);

        int wall = 0;
        int c1 = 0;
        int c2 = cols;
        int n1 = 2*cols+1;
        int n2 = 3*cols+3;
        int temp = 0;

        // first: add 'horizontal' walls
        while (wall != ((2*cols) -1)*(rows -1)) {

            AddWallToTissue(xmlwalls, wall, c1, c2, n1, n2, edgeL);
            wall++;
            if (wall%(2*cols-1) != 0) { // != end of one row
                if (wall%2 == 0) { c1++; } else { c2++; } // update cells
                temp = n1; n1 = n2; n2 = (temp+1); //update rows
            } else {
                c1++; c2++; // update cells
                if ((c1/cols)%2 == 0) { // check for (un)even rows
                    n1 += 2; n2 += (2*cols +4); //update rows
                }else {
                    n1 += (2*cols +4); n2 += 2; //update rows
                }
            }
        }

        // second: add vertical walls
        int i = 1; c1 = 0; c2 = 1; n1 = (cols+1); n2 = (2*cols)+2;
        if (cols == 1) { n1 = 6; n2 = 8; } // exception on initial n1 and n2
        for (; i != ((rows*cols)-1); i++){
            if (i%cols != (cols-1) && i%cols != 0) { // != begin and != end of one row
                AddWallToTissue(xmlwalls, wall, c1, c2, n1, n2, edgeL);
                wall++; c1++; c2++; n1++; n2++;
            } else {
                if (i%cols == (cols-1)) { // end of row
                    if (((i+1)/cols)%2 == 1) { // row# is uneven
                        AddWallToTissue(xmlwalls, wall, c1, c2, n1, n2, edgeL);
                        wall++; c1+=2; c2+=2; n1+=(cols+4); n2+=(cols+4);
                    } else { // row# is even
                        AddWallToTissue(xmlwalls, wall, c1, -1, n1, n2, edgeL);
                        wall++; n1+=(cols+2); n2+=(cols+2);
                    }
                } else { // beginning of row
                    if ((i/cols)%2 == 1) { // row# is even
                        AddWallToTissue(xmlwalls, wall, c1, c2, n1, n2, edgeL);
                        wall++; c1++; c2++; n1++; n2++;
                    } else { // row# is uneven
                        if ((i/cols) == (rows-1)) { c1++; c2++; n1++; n2++; } // final uneven row
                        AddWallToTissue(xmlwalls, wall, -1, c2, n1, n2, edgeL);
                        wall++; c1++; c2++; n1++; n2++;
                    }
                }
            }
        } if (rows == 1) { AddWallToTissue(xmlwalls, wall, c1, c2, n1, n2, edgeL); wall++; } // one row

        // third: add walls on the top and on the bottom
        double TBLength = 1.732 * edgeL; // = square root of 3
        if (cols > 2) {
            n1=(cols+1); n2=(cols+2);
            for (c1 = 1; c1 != (cols-1); c1++, wall++, n1++, n2++) { //top
                AddWallToTissue(xmlwalls, wall, c1, -1, n1, n2, TBLength);
            } n1=(2*rows*(cols+1)); n2=(n1+1);
            for (c1=(((rows-1)*cols)+1); c1 != ((rows*cols)-1); c1++, wall++, n1++, n2++) { //bottom
                AddWallToTissue(xmlwalls, wall, c1, -1, n1, n2, TBLength);
            }
        }

        // fourth: add some longer walls to the outer left and right
        if (rows > 2) {
            for (int row = 2; row != rows; row++) {
                if (row%2 == 0) {
                    AddWallToTissue(xmlwalls, wall, (cols*(row-1)), -1, ((2*row-2)*(cols+1)-1), ((2*row+1)*(cols+1)-1), (2*edgeL));
                } else {
                    AddWallToTissue(xmlwalls, wall, ((cols*row)-1), -1, ((2*row-1)*(cols+1)-2), ((2*row+2)*(cols+1)-2), (2*edgeL));
                }
                wall++;
            }
        }

        // fifth: add all corner walls
        if (!(rows == 1 || cols == 1)) { // lines of cells do not have corner walls
            AddWallToTissue(xmlwalls, wall++, 0, -1, (cols+1), ((2*cols)+1), (2*edgeL)); // upperleft wall
            AddWallToTissue(xmlwalls, wall++, (cols-1), -1, ((2*cols)-1), ((4*cols)+2), TBLength); // upperright wall
            if (rows%2 == 0) { // even number of rows
                AddWallToTissue(xmlwalls, wall++, ((rows-1)*cols), -1, ((2*rows-2)*(cols+1)-1), (2*rows*(cols+1)), TBLength); // lowerleft wall
                AddWallToTissue(xmlwalls, wall++, ((rows*cols)-1), -1, ((2*rows)*(cols+1)-2), ((2*rows+1)*(cols+1)-3), (2*edgeL)); // lowerright wall
            } else { // uneven number of rows
                AddWallToTissue(xmlwalls, wall++, ((rows-1)*cols), -1, ((2*rows-1)*(cols+1)-1), (2*rows*(cols+1)), (2*edgeL)); // lowerleft wall
                AddWallToTissue(xmlwalls, wall++, ((rows*cols)-1), -1, ((2*rows-1)*(cols+1)-2), ((2*rows+1)*(cols+1)-3), TBLength); // lowerright wall
            }
        }

    } else { // single cell, thus 0 walls

        //xmlNodePtr xmlwalls = xmlNewChild(root_node, NULL, BAD_CAST "walls",NULL);
        QDomElement xmlwalls = doc.createElement("walls");
        root_node.appendChild(xmlwalls);
        /*{
          ostringstream text;
          text << 0;
          xmlNewProp(xmlwalls, BAD_CAST "n", BAD_CAST text.str().c_str());
        }*/
        xmlwalls.setAttribute("n","0");

    }

    /* stores 'nodesets' */
    //xmlNodePtr xmlnodesets = xmlNewChild(root_node, NULL, BAD_CAST "nodesets",NULL);
    QDomElement xmlnodesets = doc.createElement("nodesets");
    root_node.appendChild(xmlnodesets);
    xmlnodesets.setAttribute("n","0");
    /*
    {
      ostringstream text;
      text << 0;
      xmlNewProp(xmlnodesets, BAD_CAST "n", BAD_CAST text.str().c_str());
    }
*/
  //  return;
}

void TissueGenerator::AddRectangles(void) {

    /* stores nodes */
   // xmlNodePtr xmlnodes = xmlNewChild(root_node, NULL, BAD_CAST "nodes",NULL);
    QDomElement xmlnodes = doc.createElement("nodes");
    root_node.appendChild(xmlnodes);
    xmlnodes.setAttribute("n",standardlocale.toString((rows+1)*(cols+1)));
   /* { ostringstream text;
      text << ((rows+1)*(cols+1));
      xmlNewProp(xmlnodes, BAD_CAST "n", BAD_CAST text.str().c_str());
    }*/
    xmlnodes.setAttribute("target_length","3");
   /* { ostringstream text;
      text << 3;
      xmlNewProp(xmlnodes, BAD_CAST "target_length", BAD_CAST text.str().c_str());
    }*/


    /* loops through nodes */
    vector<int> BoundaryVec1; //nodes added in correct order
    vector<int> BoundaryVec2; //nodes added in reversed order
    int nodeindex = 0;
    for (int c = 0; c != (cols+1); c++) {
        for (int r = 0; r != (rows+1); r++) {
            if (r == 0 || r == rows || c == 0 || c == cols) { // at boundary
                if ((c == 0 || r == rows) && c != cols) { BoundaryVec1.push_back(nodeindex); }
                if ((c == cols || r == 0) && c != 0) { BoundaryVec2.push_back(nodeindex); }
                AddNodeToTissue(xmlnodes, (r*edgeL), (c*edgeW), "false", "true", "false");
            } else { AddNodeToTissue(xmlnodes, (r*edgeL), (c*edgeW), "false", "false", "false"); }
            nodeindex++;
        }
    }

    /* stores cells */
   // xmlNodePtr xmlcells = xmlNewChild(root_node, NULL, BAD_CAST "cells",NULL);
    QDomElement xmlcells = doc.createElement("cells");
    root_node.appendChild(xmlcells);
    xmlcells.setAttribute("n",standardlocale.toString(rows * cols));
    xmlcells.setAttribute("offsetx","0");
    xmlcells.setAttribute("offsety","0");
      xmlcells.setAttribute("magnfication","1.");
        xmlcells.setAttribute("base_area",standardlocale.toString(edgeL * edgeW));
           xmlcells.setAttribute("nchem","1");
    /*{
      ostringstream text;
      text << (rows * cols);
      xmlNewProp(xmlcells, BAD_CAST "n", BAD_CAST text.str().c_str());
    }*/
    /*{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlcells, BAD_CAST "offsetx", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlcells, BAD_CAST "offsety", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 1;
      xmlNewProp(xmlcells, BAD_CAST "magnification", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << (edgeL * edgeW);
      xmlNewProp(xmlcells, BAD_CAST "base_area", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 1;
      xmlNewProp(xmlcells, BAD_CAST "nchem", BAD_CAST text.str().c_str());
    }*/

    /* loops through cells */
    int cell = 0;
    for (int r = 0; r != rows; r++) {
        for (int c = 0; c != cols; c++) {
            if (r == 0 || r == (rows-1) || c == 0 || c == (cols-1)) { // at boundary
                AddCellToTissue(xmlcells, cell, (edgeL*edgeW), "true", 0); // abs((r%2)-(cell%2)) = checkerboard pattern
            } else { AddCellToTissue(xmlcells, cell, (edgeL*edgeW), "false", 0); } // abs((r%2)-(cell%2)) = checkerboard pattern
            cell++;
        }
    }

    while (BoundaryVec2.size() != 0) {
        BoundaryVec1.push_back(BoundaryVec2.back());
        BoundaryVec2.pop_back();
    }
    AddBoundaryToTissue(xmlcells,BoundaryVec1, (rows*cols*edgeL*edgeW));


    /* stores walls */
    int walls = (2*rows*cols + rows + cols -4);
    if (rows == 1 || cols == 1) { walls -= 2; }
    if (rows == 1 && cols == 1) { walls = 0; }
   // xmlNodePtr xmlwalls = xmlNewChild(root_node, NULL, BAD_CAST "walls",NULL);
    QDomElement xmlwalls = doc.createElement("walls");
    root_node.appendChild(xmlwalls);
    xmlwalls.setAttribute("n",standardlocale.toString(walls));

   /* {
      ostringstream text;
      text << walls;
      xmlNewProp(xmlwalls, BAD_CAST "n", BAD_CAST text.str().c_str());
    }*/


    /* loops through walls */
    int wall = 0;
    int c1 = -1, c2 = -1;
    // add horizontal walls
    for (int c = 0; c != (cols+1); c++) {
        for (int r = 0; r != (rows); r++) {
            if (c != 0) { c1++; }
            if (c != cols) { c2++; }
            else { c2 = -1; }
            if (!(cols == 1 && (r == 0 || r == (rows-1))) && !(rows == 1 && (c == 0 || c == cols)) // line of cells
                && !((c == 0 || c == (cols)) && (r == 0 || r == (rows-1)))) { // corners
                AddWallToTissue(xmlwalls, wall, c1, c2, (c*(rows+1)+r), (c*(rows+1)+(r+1)), edgeL);
                wall++;
            }
        }
    }
    // add vertical walls
    for (int c = 0; c != (cols); c++) {
        for (int r = 0; r != (rows+1); r++) {
            c1 = c2;
            c2++;
            if (r == rows) { c2 = -1; }
            if (r == 0) { c2 = (rows*c); }
            if (!(cols == 1 && (r == 0 || r == rows)) && !(rows == 1 && (c == 0 || c == (cols-1))) // line of cells
                && !((c == 0 || c == (cols-1)) && (r == 0 || r == (rows)))) { // corners
                AddWallToTissue(xmlwalls, wall, c1, c2, (c*(rows+1)+r), ((c+1)*(rows+1)+r), edgeW);
                wall++;
            }
        }
    }
    // add corners
    if (rows != 1 && cols !=1) {
        double edgeD = hypot(edgeL, edgeW);
        AddWallToTissue(xmlwalls, wall, -1, 0, 1, (rows+1), edgeD); wall++; // topleft
        AddWallToTissue(xmlwalls, wall, -1, (rows-1), (rows-1), ((2*rows)+1), edgeD); wall++; // topright
        AddWallToTissue(xmlwalls, wall, -1, ((cols-1)*rows), ((cols-1)*(rows+1)), ((cols*(rows+1))+1), edgeD); wall++; // bottomleft
        AddWallToTissue(xmlwalls, wall, -1, ((cols*rows)-1), ((cols*(rows+1))-1), ((cols+1)*(rows+1)-2), edgeD); //bottomright
    }

    /* stores 'nodesets' */
    QDomElement xmlnodesets = doc.createElement("nodesets");
    root_node.appendChild(xmlnodesets);
    xmlnodes.setAttribute("n","0");
  /*  {
      ostringstream text;
      text << 0;
      xmlNewProp(xmlnodesets, BAD_CAST "n", BAD_CAST text.str().c_str());
    }

    return;*/
}

void TissueGenerator::AddNodeToTissue(QDomElement &parent, double coord_x, double coord_y, const char* Nfixed, const char* Nboundary, const char* Nsam){
    QDomElement xmlnode = doc.createElement("node");
    parent.appendChild(xmlnode);
    xmlnode.setAttribute("x",standardlocale.toString(coord_x));
    xmlnode.setAttribute("y",standardlocale.toString(coord_y));
    xmlnode.setAttribute("fixed",QString(Nfixed));
    xmlnode.setAttribute("boundary",QString(Nboundary));
    xmlnode.setAttribute("sam",QString(Nsam));

/*
    {
      ostringstream text;
      text << coord_x;
      xmlNewProp(xmlnode, BAD_CAST "x", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << coord_y;
      xmlNewProp(xmlnode, BAD_CAST "y", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << Nfixed;
      xmlNewProp(xmlnode, BAD_CAST "fixed", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << Nboundary;
      xmlNewProp(xmlnode, BAD_CAST "boundary", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << Nsam;
      xmlNewProp(xmlnode, BAD_CAST "sam", BAD_CAST text.str().c_str());
    }
*/
    return;
}

void TissueGenerator::AddCellToTissue(QDomElement &parent, int index, double area, const char* at_boundary, int cell_type) {

    QDomElement xmlcell = doc.createElement("cell");
    parent.appendChild(xmlcell);
    xmlcell.setAttribute("index",standardlocale.toString(index));
    xmlcell.setAttribute("area",standardlocale.toString(area));
    xmlcell.setAttribute("target_area",standardlocale.toString(area));
    xmlcell.setAttribute("target_length","3");
    xmlcell.setAttribute("lambda_celllength","0");
    xmlcell.setAttribute("stiffness","0");
    xmlcell.setAttribute("fixed","false");
    xmlcell.setAttribute("pin_fixed","false");
    xmlcell.setAttribute("at_boundary",QString(at_boundary));
    xmlcell.setAttribute("dead","false");
    xmlcell.setAttribute("source","false");
    xmlcell.setAttribute("boundary","None");
    xmlcell.setAttribute("div_counter","0");
    xmlcell.setAttribute("cell_type",standardlocale.toString(cell_type));
   /* {
      ostringstream text;
      text << index;
      xmlNewProp(xmlcell, BAD_CAST "index" , BAD_CAST text.str().c_str() );
    }{
      ostringstream text;
      text << area;
      xmlNewProp(xmlcell, BAD_CAST "area", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << area;
      xmlNewProp(xmlcell, BAD_CAST "target_area", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 3;
      xmlNewProp(xmlcell, BAD_CAST "target_length", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlcell, BAD_CAST "lambda_celllength", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlcell, BAD_CAST "stiffness", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlcell, BAD_CAST "fixed", BAD_CAST text.str().c_str());
    }{

      ostringstream text;
      text << "false";
      xmlNewProp(xmlcell, BAD_CAST "pin_fixed", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << at_boundary;
      xmlNewProp(xmlcell, BAD_CAST "at_boundary", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlcell, BAD_CAST "dead", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlcell, BAD_CAST "source", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "None";
      xmlNewProp(xmlcell, BAD_CAST "boundary", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlcell, BAD_CAST "div_counter", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << cell_type;
      xmlNewProp(xmlcell, BAD_CAST "cell_type", BAD_CAST text.str().c_str());
    }*/

    /* stores nodes per rectangular cell */
    if (polygon == 4) {
        int cellrow = index%rows; // column in which the cell is located
        int cellcol = ((index-cellrow)/rows);
        {
          //ostringstream text;
          QDomElement node_xml = doc.createElement("node");
            xmlcell.appendChild(node_xml);
          //text << (cellcol*(rows+1)+cellrow);
         node_xml.setAttribute("n", standardlocale.toString(cellcol*(rows+1)+cellrow));

        }{
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol*(rows+1)+cellrow+1));
          //xmlNewProp(node_xml, BAD_CAST "n",standardlocale.toString BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString((cellcol+1)*(rows+1)+cellrow+1));
          //xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString((cellcol+1)*(rows+1)+cellrow));
       //   xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }
    }
    /* stores nodes per hexagonal cell */
    if (polygon == 6) {
        int cellcol = index%cols; // column in which the cell is located
        int cellrow = (index-cellcol)/rows; // row in which the cell is located
        int inrow = (2*cols+2); // amount of nodes in one row (except for the first)
        int last = 0;
        if (rows == (cellrow+1) && rows%2 == 1) { last = 1; }
        {
          //ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol + cellrow * inrow - cellrow%2));
          //xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol + (cellrow+0.5) * inrow));
        //  xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol + (cellrow+1) * inrow));
          //xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol + (cellrow+1.5) * inrow - cellrow%2 - last)); //?
         // xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol + (cellrow+1) * inrow -1));
          //xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }{
          ostringstream text;
          QDomElement node_xml = doc.createElement("node");
          xmlcell.appendChild(node_xml);
          node_xml.setAttribute("n",standardlocale.toString(cellcol + (cellrow+0.5) * inrow -1));
          //xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
        }
    }

    {
      //ostringstream text;
      QDomElement chem_xml = doc.createElement("chem");
      xmlcell.appendChild(chem_xml);
      //text << 1;
      //xmlNewProp(chem_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
      chem_xml.setAttribute("n","1");
    }

    return;
}

vector<int> TissueGenerator::ConstructHexagonalBoundary(vector<int> T1, vector<int> T2, vector<int> L, vector<int> R, vector<int> B1, vector<int> B2){

    // vector and iterators
    vector<int> Boundaryvector;
    int max = (T1.size()+T2.size());
    int t1 = 0;
    int t2 = 0;
    int b1 = B1.size() -1;
    int b2 = B2.size() -1;

    // add top nodes
    for (int t = 0; t != max; t++) {
        if (t%2 == 0) { Boundaryvector.push_back(T2[t2++]);
        } else { Boundaryvector.push_back(T1[t1++]); }
    }
    // add right nodes
    for (int r = 0; r != (int)R.size(); r++) {
        Boundaryvector.push_back(R[r]);
    }
    // add bottom nodes in reversed order
    for (int b = max; b != 0; b--) {
        if (b%2 == 0) { Boundaryvector.push_back(B2[b2--]);
        } else { Boundaryvector.push_back(B1[b1--]); }
    }
    // add left nodes in reversed order
    for (int l = (L.size()-1); l != -1; l--) {
        Boundaryvector.push_back(L[l]);
    }

    return Boundaryvector;
}

void TissueGenerator::AddBoundaryToTissue(QDomElement &parent, vector<int> BoundaryVector, double area){

    QDomElement xmlboundary = doc.createElement("boundary_polygon");
    parent.appendChild(xmlboundary);
    xmlboundary.setAttribute("index","-1");
    xmlboundary.setAttribute("area",standardlocale.toString(area));
    xmlboundary.setAttribute("target_area",standardlocale.toString(area));
    xmlboundary.setAttribute("target_length","3");
    xmlboundary.setAttribute("lambda_celllength","0");
    xmlboundary.setAttribute("stiffness","0");
    xmlboundary.setAttribute("fixed","false");
    xmlboundary.setAttribute("pin_fixed","false");
    xmlboundary.setAttribute("at_boundary","true");
    xmlboundary.setAttribute("dead","false");
    xmlboundary.setAttribute("source","false");
    xmlboundary.setAttribute("boundary","None");
    xmlboundary.setAttribute("div_counter","0");
    xmlboundary.setAttribute("cell_type","-1");
    /*{
      ostringstream text;
      text << -1;
      xmlNewProp(xmlboundary, BAD_CAST "index", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << area;
      xmlNewProp(xmlboundary, BAD_CAST "area", BAD_CAST text.str().c_str()); // is replaced with real area when Tissue is read.
    }{
      ostringstream text;
      text << area;
      xmlNewProp(xmlboundary, BAD_CAST "target_area", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 3;
      xmlNewProp(xmlboundary, BAD_CAST "target_length", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlboundary, BAD_CAST "lambda_celllength", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlboundary, BAD_CAST "stiffness", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlboundary, BAD_CAST "fixed", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlboundary, BAD_CAST "pin_fixed", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "true";
      xmlNewProp(xmlboundary, BAD_CAST "at_boundary", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlboundary, BAD_CAST "dead", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "false";
      xmlNewProp(xmlboundary, BAD_CAST "source", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "None";
      xmlNewProp(xmlboundary, BAD_CAST "boundary", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlboundary, BAD_CAST "div_counter", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << -1;
      xmlNewProp(xmlboundary, BAD_CAST "cell_type", BAD_CAST text.str().c_str());
    }
*/

    //* stores boundary nodes
    for (int bn = 0; bn != (int)BoundaryVector.size(); bn++) {
      {
       // ostringstream text;
        QDomElement node_xml = doc.createElement( "node");
        xmlboundary.appendChild(node_xml);
        //text << BoundaryVector[bn];
        node_xml.setAttribute("n", standardlocale.toString(BoundaryVector[bn]));
      }
    }{
      //ostringstream text;
      //text << 1;
      QDomElement chem_xml = doc.createElement("chem");
      xmlboundary.appendChild(chem_xml);
      chem_xml.setAttribute("n","1");
      //xmlNewProp(chem_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
    }

    //return;
}

void TissueGenerator::AddWallToTissue(QDomElement &parent, int index, int c1, int c2, int n1, int n2, double wallLength){

    QDomElement xmlwall = doc.createElement("wall");
    parent.appendChild(xmlwall);
    xmlwall.setAttribute("index",standardlocale.toString(index));
    xmlwall.setAttribute("c1",standardlocale.toString(c1));
    xmlwall.setAttribute("c2",standardlocale.toString(c2));
    xmlwall.setAttribute("n1",standardlocale.toString(n1));
    xmlwall.setAttribute("n2",standardlocale.toString(n2));
    xmlwall.setAttribute("length",standardlocale.toString(wallLength));
    xmlwall.setAttribute("viz_flux","0");
    xmlwall.setAttribute("wall_type","Normal");

   /*
    {
      ostringstream text;
      text << index;
      xmlNewProp(xmlwall, BAD_CAST "index" , BAD_CAST text.str().c_str() );
    }{
      ostringstream text;
      text << c1;
      xmlNewProp(xmlwall, BAD_CAST "c1", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << c2;
      xmlNewProp(xmlwall, BAD_CAST "c2", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << n1;
      xmlNewProp(xmlwall, BAD_CAST "n1", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << n2;
      xmlNewProp(xmlwall, BAD_CAST "n2", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << wallLength;
      xmlNewProp(xmlwall, BAD_CAST "length", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << 0;
      xmlNewProp(xmlwall, BAD_CAST "viz_flux", BAD_CAST text.str().c_str());
    }{
      ostringstream text;
      text << "Normal";
      xmlNewProp(xmlwall, BAD_CAST "wall_type", BAD_CAST text.str().c_str());
    }
*/

    QDomElement tr1_xml = doc.createElement("transporters1");
    xmlwall.appendChild(tr1_xml);
    tr1_xml.setAttribute("v","0");

   /* {
    ostringstream text;
    text << 0;
    xmlNewProp(tr1_xml, BAD_CAST "v", BAD_CAST text.str().c_str());
    }*/

    QDomElement tr2_xml = doc.createElement("transporters1");
    xmlwall.appendChild(tr2_xml);
    tr2_xml.setAttribute("v","0");

    /*QDomElement tr2_xml = xmlNewChild(xmlwall, NULL, BAD_CAST "transporters2", NULL);
    {
    ostringstream text;
    text << 0;
    xmlNewProp(tr2_xml, BAD_CAST "v", BAD_CAST text.str().c_str());
    }
*/

}

void TissueGenerator::FinishTissue() {

    //* prints document
   //  xmlKeepBlanksDefault(0);
    // xmlSaveFormatFileEnc(hFileName.toStdString().c_str(), doc, "UTF-8", 1);
    QFile xmlout(hFileName);
    if (!xmlout.open(QIODevice::WriteOnly | QIODevice::Text)) {
      throw("cannot open file for output");
    }
    QTextStream out(&xmlout);
    doc.save(out,2, QDomNode::EncodingFromDocument);
    xmlout.close();

}
