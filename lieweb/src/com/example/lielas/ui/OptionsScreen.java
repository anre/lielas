package com.example.lielas.ui;


import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;

import lielas.Config;
import lielas.Device;
import lielas.DeviceContainer;
import lielas.ExceptionHandler;
import lielas.LanguageHelper;
import lielas.SQLHelper;
import lielas.CSVHelper;
import lielas.User;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.OptionsUserDetailsScreen;
import com.vaadin.data.Property;
import com.vaadin.data.Property.ValueChangeListener;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.terminal.Sizeable;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.CheckBox;
import com.vaadin.ui.DateField;
import com.vaadin.ui.FormLayout;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.Label;
import com.vaadin.ui.Panel;
import com.vaadin.ui.TabSheet;
import com.vaadin.ui.Table;
import com.vaadin.ui.TextField;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.NativeButton;
import com.vaadin.ui.Window;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.terminal.StreamResource;

@SuppressWarnings({ "serial", "unused" })
public class OptionsScreen extends Panel{

	private LielasApplication app;
	
	private TabSheet settingsTab = null;
	
	private VerticalLayout globalSettingsLayout = null;
	private VerticalLayout userSettingsLayout = null;
	private VerticalLayout groupSettingsLayout = null;
	private VerticalLayout updateSettingsLayout = null;
	
	private VerticalLayout usersDetailLayout = null;
	private VerticalLayout groupsDetailLayout = null;
	
	private Table usersTable = null;
	private Table groupsTable = null;
	
	private Label clockSettingsLbl = null;
	private Label languageSettingsLbl = null;
	private Label databaseSettingsLbl = null;
	private Label networkSettingsLbl = null;
	private Label gatewaySettingsLbl = null;
	
	private CheckBox useTimeServerCB = null;
	private TextField timeServerTxt = null;
	private DateField dateField = null;
	
	
	
	private CheckBox useDhcpCB = null;
	private Label ipAddressDLbl = null;
	private TextField ipAddressCTx = null;
	private Label netmaskDLbl = null;
	private TextField netmaskCTx = null;
	private Label gwAddressDLbl = null;
	private TextField gwAddressCTx = null;

	private TextField gwPrefixCTx = null;
	private TextField gwIPCTx = null;
	private TextField gwUARTCTx = null;
	private TextField gwBaudrateCTx = null;
	private TextField gwTunnelCTx = null;
	
	private NativeButton newUserBttn = null;
	private NativeButton deleteDataBttn = null;
	private NativeButton deleteDatabaseBttn  = null;
	private NativeButton createTestDataBttn = null;
	
	private HorizontalLayout userSetDetailsNoSelectionLayout = null;
	
	private OptionsUserDetailsScreen optionsUserDetailsScreen = null;
	
