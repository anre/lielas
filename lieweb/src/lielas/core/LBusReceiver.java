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
import java.util.HashMap;

import org.stringtree.json.JSONReader;
import org.ws4d.coap.messages.CoapRequestCode;

public class LBusReceiver extends LBus implements Serializable{

	private String attribute;
	private Boolean error;
	
	private static final long serialVersionUID = -166363353055333021L;
	
	public LBusReceiver(String serverAddress, Integer serverPort, String uriPath){
		super(serverAddress, serverPort, uriPath);
	}
	
	public LBusReceiver(String serverAddress, Integer serverPort, String uriPath, String attribute){
		super(serverAddress, serverPort, uriPath);
		this.attribute = attribute;
		error = false;
	}
	
	public String get(){
		error = false;
		
		CoapHelper ch = new CoapHelper(this.serverAddress, serverPort, uriPath, CoapRequestCode.GET);
		ch.send("");
		while(ch.getRecvState() == CoapHelper.RECV_STATE_PENDING){
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				ExceptionHandler.HandleException(e);
			}
		}
		if(ch.getRecvState() == CoapHelper.RECV_STATE_OK){
			String res = ch.getPayload();
			//if an attribute is given, parse json and return resource value
			if(attribute != null){
				JSONReader reader = new JSONReader();
				Object obj = reader.read(res);
				@SuppressWarnings("unchecked")
				HashMap<String, String> json = (HashMap<String, String>)obj;
				String atr = json.get(attribute);
				if(atr != null)
					return atr;
				error = true;
				return "";
			}
			return res;
		}
		error = true;
		return "";
	}
	
	public Boolean getError(){
		return error;
	}

	public String getAttribute() {
		return attribute;
	}

	public void setAttribute(String attribute) {
		this.attribute = attribute;
	}
	
}
