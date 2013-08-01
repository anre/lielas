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
import java.util.Hashtable;

public class UnitConverter implements Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1654987321L;
	
	public static String convert(String key){
		Hashtable<String, String> table = new Hashtable<String, String>();
		String val;
		
		table.put("oC", "°C");
		
		val = table.get(key);
		if(val == null)
			return key;
		return val;
	}
}