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

import java.io.Serializable;

import java.io.InputStream;
import java.io.ByteArrayInputStream;

import com.vaadin.server.StreamResource;

import lielas.LiewebUI;

import lielas.core.SQLHelper;
import lielas.core.DeviceContainer;
import lielas.core.Device;
import lielas.core.Modul;
import lielas.core.Channel;
import lielas.core.Config;
import lielas.core.ExceptionHandler;

@SuppressWarnings({ "serial", "unused" })
public class CSVHelper implements Serializable{
	
	private LiewebUI app;
	
	private String csvFiletypeEnding = "";
	
	public CSVHelper(final LiewebUI app){
		
		Config cfg = new Config();
		cfg.LoadSettings();
		
		csvFiletypeEnding = cfg.getCsvFiletypeEnding();
		
		this.app = app;
	}
	
	public StreamResource GetCSVFile(String filename){
		try{
			
			final StreamResource sr = new StreamResource( new StreamResource.StreamSource() {				
				@Override
				public InputStream getStream() {
					if(app == null || app.deviceContainer == null || DeviceContainer.sql == null){
						return new ByteArrayInputStream("Error".getBytes());
					}
					StringBuilder csvStr = DeviceContainer.sql.GetDataTable(app.deviceContainer, app.user.getTimezone());
					if(csvStr == null){
						return new ByteArrayInputStream("Error".getBytes());
					}
					return new ByteArrayInputStream(csvStr.toString().getBytes());
				}
			}, filename + csvFiletypeEnding);
			
			return sr;
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return null;
	}
	
}
