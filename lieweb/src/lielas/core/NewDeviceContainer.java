package lielas.core;

import java.io.Serializable;
import com.vaadin.data.util.BeanItemContainer;

@SuppressWarnings("serial")
public class NewDeviceContainer extends BeanItemContainer<Device> implements Serializable{
	public NewDeviceContainer() throws InstantiationException, IllegalAccessException{
		super(Device.class);
	}
	
	public static NewDeviceContainer createWithTestData(){
		NewDeviceContainer d = null;
		return d;
	}
} 