package lielas;

import java.io.Serializable;


@SuppressWarnings("serial")
public class Channel implements Serializable{
	
	private int id;
	private int address;
	private String name;
	private String group;
	private String type;
	private String minValue;
	private String maxValue;
	private String unit;
	
	public Channel(int id, int address, String type, String unit, String minValue,
			String maxValue) {
		super();
		this.id = id;
		this.address = address;
		this.name = "";
		this.group = "";
		this.type = type;
		this.unit = unit;
		this.minValue = minValue;
		this.maxValue = maxValue;
	}

	public int getAddress() {
		return address;
	}

	public void setAddress(int address) {
		this.address = address;
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

	public String getType() {
		return type;
	}

	public void setType(String type) {
		this.type = type;
	}

	public String getMinValue() {
		return minValue;
	}

	public void setMinValue(String minValue) {
		this.minValue = minValue;
	}

	public String getMaxValue() {
		return maxValue;
	}

	public void setMaxValue(String maxValue) {
		this.maxValue = maxValue;
	}

	public String getUnit() {
		return unit;
	}

	public void setUnit(String unit) {
		this.unit = unit;
	}
	
	public int getID(){
		return this.id;
	}
}
