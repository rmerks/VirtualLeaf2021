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

#include "mainbase.h"
#include "xmlwrite.h"

#include <sstream>
#include <string>

#include <QLocale>
#include <QDir>
#include <QDebug>

static const std::string _module_id("$Id$");

QDomElement MainBase::XMLViewportTree(QTransform &transform) const {
  
  QLocale standardlocale(QLocale::C);
  

 // xmlNode *xmlviewport = xmlNewNode(NULL, BAD_CAST "viewport");
  QDomElement xmlviewport;
  xmlviewport.setTagName("xmlviewport");
  {
   // xmlNewProp(xmlviewport, BAD_CAST "m11", BAD_CAST standardlocale.toString(transform.m11()).toStdString().c_str() );
      xmlviewport.setAttribute("m11",standardlocale.toString(transform.m11()));
   // xmlNewProp(xmlviewport, BAD_CAST "m12", BAD_CAST standardlocale.toString(transform.m12()).toStdString().c_str() );
      xmlviewport.setAttribute("m12",standardlocale.toString(transform.m12()));
    //xmlNewProp(xmlviewport, BAD_CAST "m21", BAD_CAST standardlocale.toString(transform.m21()).toStdString().c_str() );
      xmlviewport.setAttribute("m21",standardlocale.toString(transform.m21()));
   // xmlNewProp(xmlviewport, BAD_CAST "m22", BAD_CAST standardlocale.toString(transform.m22()).toStdString().c_str() );
      xmlviewport.setAttribute("m22",standardlocale.toString(transform.m22()));
   // xmlNewProp(xmlviewport, BAD_CAST "dx", BAD_CAST standardlocale.toString(transform.dx()).toStdString().c_str() );
      xmlviewport.setAttribute("dx",standardlocale.toString(transform.dx()));
   // xmlNewProp(xmlviewport, BAD_CAST "dy", BAD_CAST standardlocale.toString(transform.dy()).toStdString().c_str() );
      xmlviewport.setAttribute("dy",standardlocale.toString(transform.dy()));
 }
  
  return xmlviewport;
}

