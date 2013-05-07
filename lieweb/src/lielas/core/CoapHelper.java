package lielas.core;

import java.net.InetAddress;
import java.net.UnknownHostException;

import org.ws4d.coap.Constants;
import org.ws4d.coap.connection.BasicCoapChannelManager;
import org.ws4d.coap.interfaces.CoapChannelManager;
import org.ws4d.coap.interfaces.CoapClient;
import org.ws4d.coap.interfaces.CoapClientChannel;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.messages.CoapEmptyMessage;
import org.ws4d.coap.messages.CoapRequestCode;
import org.ws4d.coap.messages.CoapBlockOption.CoapBlockSize;

public class CoapHelper implements CoapClient {

	private String serverAdr;
	private int serverPort;
	CoapChannelManager channelManager = null;
	CoapClientChannel clientChannel = null;

	public CoapHelper(String serverAdr, int serverPort) {
		super();
		this.serverAdr = serverAdr;
		this.serverPort = serverPort;
		this.channelManager = BasicCoapChannelManager.getInstance();
	}

	public void send(String payload){
		try{
			clientChannel = channelManager.connect(this, InetAddress.getByName(serverAdr), serverPort);
			CoapRequest coapRequest = clientChannel.createRequest(true,  CoapRequestCode.GET);
			coapRequest.setUriPath("/runmode");
			clientChannel.setMaxReceiveBlocksize(CoapBlockSize.BLOCK_64);
			clientChannel.sendMessage(coapRequest);
			System.out.println("Coap: sent request");
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	@Override
	public void onResponse(CoapClientChannel channel, CoapResponse response) {
		// TODO Auto-generated method stub
		System.out.println("Coap: received response\n");
		
	}

	@Override
	public void onConnectionFailed(CoapClientChannel channel,
			boolean notReachable, boolean resetByServer) {
		// TODO Auto-generated method stub
		System.out.println("Coap: connection failed");
	}
	
}