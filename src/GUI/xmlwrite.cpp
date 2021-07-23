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

#include "mesh.h"
#include "parameter.h"

#include <ctime>
#include <cstring>
#include <string>
#include <sstream>
#include <QString>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>
#include <QLocale>
#include "xmlwrite.h"
#include "nodeset.h"
#include "warning.h"
#include "output.h" 

static const std::string _module_id("$Id$");

using namespace MyWarning;

void ThrowStringStream(stringstream &s) {

  static char *msg = 0;
  msg = (char *)malloc((1+s.str().length())*sizeof(char));
  strcpy(msg,s.str().c_str());
  throw(msg);
}


// Add this cell to the XML tree
void Cell::XMLAdd(xmlNodePtr cells_node) const {

  // Save the cell to a stream so we can reconstruct its state later
  xmlNodePtr xmlcell = xmlNewChild(cells_node, NULL, BAD_CAST "cell",NULL);
  XMLAddCore(xmlcell);
}

void BoundaryPolygon::XMLAdd(xmlNodePtr parent_node) const {

  xmlNodePtr xmlcell = xmlNewChild(parent_node, NULL, BAD_CAST "boundary_polygon",NULL);
  XMLAddCore(xmlcell);
}

void NodeSet::XMLAdd(xmlNode *root) const {

  xmlNode *xmlnodeset = xmlNewChild(root, NULL, BAD_CAST "nodeset", NULL);

  for (list<Node *>::const_iterator i=begin();i!=end();i++) {
    {
      ostringstream text;
      xmlNodePtr node_xml = xmlNewChild(xmlnodeset, NULL, BAD_CAST "node", NULL);
      text << (*i)->Index();
      xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
    }
  }
}




