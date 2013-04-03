package com.example.lielas;

import com.example.lielas.ui.LoginScreen;
import com.example.lielas.ui.HeaderScreen;
import com.example.lielas.ui.DeviceManagerScreen;
import com.example.lielas.ui.OptionsScreen;
import com.example.lielas.ui.DownloadScreen;
import com.example.lielas.ui.AnalyseScreen;
import com.example.lielas.ui.HelpScreen;
import com.vaadin.Application;
import com.vaadin.terminal.*;
import com.vaadin.ui.*;
import com.vaadin.ui.Button.ClickEvent;
import lielas.Device;
import lielas.DeviceContainer;
import lielas.NewDeviceContainer;
import lielas.SQLHelper;
import lielas.User;
import lielas.UserContainer;
import lielas.XMLHelper;
import lielas.LanguageHelper;

@SuppressWarnings({ "serial", "unused" })

public class LielasApplication extends Application {
	
	/**
	 * 
	 */
	
	// Header
	public HeaderScreen headerScreen = null;
	private VerticalSplitPanel verticalSplit = new VerticalSplitPanel();
	
	public LoginScreen loginScreen = null;
	public DeviceManagerScreen deviceManagerScreen = null;
	public DownloadScreen downloadScreen = null;
	public AnalyseScreen analyseScreen = null;
	public OptionsScreen optionsScreen = null;
	public HelpScreen helpScreen = null;
	
	public DeviceContainer deviceContainer = null;
	public NewDeviceContainer newDeviceContainer = null;

	public LanguageHelper langHelper;
	
	public SQLHelper sql;
	
	public UserContainer userContainer = null;
	
	@Override
	public void init() {
		
		sql = new SQLHelper();
		sql.Connect();
		
		deviceContainer = DeviceContainer.loadDevices(sql);
		
		userContainer = UserContainer.loadUsers(sql);
		
		newDeviceContainer = NewDeviceContainer.createWithTestData();

		langHelper = new LanguageHelper("lang_en.properties");
		
		
		
		setTheme("reder");
		BuildMainLayout();
		
		loginScreen = new LoginScreen(this);
		setMainComponent(loginScreen);
		//setMainComponent(getDeviceManagerScreen());
		//headerScreen.setPermisson(1);
		
		
	}
	
	public void Authenticate(String username, String pw){
		boolean loggedIn = false;
		
		for(int i = 0; i < userContainer.size(); i++){
			User u = userContainer.getIdByIndex(i);
			if(u != null){
				if(u.getLogin().equals(username)){
					if(u.getPassword().equals(SQLHelper.getMD5Hash(pw))){
						setMainComponent(getDeviceManagerScreen());
						headerScreen.setPermisson(1);
						this.getMainWindow().showNotification("Welcome " + u.getForename() + " " + u.getName(), Window.Notification.TYPE_TRAY_NOTIFICATION);
						loggedIn = true;
					}
				}
			}
		}
		
		if(!loggedIn){
			this.getMainWindow().showNotification("Username or password not correct", Window.Notification.TYPE_TRAY_NOTIFICATION);
		}
		
	}
	
	public void Logout(){
		headerScreen.setPermisson(0);
		this.close();
	}
	
	public void setMainComponent(Component c){
		verticalSplit.setSecondComponent(c);
	}

	private void BuildMainLayout(){
		setMainWindow(new Window(langHelper.GetString("PROGRAM_TITLE")));

		getMainWindow().setContent(verticalSplit);
		getMainWindow().getContent().setSizeFull();
		
		verticalSplit.setLocked(true);
		verticalSplit.setMargin(false);
		verticalSplit.setSplitPosition(73, Sizeable.UNITS_PIXELS);
		verticalSplit.setFirstComponent(getHeaderScreen());
		verticalSplit.setSizeFull();
	}
	
	public void Update(){
		headerScreen.Update();
		if(deviceManagerScreen != null)
			deviceManagerScreen.Update();
		if(downloadScreen != null)
			downloadScreen.Update();
		if(optionsScreen != null){
			optionsScreen.Update();
		}if(loginScreen != null)
			loginScreen.Update();
	}
	
	public HeaderScreen getHeaderScreen(){
		if(headerScreen == null){
			headerScreen = new HeaderScreen(this);
		}
		return headerScreen;
	}
	
	public LoginScreen getLoginScreen(){
		if(loginScreen == null){
			loginScreen = new LoginScreen(this);
		}
		return loginScreen;
	}
	
	public DeviceManagerScreen getDeviceManagerScreen(){
		if(deviceManagerScreen == null){
			deviceManagerScreen = new DeviceManagerScreen(this);
		}
		return deviceManagerScreen;
	}
	
	public DownloadScreen getDownloadScreen(){
		if(downloadScreen == null){
			downloadScreen = new DownloadScreen(this);
		}
		return downloadScreen;
	}
	
	public AnalyseScreen getAnalyseScreen(){
		if( analyseScreen == null){
			analyseScreen = new AnalyseScreen(this);
		}
		return analyseScreen;
	}
	
	public OptionsScreen getOptionsScreen(){
		if(optionsScreen == null){
			optionsScreen = new OptionsScreen(this);
		}
		return optionsScreen;
	}
	
	public HelpScreen getHelpScreen(){
		if( helpScreen == null){
			helpScreen = new HelpScreen(this);
		}
		return helpScreen;
	}

}
