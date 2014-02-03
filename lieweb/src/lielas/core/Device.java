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
import lielas.core.Channel;
import lielas.core.Modul;


@SuppressWarnings({"serial"})
public class Device implements Serializable {
	
	private String address;
	private String mac;
	private boolean registered;
	private String name;
	private String group;
	private int meassurementIntervall;
	private String supply;
	private int ID;
	private Modul[] modul;
	private int moduls;
	private Boolean hasEvent;
		
	public Device(String address, String mac, boolean registered, String name, String group,
			Integer meassurementIntervall, String supply, Integer id) {
		super();
		this.address = address;
		this.mac = mac;
		this.registered = registered;
		this.name = name;
		this.group = group;
		this.meassurementIntervall = meassurementIntervall;
		this.supply = supply;
		this.ID = id;
		
		modul = new Modul[20];
		moduls = 0;
		hasEvent = false;
	}

	public String getAddress() {
		return address;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	public String getMac() {
		return mac;
	}

	public void setMac(String mac) {
		this.mac = mac;
	}

	public boolean isRegistered() {
		return registered;
	}

	public void setRegistered(boolean registered) {
		this.registered = registered;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getGroup() {
		return group;
	}

	public void setGroup(String group) {
		this.group = group;
	}

	public Integer getMeassurementIntervall() {
		return meassurementIntervall;
	}

	public void setMeassurementIntervall(Integer meassurementIntervall) {
		this.meassurementIntervall = meassurementIntervall;
	}

	public String getSupply() {
		return supply;
	}

	public void setSupply(String supply) {
		this.supply = supply;
	}

	public Integer getID() {
		return this.ID;
	}

	public void setID(Integer id) {
		this.ID = id;
	}
	
	public int getModuls() {
		return moduls;
	}

	public void addModul(int id, Modul m){
		modul[m.getAddress()] = new Modul(id, m.getMeassurementIntervall());
		modul[m.getAddress()].setAddress(m.getAddress());
		moduls += 1;
	}
	
	public Modul getModul(int address){
		return modul[address];
	}
	
	public void addChannel(int modulAddress, Channel c){
		modul[modulAddress].addChannel(c);
	}
	
	public Channel getChannel(int modulAddress, int channelAddress){
		return modul[modulAddress].getChannel(channelAddress);
	}
	
	
	public int getChannels(int modulAddress){
		return modul[modulAddress].getChannels();
	}
	
	public boolean getHasEvent() {
		return hasEvent;
	}

	public void setHasEvent(boolean hasEvent) {
		this.hasEvent = hasEvent;
	}
}