void Cell::XMLAddCore(xmlNodePtr xmlcell) const {

  // properties

  {
    ostringstream text;
    text << index;
    xmlNewProp(xmlcell, BAD_CAST "index" , BAD_CAST text.str().c_str() );
  }
  {
    ostringstream text;
    text << area;
    xmlNewProp(xmlcell, BAD_CAST "area", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << target_area;
    xmlNewProp(xmlcell, BAD_CAST "target_area", BAD_CAST text.str().c_str());
  }


  {
    ostringstream text;
    text << target_length;
    xmlNewProp(xmlcell, BAD_CAST "target_length", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << lambda_celllength;
    xmlNewProp(xmlcell, BAD_CAST "lambda_celllength", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << stiffness;
    xmlNewProp(xmlcell, BAD_CAST "stiffness", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(fixed);
    xmlNewProp(xmlcell, BAD_CAST "fixed", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(pin_fixed);
    xmlNewProp(xmlcell, BAD_CAST "pin_fixed", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(at_boundary);
    xmlNewProp(xmlcell, BAD_CAST "at_boundary", BAD_CAST text.str().c_str());
  }


  {
    ostringstream text;
    text << bool_name(dead);
    xmlNewProp(xmlcell, BAD_CAST "dead", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(source);
    xmlNewProp(xmlcell, BAD_CAST "source", BAD_CAST text.str().c_str());
  }


  {
    ostringstream text;
    text << boundary_type_names[boundary];
    xmlNewProp(xmlcell, BAD_CAST "boundary", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << div_counter;
    xmlNewProp(xmlcell, BAD_CAST "div_counter", BAD_CAST text.str().c_str());
  }

  { 
    ostringstream text;
    text << cell_type;
    xmlNewProp(xmlcell, BAD_CAST "cell_type", BAD_CAST text.str().c_str());
  }

  for (list<Node *>::const_iterator i=nodes.begin();i!=nodes.end();i++) {
    {
      ostringstream text;
      xmlNodePtr node_xml = xmlNewChild(xmlcell, NULL, BAD_CAST "node", NULL);
      text << (*i)->Index();
      xmlNewProp(node_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
    }
  }

  for (list<Wall *>::const_iterator i=walls.begin();i!=walls.end();i++) {
    {
      ostringstream text;
      xmlNodePtr wall_xml = xmlNewChild(xmlcell, NULL, BAD_CAST "wall", NULL);
      text << XMLIO::list_index( m->walls.begin(), m->walls.end(), *i );
      xmlNewProp(wall_xml, BAD_CAST "w", BAD_CAST text.str().c_str());
    }
  }




  xmlNodePtr chem_xml = xmlNewChild(xmlcell, NULL, BAD_CAST "chem", NULL); 
  {
    ostringstream text;
    text << NChem();
    xmlNewProp(chem_xml, BAD_CAST "n", BAD_CAST text.str().c_str());
  }

  for (int i=0;i<NChem();i++) {
    xmlNodePtr chem_val_xml = xmlNewChild(chem_xml, NULL, BAD_CAST "val", NULL);
    { 
      ostringstream text;
      text << chem[i];
      xmlNewProp(chem_val_xml, BAD_CAST "v", BAD_CAST text.str().c_str());
    }
  }
}



void Node::XMLAdd(xmlNodePtr nodes_node) const { 

  // Save the node to a stream so we can reconstruct its state later
  xmlNodePtr xmlnode = xmlNewChild(nodes_node, NULL, BAD_CAST "node",NULL);

  {
    ostringstream text;
    text << x;
    xmlNewProp(xmlnode, BAD_CAST "x", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << y;
    xmlNewProp(xmlnode, BAD_CAST "y", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(fixed);
    xmlNewProp(xmlnode, BAD_CAST "fixed", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(boundary);
    xmlNewProp(xmlnode, BAD_CAST "boundary", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << bool_name(sam);
    xmlNewProp(xmlnode, BAD_CAST "sam", BAD_CAST text.str().c_str());
  }

  if (node_set) {
    ostringstream text;
    text << XMLIO::list_index(m->node_sets.begin(),m->node_sets.end(),node_set);
    xmlNewProp(xmlnode, BAD_CAST "nodeset", BAD_CAST text.str().c_str());    
  }
}

void Neighbor::XMLAdd(xmlNodePtr neighbors_node) const {

  xmlNodePtr xmlnode = xmlNewChild(neighbors_node, NULL, BAD_CAST "neighbor",NULL);
  {
    ostringstream text;
    text << cell->Index();
    xmlNewProp(xmlnode, BAD_CAST "cell", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << nb1->Index();
    xmlNewProp(xmlnode, BAD_CAST "nb1", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << nb2->Index();
    xmlNewProp(xmlnode, BAD_CAST "nb2", BAD_CAST text.str().c_str());
  }
}


// from example code on www.libxml.org:
xmlXPathObjectPtr
getnodeset (xmlDocPtr doc, xmlChar *xpath){

  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext(doc);
  if (context == NULL) {
    //printf("Error in xmlXPathNewContext\n");
    return NULL;
  }
  result = xmlXPathEvalExpression(xpath, context);
  xmlXPathFreeContext(context);
  if (result == NULL) {
    //printf("Error in xmlXPathEvalExpression\n");
    return NULL;
  }
  if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
    xmlXPathFreeObject(result);
    // printf("No result\n");
    return NULL;
  }
  return result;
}



int Cell::XMLRead(xmlNode *cur)
{

  xmlNode *n = cur->xmlChildrenNode;
  QLocale standardlocale(QLocale::C);
  bool ok;

  vector<int> tmp_nodes;

  while(n!=NULL) {
    if ((!xmlStrcmp(n->name, (const xmlChar *)"node"))) {
      xmlChar *nc = xmlGetProp(n, BAD_CAST "n");

      if (nc==0) {
	unique_warning("Token \"n\" not found in xmlwrite.cpp at or around line no. 966");
      }
      tmp_nodes.push_back(atoi( (char *)nc));
      xmlFree(nc);

    }
    n = n->next;
  }

  int nnodes = tmp_nodes.size();
  for (int i=0;i<nnodes;i++) {
    m->AddNodeToCell( this, 
		      m->nodes[tmp_nodes[i]], 
		      m->nodes[tmp_nodes[(nnodes+i-1)%nnodes]],
		      m->nodes[tmp_nodes[(i+1)%nnodes]] );

  }

  n = cur->xmlChildrenNode;
  while(n!=NULL) {
    if ((!xmlStrcmp(n->name, (const xmlChar *)"chem"))) {

      xmlNode *v_node = n->xmlChildrenNode;
      int nv=0;
      while (v_node!=NULL) {
	if ((!xmlStrcmp(v_node->name, (const xmlChar *)"val"))) {


	  if (nv>=Cell::NChem()) {
	    {
	      stringstream text;
	      text << "Exception in Mesh::XMLRead: Too many chemical values given for cell(s). Ignoring remaining values.";
	      unique_warning(text.str().c_str());
	      break;
	    }
	  }

	  xmlChar *nc = xmlGetProp(v_node, (const xmlChar *) "v");

	  if (nc==0) {
	    unique_warning("Token \"v\" not found in xmlwrite.cpp at or around line no. 1002");
	  }
	  double v=standardlocale.toDouble((char *)nc, &ok);
	  if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)nc);
	  chem[nv++]=v;
	  xmlFree(nc);
	}
	v_node = v_node->next; 
      }
    }
    n = n->next;
  }

  // read cell properties
  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "area");

    if (v_str==0) {
      unique_warning("Token \"area\" not found in xmlwrite.cpp at or around line no. 1018");
    }
    if (v_str != NULL) {
      area=standardlocale.toDouble((char *)v_str, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "target_area");

    if (v_str==0) {
      unique_warning("Token \"target_area\" not found in xmlwrite.cpp at or around line no. 1029");
    }
    if (v_str != NULL) {
      target_area=standardlocale.toDouble((char *)v_str, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)v_str);
      xmlFree(v_str);
    }
  }


  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "target_length");

    if (v_str==0) {
      unique_warning("Token \"target_length\" not found in xmlwrite.cpp at or around line no. 1041");
    }
    if (v_str != NULL) {
      target_length=standardlocale.toDouble((char *)v_str, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "lambda_celllength");

    if (v_str==0) {
      unique_warning("Token \"lambda_celllength\" not found in xmlwrite.cpp at or around line no. 1052");
    }
    if (v_str != NULL) {
      lambda_celllength=standardlocale.toDouble((char *)v_str, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "stiffness");

    if (v_str==0) {
      unique_warning("Token \"stiffness\" not found in xmlwrite.cpp at or around line no. 1063");
    }
    if (v_str != NULL) {
      stiffness=standardlocale.toDouble((char *)v_str, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)v_str);
      xmlFree(v_str);
    }
  }


  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "fixed");

    if (v_str==0) {
      unique_warning("Token \"fixed\" not found in xmlwrite.cpp at or around line no. 1075");
    }
    if (v_str != NULL) {
      fixed = strtobool( (char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "pin_fixed");

    if (v_str==0) {
      unique_warning("Token \"pin_fixed\" not found in xmlwrite.cpp at or around line no. 1086");
    }
    if (v_str != NULL) {
      pin_fixed = strtobool( (char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "at_boundary");

    if (v_str==0) {
      unique_warning("Token \"at_boundary\" not found in xmlwrite.cpp at or around line no. 1097");
    }
    if (v_str != NULL) {
      at_boundary = strtobool( (char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "dead");

    if (v_str==0) {
      unique_warning("Token \"dead\" not found in xmlwrite.cpp at or around line no. 1108");
    }
    if (v_str != NULL) {
      dead = strtobool( (char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "source");

    if (v_str==0) {
      unique_warning("Token \"source\" not found in xmlwrite.cpp at or around line no. 1119");
    }
    if (v_str != NULL) {
      source = strtobool( (char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "boundary");

    if (v_str==0) {
      unique_warning("Token \"boundary\" not found in xmlwrite.cpp at or around line no. 1130");
    }
    if (v_str != NULL) {
      for (int i=0;i<4;i++) {
	if (!xmlStrcmp( v_str, (const xmlChar *)Cell::boundary_type_names[i])) {
	  boundary=(Cell::boundary_type)i;
	  break;
	}
      }
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "div_counter");

    if (v_str==0) {
      unique_warning("Token \"div_counter\" not found in xmlwrite.cpp at or around line no. 1119");
    }
    if (v_str != NULL) {
      div_counter = atoi( (char *)v_str);
      xmlFree(v_str);
    }
  }

  {
    xmlChar *v_str = xmlGetProp(cur, BAD_CAST "cell_type");

    if (v_str==0) {
      unique_warning("Token \"cell_type\" not found in xmlwrite.cpp at or around line no. 1237");
    }
    if (v_str != NULL) {
      cell_type = atoi( (char *)v_str);
      xmlFree(v_str);
    }
  }

  // Recalculate moments and area
  SetIntegrals();
  return 0;
}

void NodeSet::XMLRead(xmlNode *root, Mesh *m)
{

  xmlNode *n = root->xmlChildrenNode;

  vector<int> tmp_nodes;
  while(n!=NULL) {
    if ((!xmlStrcmp(n->name, (const xmlChar *)"node"))) {
      xmlChar *nc = xmlGetProp(n, BAD_CAST "n");

      if (nc==0) {
	unique_warning("Token \"n\" not found in xmlwrite.cpp at or around line no. 966");
      }
      tmp_nodes.push_back(atoi( (char *)nc));
      xmlFree(nc);
    }
    n = n->next;
  }

  int nnodes = tmp_nodes.size();
  for (int i=0;i<nnodes;i++) {
    AddNode( &(m->getNode(tmp_nodes[i])) );
  }
}




void Wall::XMLAdd(xmlNode *parent) const { 

  // Save the node to a stream so we can reconstruct its state later
  xmlNodePtr xmlwall = xmlNewChild(parent, NULL, BAD_CAST "wall",NULL);

  {
    ostringstream text;
    text << Index();
    xmlNewProp(xmlwall, BAD_CAST "index" , BAD_CAST text.str().c_str() );
  }

  {
    ostringstream text;
    text << c1->Index();
    xmlNewProp(xmlwall, BAD_CAST "c1", BAD_CAST text.str().c_str());
  }


  {
    ostringstream text;
    text << c2->Index();
    xmlNewProp(xmlwall, BAD_CAST "c2", BAD_CAST text.str().c_str());
  }


  {
    ostringstream text;
    text << n1->Index();
    xmlNewProp(xmlwall, BAD_CAST "n1", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << n2->Index();
    xmlNewProp(xmlwall, BAD_CAST "n2", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << length;
    xmlNewProp(xmlwall, BAD_CAST "length", BAD_CAST text.str().c_str());
  }


  {
    ostringstream text;
    text << viz_flux;
    xmlNewProp(xmlwall, BAD_CAST "viz_flux", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << WallTypetoStr(wall_type).c_str();
    xmlNewProp(xmlwall, BAD_CAST "wall_type", BAD_CAST text.str().c_str());
  }


  xmlNodePtr tr1_xml = xmlNewChild(xmlwall, NULL, BAD_CAST "transporters1", NULL); 
  if (transporters1) {
    for (int i=0;i<Cell::NChem();i++) {
      xmlNodePtr tr1_val_xml = xmlNewChild(tr1_xml, NULL, BAD_CAST "val", NULL);
      { 
	ostringstream text;
	text << transporters1[i];
	xmlNewProp(tr1_val_xml, BAD_CAST "v", BAD_CAST text.str().c_str());

      }
    }
  }
  if (transporters2) {
    xmlNodePtr tr2_xml = xmlNewChild(xmlwall, NULL, BAD_CAST "transporters2", NULL); 

    for (int i=0;i<Cell::NChem();i++) {
      xmlNodePtr tr2_val_xml = xmlNewChild(tr2_xml, NULL, BAD_CAST "val", NULL);
      { 
	ostringstream text;
	text << transporters2[i];
	xmlNewProp(tr2_val_xml, BAD_CAST "v", BAD_CAST text.str().c_str());
      }
    }
  }

/*  if (apoplast) {
    xmlNodePtr apo_xml = xmlNewChild(xmlwall, NULL, BAD_CAST "apoplast", NULL); 

    for (int i=0;i<Cell::NChem();i++) {
      xmlNodePtr apo_val_xml = xmlNewChild(apo_xml, NULL, BAD_CAST "val", NULL);
      { 
	ostringstream text;
	text << transporters2[i];
	xmlNewProp(apo_val_xml, BAD_CAST "v", BAD_CAST text.str().c_str());
      }
    }
  } */
}


vector<double> XMLIO::XMLReadValArray(xmlNode *cur)
{

  vector<double> result;
  QLocale standardlocale(QLocale::C);
  bool ok;

  xmlNode *valarray_node = cur->xmlChildrenNode;
  while (valarray_node!=NULL) {
    if (!xmlStrcmp(valarray_node->name, (const xmlChar *)"val")) {
      xmlChar *vc = xmlGetProp(valarray_node, BAD_CAST "v");
      if (vc) {
	//result.push_back(strtod( (const char *)vc, 0));
	result.push_back(standardlocale.toDouble((char *)vc, &ok));
	if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)vc);
	xmlFree(vc);
      }

    }
    valarray_node = valarray_node->next;
  }
  return result;
}

void Mesh::XMLSave(const char *docname, xmlNode *options) const
{

  // based on libxml2 example code "tree2.c"

  xmlDocPtr doc = NULL;       /* document pointer */
  xmlNodePtr root_node = NULL;/* node pointers */
  //xmlDtdPtr dtd = NULL;       /* DTD pointer */

  //  LIBXML_TEST_VERSION;

  /* 
   * Creates a new document, a node and set it as a root node
   */
  doc = xmlNewDoc(BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "leaf");
  xmlDocSetRootElement(doc, root_node);

  /* 
   * xmlNewProp() creates attributes, which is "attached" to an node.
   * It returns xmlAttrPtr, which isn't used here.
   */
  xmlNewProp(root_node, BAD_CAST "name", BAD_CAST docname);

  time_t t;
  std::time(&t);

  char *tstring = strdup(asctime(localtime(&t))); // but this does
  // replace "end of line character by '\0'
  char *eol=strchr(tstring,'\n');
  if (eol!=NULL)
    *eol='\0';

  xmlNewProp(root_node, BAD_CAST "date", BAD_CAST tstring);
  free(tstring);


  QString simtime = QString("%1").arg(time);
  xmlNewProp(root_node, BAD_CAST "simtime", BAD_CAST simtime.toStdString().c_str()); 
  /*
   * Creates a DTD declaration. Isn't mandatory. 
   */

  par.XMLAdd(root_node);

  xmlNodePtr xmlnodes = xmlNewChild(root_node, NULL, BAD_CAST "nodes",NULL);
  { ostringstream text;
    text << NNodes();
    xmlNewProp(xmlnodes, BAD_CAST "n", BAD_CAST text.str().c_str());
  }

  { ostringstream text;
    text << Node::target_length;
    xmlNewProp(xmlnodes, BAD_CAST "target_length", BAD_CAST text.str().c_str());
  }

  for (vector<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
    (*i)->XMLAdd(xmlnodes) ;
  }


  /* 
   * xmlNewChild() creates a new node, which is "attached" as child node
   * of root_node node. 
   */
  xmlNodePtr xmlcells = xmlNewChild(root_node, NULL, BAD_CAST "cells",NULL);
  {
    ostringstream text;
    text << NCells();
    xmlNewProp(xmlcells, BAD_CAST "n", BAD_CAST text.str().c_str());
  }
  {
    ostringstream text;
    text << Cell::offset[0];
    xmlNewProp(xmlcells, BAD_CAST "offsetx", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << Cell::offset[1];
    xmlNewProp(xmlcells, BAD_CAST "offsety", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << Cell::factor;
    xmlNewProp(xmlcells, BAD_CAST "magnification", BAD_CAST text.str().c_str());
  }

  {
    ostringstream text;
    text << cells.front()->BaseArea();
    xmlNewProp(xmlcells, BAD_CAST "base_area", BAD_CAST text.str().c_str());
  }

  { 
    ostringstream text;
    text << Cell::NChem();
    xmlNewProp(xmlcells, BAD_CAST "nchem", BAD_CAST text.str().c_str());
  }

  for (vector<Cell *>::const_iterator i=cells.begin(); i!=cells.end(); i++) {
    (*i)->XMLAdd(xmlcells) ;
  }

  boundary_polygon->XMLAdd(xmlcells);

  xmlNodePtr xmlwalls = xmlNewChild(root_node, NULL, BAD_CAST "walls",NULL);
  { 
    ostringstream text;
    text << walls.size();
    xmlNewProp(xmlwalls, BAD_CAST "n", BAD_CAST text.str().c_str());
  }


  for (list<Wall *>::const_iterator i=walls.begin(); i!=walls.end(); i++) {
    (*i)->XMLAdd(xmlwalls) ;
  }


  xmlNodePtr xmlnodesets = xmlNewChild(root_node, NULL, BAD_CAST "nodesets",NULL);
  { 
    ostringstream text;
    text << node_sets.size();
    xmlNewProp(xmlnodesets, BAD_CAST "n", BAD_CAST text.str().c_str());
  }

  for_each( node_sets.begin(), node_sets.end(), bind2nd ( mem_fn( &NodeSet::XMLAdd ), xmlnodesets ) );

  // Add option tree for interactive application
  if (options) {
    xmlAddChild(root_node, options);
  }


  /* 
   * Dumping document to stdio or file
   */
  // Transparent compression seems not to work on Windows. So write uncompressed XML to ensure compatibility of LeafML files.
  xmlSetDocCompressMode(doc,0);
  xmlSaveFormatFileEnc(docname, doc, "UTF-8", 1);

  /*free the document */
  xmlFreeDoc(doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();

  /*
   * this is to debug memory for regression tests
   */
}



void Mesh::XMLReadSimtime(const xmlNode *a_node)
{

  xmlNode *root_node;
  root_node = (xmlNode *)a_node;
  xmlChar *strsimtime = xmlGetProp(root_node, BAD_CAST "simtime");


  if (strsimtime) {

    QLocale standardlocale(QLocale::C);
    bool ok;

    double simtime=standardlocale.toDouble((char *)strsimtime, &ok);
    if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(char *)strsimtime);
    time = simtime;
#ifdef QDEBUG
    qDebug() << "Simtime = " << strsimtime << endl;
#endif
  } else {
#ifdef QDEBUG
    qDebug() << "No simtime found in file." << endl;
#endif
    time =0;
  }	
}

void Mesh::XMLReadPars(const xmlNode * a_node)
{
  xmlNode *root_node;
  root_node = (xmlNode *)a_node;
  par.XMLRead(root_node);
  Seed(par.rseed);
  MakeDir(par.datadir);
}

void Mesh::XMLReadGeometry(const xmlNode * a_node)
{

  xmlNode *root_node;
  root_node = (xmlNode *)a_node;
  xmlNode *cur;
  QLocale standardlocale(QLocale::C);
  bool ok;
  // allocate Nodes
  cur = root_node->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"nodes"))){
      XMLReadNodes(cur);
    }
    cur=cur->next;
  }

  cur = root_node->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"nodesets"))) {
      XMLReadNodeSets(cur);
    }
    cur=cur->next;
  }

  cur=root_node->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"nodes"))) {
      XMLReadNodeSetsToNodes(cur);
    }
    cur = cur->next;
  }


  // allocate Cells
  cur = root_node;

  // allocate Cells
  cur = cur->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"cells"))){
      xmlChar *offsetxc = xmlGetProp(cur, BAD_CAST "offsetx");
      xmlChar *offsetyc = xmlGetProp(cur, BAD_CAST "offsety");

      double ox=standardlocale.toDouble((const char *)offsetxc, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)offsetxc);

      double oy=standardlocale.toDouble((const char *)offsetyc, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)offsetyc);
      Cell::setOffset(ox, oy);
      xmlFree(offsetxc);
      xmlFree(offsetyc);

      xmlChar *magnificationc = xmlGetProp(cur, BAD_CAST "magnification");

      Cell::SetMagnification(standardlocale.toDouble((const char *)magnificationc, &ok));
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)magnificationc);
      xmlFree(magnificationc);

      xmlChar *baseareac = xmlGetProp(cur, BAD_CAST "base_area");

      Cell::BaseArea() = standardlocale.toDouble((const char *)baseareac, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)baseareac);
      xmlFree(baseareac);

      XMLReadCells(cur);
    }
    cur=cur->next;
  }

  // allocate Walls (we need to have read the cells before constructing walls)
  vector <Wall *> tmp_walls;
  cur = root_node->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"walls"))){
      XMLReadWalls(cur, &tmp_walls);
    }
    cur=cur->next;
  }

  // read walls to cells and boundary_polygon
  cur = root_node->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"cells"))) {
      XMLReadWallsToCells(cur, &tmp_walls);
    }
    cur=cur->next;
  }

  boundary_polygon->ConstructNeighborList();
  boundary_polygon->ConstructConnections();

  for (vector<Cell *>::iterator c=cells.begin(); c!=cells.end(); c++) {
    (*c)->ConstructNeighborList();
    (*c)->ConstructConnections();
  }

  shuffled_cells.clear();
  shuffled_cells = cells;

  MyUrand r(shuffled_cells.size());
  vl_shuffle(shuffled_cells.begin(),shuffled_cells.end(),r);
}

