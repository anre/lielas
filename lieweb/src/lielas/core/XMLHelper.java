/*
*
*	Copyright (c) 2013 Andreas Reder
*	Author      : Andreas Reder <andreas.reder@lielas.org>
*	File		: 
*
*	This File is part of lielas, see www.lielas.org for more information.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
package lielas.core;

import java.io.*;
import org.w3c.dom.*;

import com.sun.org.apache.xalan.internal.xsltc.trax.TransformerFactoryImpl;

import javax.xml.parsers.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;

@SuppressWarnings("serial")
public class XMLHelper implements Serializable{

	private DocumentBuilderFactory dbfac;
	private DocumentBuilder docBuilder;
	private Document doc;
	
	public XMLHelper(){
		
		try{
			
			// create document
			dbfac = DocumentBuilderFactory.newInstance();
			docBuilder = dbfac.newDocumentBuilder();
			doc = docBuilder.newDocument();	
			
			// fill with data
			Element root = doc.createElement("root");
			doc.appendChild(root);
			
			Element child = doc.createElement("PAGE_TITLE");
			child.setAttribute("id", "1");
			child.setAttribute("description", "Website Page Title");
			root.appendChild(child);
			
			Element innerChild = doc.createElement("english");
			Text text = doc.createTextNode("Devicemanager");
			innerChild.appendChild(text);
			child.appendChild(innerChild);
			
			innerChild = doc.createElement("german");
			text = doc.createTextNode("Gerätemanager");
			innerChild.appendChild(text);
			child.appendChild(innerChild);
			
			Element child2 = doc.createElement("TAB_DEVICE_MANAGER");
			child2.setAttribute("ID", "2");
			root.appendChild(child2);
			
			
			// create transformer
			TransformerFactory transFac = TransformerFactory.newInstance();
			transFac.setAttribute(TransformerFactoryImpl.INDENT_NUMBER, new Integer(4));
			Transformer trans = transFac.newTransformer();
			trans.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
			trans.setOutputProperty(OutputKeys.INDENT,	"yes");
			
			// create string
			StringWriter sw = new StringWriter();
			StreamResult result = new StreamResult(sw);
			DOMSource source = new DOMSource(doc);
			trans.transform(source, result);
			String xmlString = sw.toString();
			
			System.out.println("XML:\n\n" + xmlString);
			
			
			
		} catch (Exception e){
			e.printStackTrace();
		}
		
	}
	

}
