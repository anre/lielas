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
	}
	
	public StreamResource GetCSVFile(String filename){
		try{
			
			final StreamResource sr = new StreamResource( new StreamResource.StreamSource() {				
				@Override
				public InputStream getStream() {
					if(app == null || app.deviceContainer == null || DeviceContainer.sql == null){
						return new ByteArrayInputStream("Error".getBytes());
					}
					StringBuilder csvStr = DeviceContainer.sql.GetDataTable(app.deviceContainer);
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