void Mesh::XMLParseTree(const xmlNode *root_node)
{
  XMLReadSimtime(root_node);
  XMLReadPars(root_node);
  XMLReadGeometry(root_node);
}


void Mesh::XMLReadNodes(xmlNode *root)
{

  QLocale standardlocale(QLocale::C);
  bool ok;

  xmlNode *cur = root;
  cur = cur->xmlChildrenNode;

  for (vector<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {
    delete *i;
  }

  nodes.clear();
  Node::nnodes=0;

  xmlChar *tlc = xmlGetProp(root, BAD_CAST "target_length");

  if (tlc != 0) {

    Node::target_length = standardlocale.toDouble((const char *)tlc, &ok);
    if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)tlc);

    xmlFree(tlc);
  } else {
    // note that libxml2 also defines a token "warning"
    MyWarning::unique_warning("Warning: value found in XML file for Node::target_length.");
  }

  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"node"))){

      xmlChar *xc = xmlGetProp(cur, BAD_CAST "x");

      if (xc==0) {
	unique_warning("Token \"x\" not found in xmlwrite.cpp at or around line no. 722");
      }

      xmlChar *yc = xmlGetProp(cur, BAD_CAST "y");

      if (yc==0) {
	unique_warning("Token \"y\" not found in xmlwrite.cpp at or around line no. 727");
      }

      xmlChar *fixedc = xmlGetProp(cur, BAD_CAST "fixed");
      if (fixedc==0) {
	unique_warning("Token \"fixed\" not found in xmlwrite.cpp at or around line.");
      }

      xmlChar *boundaryc = xmlGetProp(cur, BAD_CAST "boundary");
      if (boundaryc==0) {
	unique_warning("Token \"boundary\" not found in xmlwrite.cpp at or around line.");
      }

      xmlChar *samc = xmlGetProp(cur, BAD_CAST "sam");
      if (samc==0) {
	unique_warning("Token \"sam\" not found in xmlwrite.cpp at or around line.");
      }


      double x = standardlocale.toDouble((const char *)xc, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)xc);

      double y = standardlocale.toDouble((const char *)yc, &ok);
      if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)yc);

      Node *new_node = new Node(x,y);
      nodes.push_back(new_node);

      new_node->m = this;
      new_node->fixed = strtobool( (char *)fixedc);
      new_node->boundary = strtobool( (char *)boundaryc );
      new_node->sam = strtobool ( (char *)samc);
      new_node->node_set = 0;

      xmlFree(xc);
      xmlFree(yc);
      xmlFree(boundaryc);
      xmlFree(fixedc);
      xmlFree(samc);
    }
    cur=cur->next;
  }

  shuffled_nodes.clear();
  shuffled_nodes = nodes;

  MyUrand r(shuffled_nodes.size());
  vl_shuffle(shuffled_nodes.begin(),shuffled_nodes.end(),r);
}

