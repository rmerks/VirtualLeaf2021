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

#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

#include "mainbase.h"
#include "xmlwrite.h"

#include <sstream>
#include <string>

#include <QLocale>
#include <QDir>
#include <QDebug>

static const std::string _module_id("$Id$");

xmlNode *MainBase::XMLViewportTree(QTransform &transform) const {
  
  QLocale standardlocale(QLocale::C);
  

  xmlNode *xmlviewport = xmlNewNode(NULL, BAD_CAST "viewport");
  {
    xmlNewProp(xmlviewport, BAD_CAST "m11", BAD_CAST standardlocale.toString(transform.m11()).toStdString().c_str() );
    xmlNewProp(xmlviewport, BAD_CAST "m12", BAD_CAST standardlocale.toString(transform.m12()).toStdString().c_str() );    
    xmlNewProp(xmlviewport, BAD_CAST "m21", BAD_CAST standardlocale.toString(transform.m21()).toStdString().c_str() );    
    xmlNewProp(xmlviewport, BAD_CAST "m22", BAD_CAST standardlocale.toString(transform.m22()).toStdString().c_str() );    
    xmlNewProp(xmlviewport, BAD_CAST "dx", BAD_CAST standardlocale.toString(transform.dx()).toStdString().c_str() );    
    xmlNewProp(xmlviewport, BAD_CAST "dy", BAD_CAST standardlocale.toString(transform.dy()).toStdString().c_str() );    
 }
  
  return xmlviewport;
}

