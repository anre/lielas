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