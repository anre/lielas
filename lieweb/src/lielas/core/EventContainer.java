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
public class EventContainer extends BeanItemContainer<Event> implements Serializable{
	
	public static SQLHelper sql = null;
	
	public EventContainer() throws InstantiationException, IllegalAccessException{
		super(Event.class);
	}
	
	public static EventContainer loadEvents(SQLHelper sqlHelper){
		EventContainer ec = null;
		
		sql = sqlHelper;
		
		try{
			ec = new EventContainer();
			sql.LoadEvents(ec);
			
		}catch(Exception e){
			e.printStackTrace();
		}
		
		return ec;
	}
	
	public boolean SaveDevice(Device d){
		return sql.SaveDevice(d);
	}
	

} 