xmlNode *MainBase::XMLSettingsTree(void) {

  xmlNode *xmlsettings = xmlNewNode(NULL, BAD_CAST "settings");
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_centers");
    ostringstream text;
    text << bool_name(showcentersp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_nodes");
    ostringstream text;
    text << bool_name(showmeshp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_node_numbers");
    ostringstream text;
    text << bool_name(shownodenumbersp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_numbers");
    ostringstream text;
    text << bool_name(showcellnumbersp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_border_cells");
    ostringstream text;
    text << bool_name(showbordercellp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_axes");
    ostringstream text;
    text << bool_name(showcellsaxesp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_strain");
    ostringstream text;
    text << bool_name(showcellstrainp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_fluxes");
    ostringstream text;
    text << bool_name(showfluxesp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_walls");
    ostringstream text;
    text << bool_name(showwallsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_apoplasts");
    ostringstream text;
    text << bool_name(showapoplastsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "save_movie_frames");
    ostringstream text;
    text << bool_name(movieframesp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_only_leaf_boundary");
    ostringstream text;
    text << bool_name(showboundaryonlyp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "cell_growth");
    ostringstream text;
    text << bool_name(dynamicscellsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "hide_cells");
    ostringstream text;
    text << bool_name(hidecellsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  return xmlsettings;
}

void MainBase::XMLReadViewport(xmlNode *settings) {

  if (settings == 0) {
    return;
  }

  qreal m11=25,m12=0,m21=0,m22=25,dx=0,dy=0;
  QLocale standardlocale(QLocale::C);
  xmlNode *cur = settings->xmlChildrenNode;

  while (cur!=NULL) {
    
    if (!xmlStrcmp(cur->name,(const xmlChar *)"viewport")) {
      bool ok;
      {
	xmlChar *v_str = xmlGetProp(cur, BAD_CAST "m11");
	
	if (v_str==0) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
	if (v_str != NULL) {
	  m11=standardlocale.toDouble((char *)v_str, &ok);
	  if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",(char *)v_str);
	  xmlFree(v_str);
	}
      }
      {
	xmlChar *v_str = xmlGetProp(cur, BAD_CAST "m12");
	
	if (v_str==0) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
	if (v_str != NULL) {
	  m12=standardlocale.toDouble((char *)v_str, &ok);
	  if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",(char *)v_str);
	  xmlFree(v_str);
	}
      }
      { 
	xmlChar *v_str = xmlGetProp(cur, BAD_CAST "m21");
	
	if (v_str==0) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
	if (v_str != NULL) {
	  m21=standardlocale.toDouble((char *)v_str, &ok);
	  if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",(char *)v_str);
	  xmlFree(v_str);
	}
      }
      {
	xmlChar *v_str = xmlGetProp(cur, BAD_CAST "m22");
	
	if (v_str==0) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
	if (v_str != NULL) {
	  m22=standardlocale.toDouble((char *)v_str, &ok);
	  if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",(char *)v_str);
	  xmlFree(v_str);
	}
      }
      {
	xmlChar *v_str = xmlGetProp(cur, BAD_CAST "dx");
	
	if (v_str==0) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
	if (v_str != NULL) {
	  dx=standardlocale.toDouble((char *)v_str, &ok);
	  if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",(char *)v_str);
	  xmlFree(v_str);
	}
      }
      {
	xmlChar *v_str = xmlGetProp(cur, BAD_CAST "dy");
	
	if (v_str==0) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
	if (v_str != NULL) {
	  dy=standardlocale.toDouble((char *)v_str, &ok);
	  if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",(char *)v_str);
	  xmlFree(v_str);
	}
      }
    }
    cur=cur->next;
  }
  viewport = QTransform(m11,m12,m21,m22,dx,dy);
}

void MainBase::XMLReadSettings(xmlNode *settings)
{

  // Many files have no settings section, so don't complain about it.
  // Defaults will be used instead.
  if (settings == 0) {
    return;
  }

  XMLReadViewport(settings);
  xmlNode *cur = settings->xmlChildrenNode;

  while (cur!=NULL) {

    if ((!xmlStrcmp(cur->name, (const xmlChar *)"setting"))){

      xmlChar *name = xmlGetProp(cur, BAD_CAST "name");
    
      xmlChar *val = xmlGetProp(cur, BAD_CAST "val");
      if (!xmlStrcmp(name, (const xmlChar *)"show_cell_centers")) {
	showcentersp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_nodes")) {
	showmeshp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_node_numbers")) {
	shownodenumbersp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_cell_numbers")) {
	showcellnumbersp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_border_cells")) {
	showbordercellp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_cell_axes")) {
	showcellsaxesp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_cell_strain")) {
	showcellstrainp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_fluxes")) {
	showfluxesp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_walls")) {
	showwallsp = strtobool( (const char *)val );
      }
     /* if (!xmlStrcmp(name, (const xmlChar *)"show_apoplasts")) {
	showapoplastsp = strtobool( (const char *)val );
      }*/
      if (!xmlStrcmp(name, (const xmlChar *)"save_movie_frames")) {
	movieframesp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"show_only_leaf_boundary")) {
	showboundaryonlyp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name, (const xmlChar *)"cell_growth")) {
	dynamicscellsp = strtobool( (const char *)val );
      }
      if (!xmlStrcmp(name,(const xmlChar *)"hide_cells")) {
	hidecellsp = strtobool( (const char *)val ); 
      }
    
	
  
      xmlFree(name);
      xmlFree(val);
    }
    cur=cur->next;
  }
}

int MainBase::Save(const char *fname, const char *format, int sizex, int sizey)
{

  Vector ll,ur;
  mesh.BoundingBox(ll, ur);

  if (QString(fname).isEmpty()) {
    MyWarning::warning("No output filename given. Saving nothing.\n");
    return 1;
  }

  
  ll*=Cell::Magnification(); ur*=Cell::Magnification();

  // give the leaf some space
  Vector border = ((ur-ll)/5.);
  
  if (!QString(format).contains("PDF", Qt::CaseInsensitive)) {

    QImage *image = new QImage(QSize(sizex, sizey), QImage::Format_RGB32);
    image->fill(QColor(Qt::white).rgb());
    QPainter *painter=new QPainter(image);
    canvas.render(painter);
#ifdef QDEBUG
    qDebug() << "Native Image Filename: " << QDir::toNativeSeparators(QString(fname)) << endl;
#endif
    if (!image->save(QDir::toNativeSeparators(QString(fname)))) { // please do not add "format" here! It is much better to have the system guess the file format from the extension. That prevents loads of cross-platform problems.
      MyWarning::warning("Image '%s' not saved successfully. Is the disk full or the extension not recognized?",fname);
      delete painter;
      delete image;
      return 1;
    } 
    delete painter;
    delete image;
  } else {
    QPrinter pdf(QPrinter::HighResolution);
    pdf.setOutputFileName(fname);
    pdf.setOutputFormat(QPrinter::PdfFormat);
    QPainter painter(&pdf);
    canvas.render(&painter, QRectF(), QRectF(-500,-500, 1000, 1000)); // NB Fiddle with these arguments if relative size of PDF images is important.
    
    cerr << "Rendering to printer\n";
  }
  return 0;
}

void MainBase::CutSAM()
{
  mesh.CutAwaySAM();
}

/* finis */