void Mesh::XMLReadWalls(xmlNode *root, vector<Wall *> *tmp_walls)
{

  xmlNode *cur = root;
  cur = cur->xmlChildrenNode;

  for (list<Wall *>::iterator i=walls.begin(); i!=walls.end(); i++) {
    delete *i;
  }

  walls.clear();
  Wall::nwalls = 0;
  tmp_walls->clear();

  QLocale standardlocale(QLocale::C);
  bool ok;

  while (cur!=NULL) {

    vector<int> tmp_nodes;
    while(cur!=NULL) {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"wall"))) {

	xmlChar *nc = xmlGetProp(cur, BAD_CAST "c1");

	if (nc==0) {
	  unique_warning("Token \"c1\" not found in xmlwrite.cpp at or around line no. 777");
	}
	int c1 = atoi( (char *)nc);
	xmlFree(nc);

	nc = xmlGetProp(cur, BAD_CAST "c2");

	if (	nc==0) {
	  unique_warning("Token \"c2\" not found in xmlwrite.cpp at or around line no. 785");
	}
	int c2 = atoi( (char *)nc);
	xmlFree(nc);

	nc = xmlGetProp(cur, BAD_CAST "n1");

	if (	nc==0) {
	  unique_warning("Token \"n1\" not found in xmlwrite.cpp at or around line no. 793");
	}
	int n1 = atoi( (char *)nc);
	xmlFree(nc);

	nc = xmlGetProp(cur, BAD_CAST "n2");

	if (	nc==0) {
	  unique_warning("Token \"n2\" not found in xmlwrite.cpp at or around line no. 801");
	}
	int n2 = atoi( (char *)nc);
	xmlFree(nc);

	nc = xmlGetProp(cur, BAD_CAST "length");

	if (	nc==0) {
	  unique_warning("Token \"length\" not found in xmlwrite.cpp at or around line no. 809");
	}

	double length = standardlocale.toDouble((const char *)nc, &ok);
	if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)nc);

	xmlFree(nc);


	nc = xmlGetProp(cur, BAD_CAST "viz_flux");

	double viz_flux = 0.0;
	if (nc!=0) {

	  viz_flux = standardlocale.toDouble((const char *)nc, &ok);
	  if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)nc);
	}
	xmlFree(nc);

	Wall::WallType wall_type;
	{
	  xmlChar *v_str = xmlGetProp(cur, BAD_CAST "wall_type");

	  if (v_str != 0) {

	    if (!xmlStrcmp(v_str, (const xmlChar *)"aux_source")) {
	      wall_type = Wall::AuxSource;
	    } else {
	      if (!xmlStrcmp(v_str, (const xmlChar *)"aux_sink")) {
		wall_type = Wall::AuxSink;
	      } else {
		wall_type = Wall::Normal;
	      }
	    }
	    xmlFree(v_str);

	  } else {
	    wall_type = Wall::Normal;
	  }
	}

	bool dead = false;
	{
	  // Note: property "delete" is used to manually clean up wall lists in XML files
	  // Simply add property 'delete="true"' to the wall and it will be removed from
	  // the mesh. (This saves us from manually reindexing the file). Otherwise do not use it.
	  xmlChar *v_str = xmlGetProp(cur, BAD_CAST "delete");

	  if (v_str != 0) {
	    dead = strtobool( (char *)v_str);
	    xmlFree(v_str);
	  }
	}

	Cell *cc1 = c1 != -1 ? cells[c1] : boundary_polygon;
	Cell *cc2 = c2 != -1 ? cells[c2] : boundary_polygon;

	Wall *w = new Wall( nodes[n1], nodes[n2], cc1, cc2);
	w->length = length;
	w->viz_flux = viz_flux;
	w->wall_type = wall_type;
	w->dead = dead;
	tmp_walls->push_back(w);
	walls.push_back(w);

	xmlNode *w_node = cur->xmlChildrenNode;
	while (w_node!=NULL) {
	  if ((!xmlStrcmp(w_node->name, (const xmlChar *)"transporters1"))) {

	    xmlNode *v_node = w_node->xmlChildrenNode;
	    int nv=0;
	    while (v_node!=NULL) {

	      if ((!xmlStrcmp(v_node->name, (const xmlChar *)"val"))) {
		if (nv>=Cell::NChem()) {
		  {
		    stringstream text;
		    text << "Exception in Mesh::XMLRead: Too many transporter values given for wall(s). Ignoring remaining values.";

		    unique_warning(text.str().c_str());
		    break;
		  }
		}
		xmlChar *nc = xmlGetProp(v_node, (const xmlChar *) "v");

		if (nc==0) {
		  unique_warning("Token \"v\" not found in xmlwrite.cpp at or around line no. 835");
		}

		double v = standardlocale.toDouble((const char *)nc, &ok);
		if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)nc);

		w->transporters1[nv++]=v;
		xmlFree(nc);

	      }
	      v_node = v_node->next; 
	    }
	  }

	  if ((!xmlStrcmp(w_node->name, (const xmlChar *)"transporters2"))) {

	    xmlNode *v_node = w_node->xmlChildrenNode;
	    int nv=0;
	    while (v_node!=NULL) {
	      if ((!xmlStrcmp(v_node->name, (const xmlChar *)"val"))) {
		if (nv>=Cell::NChem()) {
		  {
		    stringstream text;
		    text << "Exception in Mesh::XMLRead: Too many transporter values given for wall(s). Ignoring remaining values.";
		    unique_warning(text.str().c_str());
		    break;
		  }
		}

		xmlChar *nc = xmlGetProp(v_node, (const xmlChar *) "v");

		if (nc==0) {
		  unique_warning("Token \"v\" not found in xmlwrite.cpp at or around line no. 861");
		}

		double v = standardlocale.toDouble((const char *)nc, &ok);
		if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)nc);

		w->transporters2[nv++]=v;
		xmlFree(nc);
	      }
	      v_node = v_node->next; 
	    }
	  } 

	/*  if ((!xmlStrcmp(w_node->name, (const xmlChar *)"apoplast"))) {

	    xmlNode *v_node = w_node->xmlChildrenNode;
	    int nv=0;
	    while (v_node!=NULL) {

	      if ((!xmlStrcmp(v_node->name, (const xmlChar *)"val"))) {
		if (nv>=Cell::NChem()) {
		  {
		    stringstream text;
		    text << "Exception in Mesh::XMLRead: Too many transporter values given for wall(s). Ignoring remaining values.";

		    unique_warning(text.str().c_str());
		    break;
		  }
		}
		xmlChar *nc = xmlGetProp(v_node, (const xmlChar *) "v");

		if (nc==0) {
		  unique_warning("Token \"v\" not found in xmlwrite.cpp at or around line no. 887");
		}

		double v = standardlocale.toDouble((const char *)nc, &ok);
		if (!ok) MyWarning::error("Could not convert \"%s\" to double in XMLRead.",(const char *)nc);

		w->apoplast[nv++]=v;
		xmlFree(nc);
	      }
	      v_node = v_node->next; 
	    }
	  }*/
	  w_node=w_node->next;
	}
      }
      cur = cur->next;
    }
  }
}


