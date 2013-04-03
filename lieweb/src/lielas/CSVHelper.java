package lielas;

import java.io.Serializable;

import java.io.InputStream;
import java.io.ByteArrayInputStream;

import com.example.lielas.LielasApplication;
import com.vaadin.terminal.StreamResource;

import lielas.SQLHelper;
import lielas.DeviceContainer;
import lielas.Device;
import lielas.Modul;
import lielas.Channel;
import lielas.Config;
import lielas.ExceptionHandler;

@SuppressWarnings({ "serial", "unused" })
public class CSVHelper implements Serializable{
	
	private LielasApplication app;
	
	private String csvFiletypeEnding = "";
	
	public CSVHelper(final LielasApplication app){
		this.app = app;
		
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
			}, filename + csvFiletypeEnding, app);
			
			return sr;
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return null;
	}
	
}
