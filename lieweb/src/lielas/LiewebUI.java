package lielas;

import lielas.ui.LoginScreen;
import lielas.ui.HeaderScreen;
import lielas.ui.DeviceManagerScreen;
import lielas.ui.OptionsScreen;
import lielas.ui.DownloadScreen;
import lielas.ui.AnalyseScreen;
import lielas.ui.HelpScreen;
import com.vaadin.ui.UI;
import com.vaadin.annotations.Theme;
import com.vaadin.server.VaadinRequest;
import com.vaadin.terminal.*;
import com.vaadin.ui.*;
import com.vaadin.ui.Button.ClickEvent;
import lielas.core.Device;
import lielas.core.DeviceContainer;
import lielas.core.NewDeviceContainer;
import lielas.core.SQLHelper;
import lielas.core.User;
import lielas.core.UserContainer;
import lielas.core.XMLHelper;
import lielas.core.LanguageHelper;

@SuppressWarnings({ "serial", "unused" })

@Theme("reder")
public class LiewebUI extends UI {
	
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
	protected void init(VaadinRequest request) {
		
		sql = new SQLHelper();
		sql.Connect();
		
		deviceContainer = DeviceContainer.loadDevices(sql);
		
		userContainer = UserContainer.loadUsers(sql);
		
		newDeviceContainer = NewDeviceContainer.createWithTestData();

		langHelper = new LanguageHelper("lang_en.properties");
		
		
		
		BuildMainLayout();
		
		//loginScreen = new LoginScreen(this);
		//setMainComponent(loginScreen);
		setMainComponent(getDeviceManagerScreen());
		headerScreen.setPermisson(1);
		
		
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
						Notification.show("Welcome " + u.getForename() + " " + u.getName(),Notification.Type.WARNING_MESSAGE);
						loggedIn = true;
					}
				}
			}
		}
		
		if(!loggedIn){
			Notification.show("Username or password not correct" ,Notification.Type.TRAY_NOTIFICATION);
		}
		
	}
	
	public void Logout(){
		headerScreen.setPermisson(0);
		setMainComponent(loginScreen);
	}
	
	public void setMainComponent(Component c){
		verticalSplit.setSecondComponent(c);
	}

	private void BuildMainLayout(){
		this.getPage().setTitle(langHelper.GetString("PROGRAM_TITLE"));
		
		setContent(verticalSplit);
		getContent().setSizeFull();
		
		verticalSplit.setLocked(true);
		verticalSplit.setSplitPosition(73, Unit.PIXELS);
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
