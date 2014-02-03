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


@SuppressWarnings({"serial"})
public class Event implements Serializable {
	
	private Integer id;
	private String tmcreated;
	private String affects;
	private String cl; // class
	private String description;
	private Boolean handled;
	private String tmhandled;
	
	public Event(Integer id, String tmcreated, String affects, String cl, String description, Boolean handled){
		this.id = id;
		this.tmcreated = tmcreated;
		this.affects = affects;
		this.cl = cl;
		this.description = description;
		this.handled = handled;
	}

	public Integer getId() {
		return id;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public String getTmcreated() {
		return tmcreated;
	}

	public void setTmcreated(String tmcreated) {
		this.tmcreated = tmcreated;
	}

	public String getAffects() {
		return affects;
	}

	public void setAffects(String affects) {
		this.affects = affects;
	}

	public String getCl() {
		return cl;
	}

	public void setCl(String cl) {
		this.cl = cl;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public Boolean getHandled() {
		return handled;
	}

	public void setHandled(Boolean handled) {
		this.handled = handled;
	}

	public String getTmhandled() {
		return tmhandled;
	}

	public void setTmhandled(String tmhandled) {
		this.tmhandled = tmhandled;
	}

}






