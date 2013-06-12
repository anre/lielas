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
	private int processIntervall;
	private int alarmIntervall;
	private String supply;
	private int ID;
	private Modul[] modul;
	private int moduls;
	
		
	public Device(String address, String mac, boolean registered, String name, String group,
			Integer meassurementIntervall, Integer processIntervall,
			Integer alarmIntervall, String supply, Integer id) {
		super();
		this.address = address;
		this.mac = mac;
		this.registered = registered;
		this.name = name;
		this.group = group;
		this.meassurementIntervall = meassurementIntervall;
		this.processIntervall = processIntervall;
		this.alarmIntervall = alarmIntervall;
		this.supply = supply;
		this.ID = id;
		
		modul = new Modul[20];
		moduls = 0;
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
		modul[m.getAddress()] = new Modul(id, m.getMeassurementIntervall(), m.getProcessIntervall(), m.getAlarmIntervall());
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






