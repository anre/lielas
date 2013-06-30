package lielas.core;

import java.io.Serializable;

import lielas.core.CoapHelper;
import org.ws4d.coap.messages.CoapRequestCode;


public class LBus implements Serializable{

	private static final long serialVersionUID = 3747783061876507055L;
	
	public String LBUS_CMD_CHG = "chg";
	public String LBUS_CMD_DEL = "del";
	public String LBUS_CMD_LOGIN = "login";
	
	private String cmd = "";
	private Integer user = 0;
	private String address = "";
	private String payload= "";
	
	private String serverAddress = "";
	private Integer serverPort = 0;
	private String uriPath = "";

	public LBus(String serverAddress, Integer serverPort, String uriPath){
		this.serverAddress = serverAddress;
		this.serverPort = serverPort;
		this.uriPath = uriPath;
	}
	
	public LBus(String serverAddress, Integer serverPort, String uriPath, String cmd, Integer user, String address, String payload){
		this.serverAddress = serverAddress;
		this.serverPort = serverPort;
		this.uriPath = uriPath;
		this.cmd = cmd;
		this.user = user;
		this.address = address;
		this.payload = payload;
	}
	
	public void send(){
		CoapHelper ch = new CoapHelper(serverAddress, serverPort, uriPath, CoapRequestCode.PUT);
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