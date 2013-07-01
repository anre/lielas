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
spackage lielas.core;

import java.io.Serializable;
import lielas.core.Channel;

@SuppressWarnings("serial")

public class Modul implements Serializable{

	private int address;
	private int meassurementIntervall;
	private int processIntervall;
	private int alarmIntervall;
	private Channel[] channel;
	private int id;

	private int channels;
	
	public Modul(int id, int mInt, int pInt, int aInt) {
		super();
		this.id = id;
		this.meassurementIntervall = mInt;
		this.processIntervall = pInt;
		this.alarmIntervall = aInt;
		this.channels = 0;
		
		channel =  new Channel[20];
	}

	public Integer getAddress() {
		return address;
	}

	public void setAddress(Integer address) {
		this.address = address;
	}

	public Integer getMeassurementIntervall() {
		return meassurementIntervall;
	}

	public void setMeassurementIntervall(Integer meassurementIntervall) {
		this.meassurementIntervall = meassurementIntervall;
	}

	public Integer getProcessIntervall() {
		return processIntervall;
	}

	public void setProcessIntervall(Integer processIntervall) {
		this.processIntervall = processIntervall;
	}

	public Integer getAlarmIntervall() {
		return alarmIntervall;
	}

	public void setAlarmIntervall(Integer alarmIntervall) {
		this.alarmIntervall = alarmIntervall;
	}

	public Integer getChannels() {
		return channels;
	}

	public void setChannels(Integer channels) {
		this.channels = channels;
	}
	
	public void addChannel(Channel c){
		channel[c.getAddress()] = new Channel(c.getID(), c.getAddress(), c.getType(), c.getUnit(), c.getMinValue(), c.getMaxValue());
		channel[c.getAddress()].setName(c.getName());
		channel[c.getAddress()].setGroup(c.getGroup());
		this.channels += 1;
	}
	
	public Channel getChannel(int address){
		return channel[address];
	}
	
	public int getID(){
		return this.id;
	}
	
	static public String getProcessIntervallString(int pInt){
		switch(pInt){
		case 1:
			return LanguageHelper.AT_PINT_OPT_LIFETIME;
		case 2:
			return LanguageHelper.AT_PINT_OPT_NORMAL;
		case 3:
			return LanguageHelper.AT_PINT_OPT_FAST_GATHER;
		}
		return null;
	}
	
	static public int getProcessIntervallInt(String pInt, LanguageHelper langHelper){
		if(pInt.equals(langHelper.GetString(LanguageHelper.AT_PINT_OPT_LIFETIME))){
			return 1;
		}else if(pInt.equals(langHelper.GetString(LanguageHelper.AT_PINT_OPT_NORMAL))){
			return 2;
		}else if(pInt.equals(langHelper.GetString(LanguageHelper.AT_PINT_OPT_FAST_GATHER))){
			return 3;
		}
		return 0;
	}
	
}
