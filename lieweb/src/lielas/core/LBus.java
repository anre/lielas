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




public class LBus implements Serializable{

	private static final long serialVersionUID = 3747783061876507055L;
	
	public String LBUS_CMD_CHG = "chg";
	public String LBUS_CMD_DEL = "del";
	public String LBUS_CMD_LOGIN = "login";
	

	protected String serverAddress = "";
	protected Integer serverPort = 0;
	protected String uriPath = "";

	public LBus(String serverAddress, Integer serverPort, String uriPath){
		this.serverAddress = serverAddress;
		this.serverPort = serverPort;
		this.uriPath = uriPath;
	}
	

}