QDomElement MainBase::XMLSettingsTree(void) {

  //xmlNode *xmlsettings = xmlNewNode(NULL, BAD_CAST "settings");
    QDomDocument doc("tmp-settings-doc");
    QDomElement xmlsettings = doc.createElement("settings");
    doc.appendChild(xmlsettings);
  {
    //xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
      QDomElement xmloption =  doc.createElement("setting");
      xmlsettings.appendChild(xmloption);
   // xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_centers");
      xmloption.setAttribute("name","show_cell_centers");

   // ostringstream text;
    //text << bool_name(showcentersp);
      xmloption.setAttribute("val",bool_name(showcentersp));
   // xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }
  {
    //xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
      QDomElement xmloption =  doc.createElement("setting");
      xmlsettings.appendChild(xmloption);
   // xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_centers");
      xmloption.setAttribute("name","show_middle_lamella");

   // ostringstream text;
    //text << bool_name(showcentersp);
      xmloption.setAttribute("val",bool_name(showmiddlelamella));
  }
  {
    //xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
      QDomElement xmloption =  doc.createElement("setting");
      xmlsettings.appendChild(xmloption);
   // xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_centers");
      xmloption.setAttribute("name","show_stiffness");

   // ostringstream text;
    //text << bool_name(showcentersp);
      xmloption.setAttribute("val",bool_name(showstiffness));
  }
/*  {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_nodes");
    ostringstream text;
    text << bool_name(showmeshp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption = doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_nodes");
        xmloption.setAttribute("val",bool_name(showmeshp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_node_numbers");
    ostringstream text;
    text << bool_name(shownodenumbersp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_node_numbers");
        xmloption.setAttribute("val",bool_name(shownodenumbersp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_numbers");
    ostringstream text;
    text << bool_name(showcellnumbersp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_cell_numbers");
        xmloption.setAttribute("val",bool_name(showcellnumbersp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_border_cells");
    ostringstream text;
    text << bool_name(showbordercellp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_borders_cells");
        xmloption.setAttribute("val",bool_name(showbordercellp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_axes");
    ostringstream text;
    text << bool_name(showcellsaxesp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_cell_axes");
        xmloption.setAttribute("val",bool_name(showcellsaxesp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_cell_strain");
    ostringstream text;
    text << bool_name(showcellstrainp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_cell_strain");
        xmloption.setAttribute("val",bool_name(showcellstrainp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_fluxes");
    ostringstream text;
    text << bool_name(showfluxesp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_fluxes");
        xmloption.setAttribute("val",bool_name(showfluxesp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_walls");
    ostringstream text;
    text << bool_name(showwallsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_walls");
        xmloption.setAttribute("val",bool_name(showwallsp));
    }
 //{
   // xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
   // xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_apoplasts");
   // ostringstream text;
   // text << bool_name(showapoplastsp);
  //  xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
//  }*/
  /*{
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "save_movie_frames");
    ostringstream text;
    text << bool_name(movieframesp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","save_movie_frames");
        xmloption.setAttribute("val",bool_name(movieframesp));
    }

  /*{
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "show_only_leaf_boundary");
    ostringstream text;
    text << bool_name(showboundaryonlyp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","show_only_leaf_boundary");
        xmloption.setAttribute("val",bool_name(showboundaryonlyp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "cell_growth");
    ostringstream text;
    text << bool_name(dynamicscellsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","cell_growth");
        xmloption.setAttribute("val",bool_name(dynamicscellsp));
    }
 /* {
    xmlNode *xmloption = xmlNewChild(xmlsettings, NULL, BAD_CAST "setting", NULL);
    xmlNewProp(xmloption, BAD_CAST "name", BAD_CAST "hide_cells");
    ostringstream text;
    text << bool_name(hidecellsp);
    xmlNewProp(xmloption, BAD_CAST "val", BAD_CAST text.str().c_str());
  }*/
    {
        QDomElement xmloption=doc.createElement("setting");
        xmlsettings.appendChild(xmloption);
        xmloption.setAttribute("name","hide_cells");
        xmloption.setAttribute("val",bool_name(hidecellsp));
    }
  return xmlsettings;
}

void MainBase::XMLReadViewport(QDomElement &settings) {

  if (settings == QDomElement()) {
    return;
  }

  qreal m11=25,m12=0,m21=0,m22=25,dx=0,dy=0;
  QLocale standardlocale(QLocale::C);
  QDomElement cur = settings.firstChild().toElement();

  while (!cur.isNull()) {
    
    if (cur.tagName()=="viewport") {
      bool ok;
      {
    QString v_str = cur.attribute("m11");
    if (v_str.isEmpty()) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
    } else {
      m11=standardlocale.toDouble(v_str, &ok);
      if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",v_str.toStdString().c_str());

	}
      }
      {
    QString v_str = cur.attribute("m12");
	
    if (v_str.isEmpty()) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
    } else {
      m12=standardlocale.toDouble(v_str, &ok);
      if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",v_str.toStdString().c_str());
	}
      }
      { 
    QString v_str = cur.attribute("m21");
	
    if (v_str.isEmpty()) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
    else {
      m21=standardlocale.toDouble(v_str, &ok);
      if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",v_str.toStdString().c_str());

	}
      }
      {
    QString v_str = cur.attribute("m22");
	
    if (v_str.isEmpty()) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
    else {
      m22=standardlocale.toDouble(v_str, &ok);
      if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",v_str.toStdString().c_str());

	}
      }
      {
    QString v_str = cur.attribute("dx");
	
    if (v_str.isEmpty()) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
    else {
      dx=standardlocale.toDouble(v_str, &ok);
      if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",v_str.toStdString().c_str());

	}
      }
      {
    QString v_str = cur.attribute("dy");
	
    if (v_str.isEmpty()) {
	  MyWarning::unique_warning("Error reading viewport in mainbase.cpp");
	}
    else {
      dy=standardlocale.toDouble(v_str, &ok);
      if (!ok) MyWarning::error("Could Not Convert \"%S\" To Double In XMLRead.",v_str.toStdString().c_str());

	}
      }
    }
    cur=cur.nextSibling().toElement();
  }
  viewport = QTransform(m11,m12,m21,m22,dx,dy);
}

void MainBase::XMLReadSettings(QDomElement &settings)
{

  // Many files have no settings section, so don't complain about it.
  // Defaults will be used instead.
  if (settings==QDomElement()) {
    return;
  }

  XMLReadViewport(settings);
  QDomElement cur = settings.firstChild().toElement();

  while (!cur.isNull()) {

    if (cur.tagName()=="setting") {

        QString name = cur.attribute("name");
        //xmlChar *name = xmlGetProp(cur, BAD_CAST "name");
        QString val = cur.attribute("val");

    //  xmlChar *val = xmlGetProp(cur, BAD_CAST "val");
        if (name=="show_cell_centers") {
    //  if (!xmlStrcmp(name, (const xmlChar *)"show_cell_centers")) {
    showcentersp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_nodes") {
    showmeshp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_node_numbers") {
    shownodenumbersp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_cell_numbers") {
    showcellnumbersp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_border_cells") {
    showbordercellp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_cell_axes") {
    showcellsaxesp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_cell_strain") {
    showcellstrainp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_fluxes") {
    showfluxesp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_walls") {
    showwallsp = strtobool(  val.toStdString().c_str() );
      }
     /* if (!xmlStrcmp(name, (const xmlChar *)"show_apoplasts")) {
    showapoplastsp = strtobool(  val.toStdString().c_str() );
      }*/
      if (name=="save_movie_frames") {
    movieframesp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_only_leaf_boundary") {
    showboundaryonlyp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="cell_growth") {
    dynamicscellsp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="hide_cells") {
    hidecellsp = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_stiffness") {
    showstiffness = strtobool(  val.toStdString().c_str() );
      }
      if (name=="show_middle_lamella") {
    showmiddlelamella = strtobool(  val.toStdString().c_str() );
      }

    }
    cur=cur.nextSibling().toElement();
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
  }
  return 0;
}

void MainBase::CutSAM()
{
  mesh.CutAwaySAM();
}

/* finis */
