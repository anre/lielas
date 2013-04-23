package lielas.core;

import java.io.Serializable;
import com.vaadin.data.util.BeanItemContainer;
import lielas.core.SQLHelper;

@SuppressWarnings("serial")
public class DeviceContainer extends BeanItemContainer<Device> implements Serializable{
	
	public static SQLHelper sql = null;
	
	public DeviceContainer() throws InstantiationException, IllegalAccessException{
		super(Device.class);
	}
	
	public static DeviceContainer loadDevices(SQLHelper sqlHelper){
		DeviceContainer dc = null;
		Device d;
		int id = 0;
		
		sql = sqlHelper;
		
		try{
			dc = new DeviceContainer();
			
			d = sql.GetDevice(id);
			while(d != null){
				dc.addItem(d);
				id = d.getID();
				d = sql.GetDevice(id);
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		
		return dc;
	}
	
	public boolean SaveDevice(Device d){
		return sql.SaveDevice(d);
	}
	

} 