void Mesh::XMLReadWallsToCells(xmlNode *root, vector<Wall *> *tmp_walls)
{

  // Add the walls to the cells (do this after reading the walls; read walls after reading cells...)
  // 1. Read Nodes
  // 2. Read Cells
  // 3. Read Walls
  // 4. Read Walls into Cells

  xmlNode *cur = root->xmlChildrenNode;
  int ci=0; // cell index

  while (cur!=NULL) {

    if ((!xmlStrcmp(cur->name, (const xmlChar *)"cell")) ||
	(!xmlStrcmp(cur->name, (const xmlChar *)"boundary_polygon" ))) {

      vector<int> tmp_walls_ind;
      xmlNode *n = cur->xmlChildrenNode;

      while(n!=NULL) {

	if ((!xmlStrcmp(n->name, (const xmlChar *)"wall"))) {
	  xmlChar *nc = xmlGetProp(n, BAD_CAST "w");

	  if (nc==0) {
	    unique_warning("Token \"w\" not found in xmlwrite.cpp at or around line no. 931");
	  }
	  tmp_walls_ind.push_back(atoi( (char *)nc));
	  xmlFree(nc);
	}
	n = n->next;
      }

      if (!xmlStrcmp(cur->name, (const xmlChar *)"boundary_polygon")) {

	int nwalls = tmp_walls_ind.size();
	for (int i=0;i<nwalls;i++) {
	  boundary_polygon->walls.push_back((*tmp_walls)[tmp_walls_ind[i]]);
	}
      } else {

	int nwalls = tmp_walls_ind.size();
	for (int i=0;i<nwalls;i++) {
	  cells[ci]->walls.push_back((*tmp_walls)[tmp_walls_ind[i]]);	
	}
	ci++;
      }
    } 
    cur=cur->next;
  }
}


