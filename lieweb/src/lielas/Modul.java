package lielas;

import java.io.Serializable;
import lielas.Channel;

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

	public int getAddress() {
		return address;
	}

	public void setAddress(int address) {
		this.address = address;
	}

	public int getMeassurementIntervall() {
		return meassurementIntervall;
	}

	public void setMeassurementIntervall(int meassurementIntervall) {
		this.meassurementIntervall = meassurementIntervall;
	}

	public int getProcessIntervall() {
		return processIntervall;
	}

	public void setProcessIntervall(int processIntervall) {
		this.processIntervall = processIntervall;
	}

	public int getAlarmIntervall() {
		return alarmIntervall;
	}

	public void setAlarmIntervall(int alarmIntervall) {
		this.alarmIntervall = alarmIntervall;
	}

	public int getChannels() {
		return channels;
	}

	public void setChannels(int channels) {
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
