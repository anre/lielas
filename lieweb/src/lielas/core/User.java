package lielas.core;

import java.io.Serializable;

@SuppressWarnings("serial")
public class User implements Serializable{

	private int id;
	private String login = null;
	private String forename = null;
	private String name = null;
	private String usergroup = null;
	private String timezone = null;
	private String password = null;
	
	public User(int id){
		this.id = id;
	}
	
	public User(SQLHelper sql){
		this.id = sql.GetNewUserId();
	}
	
	public int getID() {
		return id;
	}
	public void setID(int id) {
		this.id = id;
	}
	public String getLogin() {
		return login;
	}
	public void setLogin(String login) {
		this.login = login;
	}
	public String getForename() {
		return forename;
	}
	public void setForename(String forename) {
		this.forename = forename;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getUsergroup() {
		return usergroup;
	}
	public void setUsergroup(String usergroup) {
		this.usergroup = usergroup;
	}
	public String getTimezone() {
		return timezone;
	}
	public void setTimezone(String timezone) {
		this.timezone = timezone;
	}
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
}
