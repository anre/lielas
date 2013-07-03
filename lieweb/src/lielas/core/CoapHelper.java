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

import java.net.InetAddress;

import org.ws4d.coap.connection.BasicCoapChannelManager;
import org.ws4d.coap.interfaces.CoapChannelManager;
import org.ws4d.coap.interfaces.CoapClient;
import org.ws4d.coap.interfaces.CoapClientChannel;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.messages.CoapRequestCode;
import org.ws4d.coap.messages.CoapBlockOption.CoapBlockSize;

public class CoapHelper implements CoapClient {

	private String serverAdr;
	private Integer serverPort;
	private String uriPath;
	private CoapRequestCode crc;
	
	public static CoapRequestCode REQUEST_GET = CoapRequestCode.GET;
	public static CoapRequestCode REQUEST_PUT = CoapRequestCode.PUT;
	
	public Integer recvState;
	private String payload;
	
	public static int RECV_STATE_PENDING = 0;
	public static int RECV_STATE_OK = 1;
	public static int RECV_STATE_FAILURE = 2;
	
	CoapChannelManager channelManager = null;
	CoapClientChannel clientChannel = null;

	public CoapHelper(String serverAdr, int serverPort, String uriPath, CoapRequestCode crc) {
		this.serverAdr = serverAdr;
		this.serverPort = serverPort;
		this.uriPath = uriPath;
		this.crc = crc;
		this.channelManager = BasicCoapChannelManager.getInstance();
	}

	public void send(String payload){
		try{
			recvState = RECV_STATE_PENDING;
			payload = "";
			clientChannel = channelManager.connect(this, InetAddress.getByName(serverAdr), serverPort);
			CoapRequest coapRequest = clientChannel.createRequest(true,  crc);
			coapRequest.setUriPath("/" + uriPath);
			coapRequest.setPayload(payload);
			clientChannel.setMaxReceiveBlocksize(CoapBlockSize.BLOCK_64);
			clientChannel.sendMessage(coapRequest);
			System.out.println("Coap: sent request to " + serverAdr + ":" + serverPort.toString() + "/" + uriPath);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	public Integer getRecvState(){
		return recvState;
	}
	
	public String getPayload(){
		return payload;
	}
	
	@Override
	public void onResponse(CoapClientChannel channel, CoapResponse response) {
		payload = new String(response.getPayload());
		if(payload == null){
			payload = "";
		}
		//System.out.println("Coap: received response\nResponse Code: " + response.getResponseCode().toString() + "\nPayload: " + payload);
		recvState = RECV_STATE_OK;
		
	}

	@Override
	public void onConnectionFailed(CoapClientChannel channel,
			boolean notReachable, boolean resetByServer) {
		recvState = RECV_STATE_FAILURE;
	}
	
}

