void Mesh::XMLReadNodeSetsToNodes(xmlNode *root)
{

  xmlNode *cur = root->xmlChildrenNode;
  int ci=0; // cell index

  while (cur!=NULL) {



    if ((!xmlStrcmp(cur->name, (const xmlChar *)"node"))) {

      xmlNode *n = cur->xmlChildrenNode;

      while(n!=NULL) {

	xmlChar *nc = xmlGetProp(n, BAD_CAST "nodeset");

	if (nc!=0) {
	  int nodeset_n = atoi( (char *)nc);
	  nodes[ci]->node_set = node_sets[nodeset_n];
	  xmlFree(nc);
	} else {
	  nodes[ci]->node_set = 0;
	}

	n = n->next;
      }
      ci++;
    } 
    cur=cur->next;
  }
}


void Mesh::XMLReadNodeSets(xmlNode *root)
{

  for (vector<NodeSet *>::iterator i=node_sets.begin(); i!=node_sets.end(); i++) {
    delete *i;
  }

  node_sets.clear();

  xmlNode *cur = root->xmlChildrenNode;

  while (cur!=NULL) {

    NodeSet *new_nodeset=0;

    if ((!xmlStrcmp(cur->name, (const xmlChar *)"nodeset"))){
      new_nodeset = new NodeSet();
      node_sets.push_back(new_nodeset);
    } 

    if (new_nodeset == 0) { 
      cur = cur->next;
      continue;
    }
    new_nodeset->XMLRead(cur, this);
    cur=cur->next;
  }
}

