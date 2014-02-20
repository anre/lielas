/*
*
*	Copyright (c) 2014 Andreas Reder
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

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;

public class TcpClient{
	
	String serverAddress;
	Integer serverPort;
	
	public TcpClient(String serverAddress, Integer serverPort){
		this.serverAddress = serverAddress;
		this.serverPort = serverPort;
	}
	
	public boolean SendLoginMessage(Integer userId){
		String msg = "{\n";
		msg += "\"address\":\"liegw\",\n";
		msg += "\"user\":\"" + userId.toString() + "\",\n";
		msg += "\"cmd\":\"login\"\n";
		msg += "}";
		
		return send(msg);
	}
	
	public boolean SendDeleteDeviceMessage(Integer userId, Integer deviceId){
		String msg = "{\n";
		msg += "\"address\":\"liegw\",\n";
		msg += "\"user\":\"" + userId.toString() + "\",\n";
		msg += "\"cmd\":\"del\",\n";
		msg += "\"payload\":{\n";
		msg += "\t\"device\":\"" + deviceId.toString() + "\"\n";
		msg += "\t}";
		msg += "}";
		
		return send(msg);
	}
	
	public boolean SendChangeDateTimeMessage(Integer userId, String datetime){
		String msg = "{\n";
		msg += "\"address\":\"liegw\",\n";
		msg += "\"user\":\"" + userId.toString() + "\",\n";
		msg += "\"cmd\":\"chg\",\n";
		msg += "\"payload\":{\n";
		msg += "\t\"rtc\":\"" + datetime + "\"\n";
		msg += "\t}";
		msg += "}";

		return send(msg);
	}
	
	public boolean SendSaveDeviceSettingsMessage(Integer userId, Integer deviceId){
		String msg = "{\n";
		msg += "\"address\":\"liegw\",\n";
		msg += "\"user\":\"" + userId.toString() + "\",\n";
		msg += "\"cmd\":\"chg\",\n";
		msg += "\"payload\":{\n";
		msg += "\t\"device\":\"" + deviceId.toString() + "\"\n";
		msg += "\t}\n";
		msg += "}";
		
		return send(msg);
	}
	
	public boolean SendSaveNetworksettingsMessage(Integer userId, String netType){
		String msg = "{\n";
		msg += "\"address\":\"liegw\",\n";
		msg += "\"user\":\"" + userId.toString() + "\",\n";
		msg += "\"cmd\":\"chg\",\n";
		msg += "\"payload\":{\n";
		msg += "\t\"net_type\":\"" + netType + "\"\n";
		msg += "\t}";
		msg += "}";
		
		return send(msg);
	}
	
	public String getRtcState(Integer userId){
		String msg = "{\n";
		msg += "\"address\":\"liegw\",\n";
		msg += "\"user\":\"" + userId.toString() + "\",\n";
		msg += "\"cmd\":\"rtc\"\n";
		msg += "}";
		
		return sendAndReceive(msg);
	}
	
	private boolean send(String msg){
		try{
			java.net.Socket socket = new java.net.Socket(this.serverAddress, this.serverPort);
			BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			PrintWriter out = new PrintWriter(socket.getOutputStream());
			out.print(msg);
			out.flush();
			String answer = in.readLine();
			System.out.println("FROM Server: " + answer);
			if(answer.startsWith("error")){
				answer = in.readLine();
				System.out.println("Error: " + answer);
			}
			socket.close();
			if(answer.startsWith("ok")){
					return true;
			}
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		return false;
	}
	
	private String sendAndReceive(String msg){
		try{
			java.net.Socket socket = new java.net.Socket(this.serverAddress, this.serverPort);
			BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			PrintWriter out = new PrintWriter(socket.getOutputStream());
			out.print(msg);
			out.flush();
			String answer = in.readLine();
			if(!answer.startsWith("ok")){
				System.out.println("FROM Server: " + answer);
				answer = in.readLine();
				System.out.println("Error: " + answer);
				socket.close();
				return "";
			}
			answer = in.readLine();
			System.out.println("FROM Server: " + answer);
			socket.close();
			return answer;
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		return null;
	}
}