	public OptionsScreen(final LielasApplication app){
		this.app = app;
		setSizeFull();
		Activate();
		
		HorizontalLayout hLayout = new HorizontalLayout();
		hLayout.setSizeFull();
		
		/************************************************************************************************************************
		 * 
		 * 		settingsTab
		 * 
		 ************************************************************************************************************************/	
		
		settingsTab = new TabSheet();
		settingsTab.setHeight(100, Sizeable.UNITS_PERCENTAGE);
		settingsTab.setWidth(1150, Sizeable.UNITS_PIXELS);
		settingsTab.addStyleName("tab-settings");
		
		/************************************************************************************************************************
		 * 		globalSettingsLayout
		 ************************************************************************************************************************/	
		globalSettingsLayout = new VerticalLayout();
		globalSettingsLayout.setSizeFull();
		
		// Clock Settings
		
		VerticalLayout clockSettingsPanel = new VerticalLayout();
		clockSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(clockSettingsPanel);
		
		clockSettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_CLOCK_SETTINGS));
		clockSettingsLbl.addStyleName("bold");
		clockSettingsPanel.addComponent(clockSettingsLbl);

		FormLayout timeServerLo = new FormLayout();
		clockSettingsPanel.addComponent(timeServerLo);
		
		timeServerTxt = new TextField("Time Server");
		timeServerTxt.setValue("0.at.pool.ntp.org");
		timeServerLo.addComponent(timeServerTxt);
		
		useTimeServerCB = new CheckBox("Automatically get Time");
		useTimeServerCB.setValue(true);
		useTimeServerCB.setImmediate(true);
		timeServerLo.addComponent(useTimeServerCB);
		
		useTimeServerCB.addListener(new ValueChangeListener(){
			public void valueChange(ValueChangeEvent event){
				UseTimeServerCBClickHandler(event);
			}
		});
				
		dateField = new DateField();
		dateField.setValue(new Date());
		dateField.setResolution(DateField.RESOLUTION_SEC);
		dateField.setEnabled(false);
		timeServerLo.addComponent(dateField);
		
		// Language Settings 
		
		Panel languageSettingsPanel = new Panel();
		languageSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(languageSettingsPanel);
		
		languageSettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_LANG_SETTINGS));
		languageSettingsLbl.addStyleName("bold");
		languageSettingsPanel.addComponent(languageSettingsLbl);
		
		// Database Settings
		
		Panel databaseSettingsPanel = new Panel();
		databaseSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(databaseSettingsPanel);
		
		databaseSettingsLbl = new Label("Database Settings");
		databaseSettingsLbl.addStyleName("bold");
		databaseSettingsPanel.addComponent(databaseSettingsLbl);
		
		deleteDataBttn = new NativeButton("Delete Data");
		deleteDataBttn.addStyleName("optionsscreen");
		deleteDataBttn.setHeight(24, Sizeable.UNITS_PIXELS);
		databaseSettingsPanel.addComponent(deleteDataBttn);
		
		deleteDataBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				DeleteDataBttnClicked(event);
			}			
		});
		
	/*	deleteDatabaseBttn = new NativeButton("Delete Database");
		deleteDatabaseBttn.addStyleName("optionsscreen");
		//deleteDatabaseBttn.setWidth(180, Sizeable.UNITS_PIXELS);
		deleteDatabaseBttn.setHeight(24, Sizeable.UNITS_PIXELS);
		databaseSettingsPanel.addComponent(deleteDatabaseBttn);
		
		deleteDatabaseBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				DeleteDatabaseBttnClicked(event);
			}			
		});*/
		
		
		/*createTestDataBttn = new NativeButton("Create Testdata");
		createTestDataBttn.addStyleName("optionsscreen");
		//deleteDatabaseBttn.setWidth(180, Sizeable.UNITS_PIXELS);
		createTestDataBttn.setHeight(24, Sizeable.UNITS_PIXELS);
		databaseSettingsPanel.addComponent(createTestDataBttn);
		
		createTestDataBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {		
				if(DeviceContainer.sql != null){
					DeviceContainer.sql.CreateTestData(app.deviceContainer, 100000);
				}
			}			
		});*/
		
		// Network settings
		
		Panel networkSettingsPanel = new Panel();
		networkSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(networkSettingsPanel);
		
		networkSettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_NETWORK_SETTINGS));
		networkSettingsLbl.addStyleName("bold");
		networkSettingsPanel.addComponent(networkSettingsLbl);
		
		useDhcpCB = new CheckBox("DHCP");
		networkSettingsPanel.addComponent(useDhcpCB);
		
		FormLayout networkFormLayout = new FormLayout();
		networkSettingsPanel.addComponent(networkFormLayout);
		
		ipAddressCTx = new TextField("IP");
		networkFormLayout.addComponent(ipAddressCTx);
		
		netmaskCTx = new TextField("Netzmaske");
		networkFormLayout.addComponent(netmaskCTx);

		gwAddressCTx = new TextField("Gateway");
		networkFormLayout.addComponent(gwAddressCTx);
		
		getIP();
	
		// Gateway settings
		Panel gatewaySettingsPanel = new Panel();
		gatewaySettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(gatewaySettingsPanel);
		
		gatewaySettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_6LOWPANGW_SETTINGS));
		gatewaySettingsLbl.addStyleName("bold");
		gatewaySettingsPanel.addComponent(gatewaySettingsLbl);
		
		FormLayout gatewayFormLayout = new FormLayout();
		gatewaySettingsPanel.addComponent(gatewayFormLayout);
		
		gwPrefixCTx = new TextField("Prefix");
		gatewayFormLayout.addComponent(gwPrefixCTx);
		
		gwIPCTx = new TextField("IP");
		gatewayFormLayout.addComponent(gwIPCTx);
		
		gwUARTCTx = new TextField("UART");
		gatewayFormLayout.addComponent(gwUARTCTx);
		
		gwBaudrateCTx = new TextField("Baudrate");
		gatewayFormLayout.addComponent(gwBaudrateCTx);
		
		gwTunnelCTx = new TextField("Tunnel");
		gatewayFormLayout.addComponent(gwTunnelCTx);
		
		VerticalLayout gSpacerLayout = new VerticalLayout();
		globalSettingsLayout.addComponent(gSpacerLayout);
		globalSettingsLayout.setExpandRatio(gSpacerLayout, 1.0f);
		
		/************************************************************************************************************************
		 * 		userSettingsLayout
		 ************************************************************************************************************************/
		userSettingsLayout = new VerticalLayout();
		userSettingsLayout.setSizeFull();
		
		HorizontalLayout uhLo = new HorizontalLayout();
		uhLo.setSizeFull();
		uhLo.addStyleName("userlist");
		
		usersTable = new Table(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_NAME));
		usersTable.setWidth(620, Sizeable.UNITS_PIXELS);
		usersTable.setSelectable(true);
		usersTable.setImmediate(true);
		usersTable.setPageLength(0);
		usersTable.setStyleName("userlist");
		
		usersTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER, String.class, null);
		usersTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP, String.class, null);
		usersTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE, String.class, null);
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE));
		
		FillUsersTable();
		
		usersTable.addListener(new Table.ValueChangeListener() {
			public void valueChange(ValueChangeEvent event) {
				UsersTableClickedHandler(event);
			}
		});

		uhLo.addComponent(usersTable);
		
		userSettingsLayout.addComponent(uhLo);
		
		optionsUserDetailsScreen = new OptionsUserDetailsScreen(this.app);
		optionsUserDetailsScreen.Update();
		
		uhLo.addComponent(optionsUserDetailsScreen);
		
		
		/************************************************************************************************************************
		 * 		groupSettingsLayout
		 ************************************************************************************************************************/
		groupSettingsLayout = new VerticalLayout();
		groupSettingsLayout.setSizeFull();
		
		HorizontalLayout ghLo = new HorizontalLayout();
		ghLo.setSizeFull();
		ghLo.addStyleName("userlist");
		
		
		groupsTable = new Table(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_NAME));
		groupsTable.setWidth(620, Sizeable.UNITS_PIXELS);
		groupsTable.setSelectable(true);
		groupsTable.setImmediate(true);
		groupsTable.setPageLength(0);
		groupsTable.setStyleName("grouplist");
		
		groupsTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP, String.class, null);
		groupsTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES, String.class, null);
		groupsTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES, String.class, null);
		groupsTable.addContainerProperty(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD, String.class, null);
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD));
		

		groupsTable.addItem(new Object[]{ "Admin", "allowed", "allowed",  "allowed"}, new Integer(1));
		groupsTable.addItem(new Object[]{ "User", "forbidden", "forbidden",  "allowed"}, new Integer(2));
		
		// Details
		
		groupsDetailLayout = new VerticalLayout();
		groupsDetailLayout.setWidth(460, Sizeable.UNITS_PIXELS);
		groupsDetailLayout.setStyleName("detaillist");
		groupsDetailLayout.setVisible(true);
		
		// Header 
		
		/*groupDetailHeaderLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_DETAILS_CAPTION));
		groupDetailHeaderLbl.setWidth(460, Sizeable.UNITS_PIXELS);
		groupDetailHeaderLbl.addStyleName("detaillist-header");
		groupDetailHeaderLbl.setHeight(29, Sizeable.UNITS_PIXELS);
		groupsDetailLayout.addComponent(groupDetailHeaderLbl);*/
		
		// Footer
		
		HorizontalLayout groupsDetailFooterLayout = new HorizontalLayout();
		groupsDetailFooterLayout.setWidth(460, Sizeable.UNITS_PIXELS);
		groupsDetailFooterLayout.setHeight(40, Sizeable.UNITS_PIXELS);
		groupsDetailFooterLayout.setStyleName("detaillist-footer");
		groupsDetailFooterLayout.setMargin(false);
		groupsDetailLayout.addComponent(groupsDetailFooterLayout);
		
		
		ghLo.addComponent(groupsTable);
		ghLo.addComponent(groupsDetailLayout);
		
		//groupSettingsLayout.addComponent(ghLo);


		/************************************************************************************************************************
		 * 		updateSettingsLayout
		 ************************************************************************************************************************/
		updateSettingsLayout = new VerticalLayout();
		updateSettingsLayout.setSizeFull();
		
		
		/************************************************************************************************************************/
		 
		 
		settingsTab.addTab(globalSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL));
		settingsTab.addTab(userSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER));
		//settingsTab.addTab(groupSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP));
		settingsTab.addTab(updateSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_UPDATE));
		
		hLayout.addComponent(settingsTab);
		hLayout.setComponentAlignment(settingsTab, Alignment.TOP_CENTER);
		setContent(hLayout);
		
		this.Update();
	}
	
	public void Activate(){
		app.headerScreen.setOptionsButtonActive();
	}
	
	public void Update(){
		Config cfg =  new Config();
		cfg.LoadSettings();
		
		settingsTab.getTab(globalSettingsLayout).setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL));
		settingsTab.getTab(userSettingsLayout).setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER));
		//settingsTab.getTab(groupSettingsLayout).setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP));
		settingsTab.getTab(updateSettingsLayout).setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_UPDATE));
		
		// update Global settings
		clockSettingsLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_CLOCK_SETTINGS));
		languageSettingsLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_LANG_SETTINGS));
		
		gwPrefixCTx.setValue(cfg.getSixLowPanPrefix());
		gwIPCTx.setValue(cfg.getSixLowPanGatewayIP());
		gwUARTCTx.setValue(cfg.getSixLowPanUART());
		gwBaudrateCTx.setValue(cfg.getSixLowPanBaudrate());
		gwTunnelCTx.setValue(cfg.getSixLowPanTunnel());
		
		// user settings 
		usersTable.setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_NAME));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE));
		
		FillUsersTable();
		
		optionsUserDetailsScreen.Update();
		
		// group settings
		/*groupsTable.setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_NAME));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD));
		
		groupDetailHeaderLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_DETAILS_CAPTION));*/
		
	}
	
	private String getIP(){
		String ip = "";
		
		try{
			//ProcessBuilder pb = new ProcessBuilder("sudo ifconfig eth0");
			
			
			
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return ip;		
	}
	
	private void FillUsersTable(){
		usersTable.removeAllItems();
		User user = app.userContainer.firstItemId();
		for(int i = 0; i < app.userContainer.size(); i++){
			usersTable.addItem(new Object[]{user.getLogin(), "", user.getTimezone()}, user.getID());
			user = app.userContainer.nextItemId(user);
		}
	}
	
	private void UseTimeServerCBClickHandler(ValueChangeEvent event){
		dateField.setEnabled(!((Boolean) event.getProperty().getValue()));
		
	}
	
	private void UsersTableClickedHandler(ValueChangeEvent event){
		User user = null;
		
		if( usersTable.getValue() == null){
			optionsUserDetailsScreen.setUser(null);
			optionsUserDetailsScreen.Update();
		}else{
			int id = Integer.parseInt(event.getProperty().getValue().toString());
			
			for( int i = 0; i< app.userContainer.size() && user == null; i++){
				user = app.userContainer.getIdByIndex(i);
				if(user.getID() != id){
					user = null;
				}
			}
			
			if(user != null){
				optionsUserDetailsScreen.setUser(user);
				optionsUserDetailsScreen.Update();
			}
		}
	}
	
	public void SelectUsersTableRow(int row){
		usersTable.select(row);
	}
	
	private void DeleteDataBttnClicked(ClickEvent event){
		if(DeviceContainer.sql.DeleteData()){
			app.getMainWindow().showNotification("Database successfully deleted");
		}else{
			app.getMainWindow().showNotification("Error: Couldn't delete database", Window.Notification.TYPE_ERROR_MESSAGE);
		}
	}
	
	private void DeleteDatabaseBttnClicked(ClickEvent event){
		
		/*if(DeviceContainer.sql != null){
			DeviceContainer.sql.CreateTestData(app.deviceContainer, 100000);
		}*/

		if(DeviceContainer.sql.DeleteDatabase()){
			app.getMainWindow().showNotification("Database successfully deleted");
		}else{
			app.getMainWindow().showNotification("Error: Couldn't delete database", Window.Notification.TYPE_ERROR_MESSAGE);
		}
	}
}

