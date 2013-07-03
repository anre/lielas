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

import lielas.core.CoapHelper;
import org.ws4d.coap.messages.CoapRequestCode;

public class LBusSender extends LBus implements Serializable{

	private String cmd = "";
	private Integer user = 0;
	private String address = "";
	private String payload= "";
	
	private static final long serialVersionUID = -166363353055333020L;
	
	public LBusSender(String serverAddress, Integer serverPort, String uriPath){
		super(serverAddress, serverPort, uriPath);
	}
	
	public LBusSender(String serverAddress, Integer serverPort, String uriPath, String cmd, Integer user, String address, String payload){
		super(serverAddress, serverPort, uriPath);
		this.cmd = cmd;
		this.user = user;
		this.address = address;
		this.payload = payload;
	}
	
	public void send(){
		CoapHelper ch = new CoapHelper(this.serverAddress, serverPort, uriPath, CoapRequestCode.PUT);
		String lbusCmd;
		
		lbusCmd  = "{\n";
		lbusCmd += "\"cmd:\":\"" + this.cmd + "\",\n";
		lbusCmd += "\"user:\":\"" + this.user.toString() + "\",\n";
		lbusCmd += "\"address:\":\"" + this.address + "\",\n";
		if(!this.payload.equals("")){
			lbusCmd += "\"payload:\":{" + this.payload + "}\n";
		}
		lbusCmd += "}\n";
		
		ch.send(lbusCmd);
		
	}
	
	public String getCmd() {
		return cmd;
	}
	public void setCmd(String cmd) {
		this.cmd = cmd;
	}
	public Integer getUser() {
		return user;
	}
	public void setUser(Integer user) {
		this.user = user;
	}
	public String getAddress() {
		return address;
	}
	public void setAddress(String address) {
		this.address = address;
	}
	public String getPayload() {
		return payload;
	}
	public void setPayload(String payload) {
		this.payload = payload;
	}
}


