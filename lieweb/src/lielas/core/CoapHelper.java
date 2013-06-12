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
	
	
	CoapChannelManager channelManager = null;
	CoapClientChannel clientChannel = null;

	public CoapHelper(String serverAdr, int serverPort, String uriPath, CoapRequestCode crc) {
		super();
		this.serverAdr = serverAdr;
		this.serverPort = serverPort;
		this.uriPath = uriPath;
		this.crc = crc;
		this.channelManager = BasicCoapChannelManager.getInstance();
	}

	public void send(String payload){
		try{
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
	
	@Override
	public void onResponse(CoapClientChannel channel, CoapResponse response) {
		// TODO Auto-generated method stub
		String payload = new String(response.getPayload());
		System.out.println("Coap: received response\nResponse Code: " + response.getResponseCode().toString() + "\nPayload: " + payload);
		
	}

	@Override
	public void onConnectionFailed(CoapClientChannel channel,
			boolean notReachable, boolean resetByServer) {
		// TODO Auto-generated method stub
		System.out.println("Coap: connection failed");
	}
	
}

















