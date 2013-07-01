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
				id = d.getID() + 1;
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