void Mesh::XMLReadCells(xmlNode *root)
{
  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    delete *i;
  }

  cells.clear();
  Cell::NCells() = 0;

  if (boundary_polygon) {
    delete boundary_polygon;
    boundary_polygon=0;
  }

  xmlNode *cur = root->xmlChildrenNode;

  while (cur!=NULL) {

    Cell *new_cell=0;

    if ((!xmlStrcmp(cur->name, (const xmlChar *)"cell"))){

      new_cell = new Cell(0,0);
      new_cell->m = this;
      cells.push_back(new_cell);
    } else {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"boundary_polygon"))) {
	new_cell = boundary_polygon = new BoundaryPolygon(0,0);
	boundary_polygon->m = this;
      }
    }

    if (new_cell == 0) { 
      cur = cur->next;
      continue;
    }

    new_cell->XMLRead(cur);
    cur=cur->next;
  }
}

void Mesh::XMLRead(const char *docname, xmlNode **settings, bool geometry, bool pars, bool simtime)
{

  xmlDocPtr doc = xmlParseFile(docname);
  if (doc == NULL ) {
    throw("Document not parsed successfully.");
    return;
  }

  xmlNodePtr cur = xmlDocGetRootElement(doc);

  if (cur == NULL) {
    throw("Document is empty");
    xmlFreeDoc(doc);
    return;
  }

  if (xmlStrcmp(cur->name, (const xmlChar *) "leaf")) {
    throw("XML file of the wrong type, it is not a leaf.");
    xmlFreeDoc(doc);
    return;
  }

  /*Get the root element node */
  xmlNode *root_element = xmlDocGetRootElement(doc);

  if (geometry) XMLReadGeometry(root_element);
  if (pars) XMLReadPars(root_element);
  if (simtime) XMLReadSimtime(root_element);

  // If pointer settings defined, return a copy of the settings tree
  if (settings) {
    xmlNode *cur = root_element->xmlChildrenNode;
    // if settings field is not found, *settings will be returned 0.
    *settings = 0;
    while (cur!=NULL) {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"settings"))){
	*settings = xmlCopyNode(cur,1);
      }
      cur=cur->next;
    }
  }
  xmlFreeDoc(doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();

  // We're doing this so we can manually delete walls with by adding the 'delete="true"' property
  CleanUpCellNodeLists();
}


void Parameter::XMLRead(xmlNode *root)
{

  xmlNode *cur = root->xmlChildrenNode;
  while (cur!=NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"parameter"))){
      xmlNode *par_node = cur->xmlChildrenNode;
      while (par_node!=NULL) {
	{
	  if (!xmlStrcmp(par_node->name, (const xmlChar *)"par")) {
	    xmlChar *namec = xmlGetProp(par_node, BAD_CAST "name");
	    xmlChar *valc = xmlGetProp(par_node, BAD_CAST "val");
	    if (valc) {
	      AssignValToPar((const char*)namec,(const char*)valc);
	    } else {
	      /* Probably a valarray */
	      xmlNode *sub_par_node = par_node->xmlChildrenNode;
	      vector<double> valarray; 
	      while (sub_par_node != NULL) {
		if (!xmlStrcmp(sub_par_node->name, (const xmlChar *)"valarray")) {
		  valarray = XMLIO::XMLReadValArray(sub_par_node);
		}
		sub_par_node = sub_par_node->next;
	      }
	      AssignValArrayToPar((const char*)namec, valarray);
	    }
	  }
	}	  
	par_node = par_node->next;
      }
    }
    cur=cur->next;
  }
}

/* finis */
