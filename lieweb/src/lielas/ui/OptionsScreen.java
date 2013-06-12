package lielas.ui;


import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;

import lielas.core.Config;
import lielas.core.Device;
import lielas.core.DeviceContainer;
import lielas.core.ExceptionHandler;
import lielas.core.LanguageHelper;
import lielas.core.SQLHelper;
import lielas.core.CSVHelper;
import lielas.core.User;

import lielas.LiewebUI;
import lielas.ui.OptionsUserDetailsScreen;

//import com.github.wolfie.refresher.Refresher;
//import com.github.wolfie.refresher.Refresher.RefreshListener;
import com.google.gwt.user.client.ui.Widget;
import com.vaadin.data.Property;
import com.vaadin.data.Property.ValueChangeListener;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.server.Sizeable.Unit;
import com.vaadin.shared.ui.datefield.Resolution;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.CheckBox;
import com.vaadin.ui.DateField;
import com.vaadin.ui.FormLayout;
import com.vaadin.ui.GridLayout;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.Label;
import com.vaadin.ui.NativeSelect;
import com.vaadin.ui.Notification;
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
import com.vaadin.server.AbstractExtension;

@SuppressWarnings({ "serial", "unused" })
public class OptionsScreen extends Panel{

	private LiewebUI app;
	
	private TabSheet settingsTab = null;
	
//	Refresher refresher = null;
	
	private VerticalLayout globalSettingsLayout = null;
	private VerticalLayout userSettingsLayout = null;
	private VerticalLayout groupSettingsLayout = null;
	private VerticalLayout updateSettingsLayout = null;
	private VerticalLayout registerSettingsLayout = null;
	
	private VerticalLayout usersDetailLayout = null;
	private VerticalLayout groupsDetailLayout = null;
	
	private Table usersTable = null;
	private Table groupsTable = null;
	
	private Label clockSettingsLbl = null;
	private Label languageSettingsLbl = null;
	private Label databaseSettingsLbl = null;
	private Label networkSettingsLbl = null;
	private Label sixLowPanSettingsLbl = null;
	private Label gatewaySettingsLbl = null;
	
	private CheckBox useTimeServerCB = null;
	private TextField timeServerTxt = null;
	private DateField dateField = null;
	
	
	private Label useDhcpDLbl = null;
	private CheckBox useDhcpCB = null;
	private Label ipAddressDLbl = null;
	private TextField ipAddressCTx = null;
	private Label netmaskDLbl = null;
	private TextField netmaskCTx = null;
	private Label gwAddressDLbl = null;
	private TextField gwAddressCTx = null;

	private Label slpPanIdDLbl;
	private TextField slpPanIdCTx;
	
	private TextField gwPrefixCTx = null;
	private TextField gwIPCTx = null;
	private TextField gwUARTCTx = null;
	private TextField gwBaudrateCTx = null;
	private TextField gwTunnelCTx = null;
	
	private NativeButton newUserBttn = null;
	private NativeButton deleteDataBttn = null;
	private NativeButton deleteDatabaseBttn  = null;
	private NativeButton createTestDataBttn = null;
	
	Label regCaptionLbl;
	Label regMIntDLbl;
	TextField regMIntCTx;
	Label regPIntDLbl;
	NativeSelect regPIntCSel;
	Label regAIntDLbl;
	TextField regAIntCTx;
	NativeButton regSaveButton;
	Label regTimeDLbl;
	TextField regTimeCTx;
	
	final Integer RUNMODE_NORMAL = 1;
	final Integer RUNMODE_REG = 2;
	
	Integer runmode = RUNMODE_NORMAL;
	
	private HorizontalLayout userSetDetailsNoSelectionLayout = null;
	
	private OptionsUserDetailsScreen optionsUserDetailsScreen = null;
	
	public OptionsScreen(final LiewebUI app){
		this.app = app;
		setSizeFull();
		Activate();
		
		HorizontalLayout hLayout = new HorizontalLayout();
		hLayout.setSizeFull();
		
		//refresher
		/*refresher = new Refresher();
		refresher.setRefreshInterval(2000);
		refresher.addListener(new RefreshListener(){
			@Override
			public void refresh(final Refresher source){
				CheckRunmode();
			}
		});
		addExtension(refresher);*/
		
		/************************************************************************************************************************
		 * 
		 * 		settingsTab
		 * 
		 ************************************************************************************************************************/	
		
		settingsTab = new TabSheet();
		settingsTab.setHeight(100, Unit.PERCENTAGE);
		settingsTab.setWidth(1150, Unit.PIXELS);
		settingsTab.addStyleName("tab-settings");
		
		/************************************************************************************************************************
		 * 		globalSettingsLayout
		 ************************************************************************************************************************/	
		globalSettingsLayout = new VerticalLayout();
		//globalSettingsLayout.setSizeFull();

		//////////////////////////////////////////////////////////////////////////////////////////////
		// 								Clock Settings
		//////////////////////////////////////////////////////////////////////////////////////////////
		
		
		//create panel
		Panel clockSettingsPanel = new Panel();
		clockSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(clockSettingsPanel);
		
		//create vlayout
		VerticalLayout clockSettingsLayout = new VerticalLayout();
		clockSettingsLayout.addStyleName("settings-block");
		clockSettingsPanel.setContent(clockSettingsLayout);
		
		//header
		//clockSettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_CLOCK_SETTINGS));
		clockSettingsLbl = new Label("test");
		clockSettingsLbl.addStyleName("bold");
		clockSettingsLayout.addComponent(clockSettingsLbl);

		//content
		FormLayout timeServerLo = new FormLayout();
		clockSettingsLayout.addComponent(timeServerLo);
		
		timeServerTxt = new TextField("Time Server");
		timeServerTxt.setValue("0.at.pool.ntp.org");
		timeServerLo.addComponent(timeServerTxt);
		
		useTimeServerCB = new CheckBox("Automatically get Time");
		useTimeServerCB.setValue(true);
		useTimeServerCB.setImmediate(true);
		timeServerLo.addComponent(useTimeServerCB);
		
		useTimeServerCB.addValueChangeListener(new ValueChangeListener(){
			public void valueChange(ValueChangeEvent event){
				UseTimeServerCBClickHandler(event);
			}
		});
				
		dateField = new DateField();
		dateField.setValue(new Date());
		dateField.setResolution(Resolution.SECOND);
		dateField.setEnabled(false);
		timeServerLo.addComponent(dateField);
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// 						Language Settings 
		//////////////////////////////////////////////////////////////////////////////////////////////
		
		
		//create panel
		Panel languageSettingsPanel = new Panel();
		languageSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(languageSettingsPanel);
		
		//create vlayout
		VerticalLayout languageSettingsLayout = new VerticalLayout();
		languageSettingsLayout.addStyleName("settings-block");
		languageSettingsPanel.setContent(languageSettingsLayout);

		//header
		languageSettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_LANG_SETTINGS));
		languageSettingsLbl.addStyleName("bold");
		languageSettingsLayout.addComponent(languageSettingsLbl);
		
		//content
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// 							Database Settings
		//////////////////////////////////////////////////////////////////////////////////////////////
		

		//create panel
		Panel databaseSettingsPanel = new Panel();
		databaseSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(databaseSettingsPanel);

		//create vlayout
		VerticalLayout databaseSettingsLayout = new VerticalLayout();
		databaseSettingsLayout.addStyleName("settings-block");
		databaseSettingsPanel.setContent(databaseSettingsLayout);
		
		//header
		databaseSettingsLbl = new Label("Database Settings");
		databaseSettingsLbl.addStyleName("bold");
		databaseSettingsLbl.addStyleName("header");
		databaseSettingsLayout.addComponent(databaseSettingsLbl);

		//content
		deleteDataBttn = new NativeButton("Delete Data");
		deleteDataBttn.addStyleName("optionsscreen");
		deleteDataBttn.setHeight(24, Unit.PIXELS);
		databaseSettingsLayout.addComponent(deleteDataBttn);
		
		deleteDataBttn.addClickListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				DeleteDataBttnClicked(event);
			}			
		});
		
	/*	deleteDatabaseBttn = new NativeButton("Delete Database");
		deleteDatabaseBttn.addStyleName("optionsscreen");
		//deleteDatabaseBttn.setWidth(180, Unit.PIXELS);
		deleteDatabaseBttn.setHeight(24, Unit.PIXELS);
		databaseSettingsPanel.addComponent(deleteDatabaseBttn);
		
		deleteDatabaseBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				DeleteDatabaseBttnClicked(event);
			}			
		});*/
		
		
		/*createTestDataBttn = new NativeButton("Create Testdata");
		createTestDataBttn.addStyleName("optionsscreen");
		//deleteDatabaseBttn.setWidth(180, Unit.PIXELS);
		createTestDataBttn.setHeight(24, Unit.PIXELS);
		databaseSettingsPanel.addComponent(createTestDataBttn);
		
		createTestDataBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {		
				if(DeviceContainer.sql != null){
					DeviceContainer.sql.CreateTestData(app.deviceContainer, 100000);
				}
			}			
		});*/
		

		//////////////////////////////////////////////////////////////////////////////////////////////
		// 							Network settings
		//////////////////////////////////////////////////////////////////////////////////////////////

		//create panel
		Panel networkSettingsPanel = new Panel();
		networkSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(networkSettingsPanel);
		

		//create vlayout
		VerticalLayout networkSettingsLayout = new VerticalLayout();
		networkSettingsLayout.addStyleName("settings-block");
		networkSettingsPanel.setContent(networkSettingsLayout);
		
		//header
		networkSettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_NETWORK_SETTINGS));
		networkSettingsLbl.addStyleName("bold");
		networkSettingsLayout.addComponent(networkSettingsLbl);
		
		
		//content
		
		GridLayout networkSettingsGridLayout = new GridLayout(2, 10);
		networkSettingsLayout.addComponent(networkSettingsGridLayout);

		useDhcpDLbl = new Label("DHCP");
		useDhcpDLbl.addStyleName("settings");
		networkSettingsGridLayout.addComponent(useDhcpDLbl, 0, 0);
		useDhcpCB = new CheckBox();
		useDhcpCB.addStyleName("settings");
		networkSettingsGridLayout.addComponent(useDhcpCB, 1, 0);
		
		ipAddressDLbl = new Label("IP");
		ipAddressDLbl.addStyleName("settings");
		networkSettingsGridLayout.addComponent(ipAddressDLbl, 0, 1);
		ipAddressCTx = new TextField();
		ipAddressCTx.addStyleName("settings");
		networkSettingsGridLayout.addComponent(ipAddressCTx, 1, 1);
		
		netmaskDLbl = new Label("Netzmaske");
		netmaskDLbl.addStyleName("settings");
		networkSettingsGridLayout.addComponent(netmaskDLbl, 0, 2);
		netmaskCTx = new TextField();
		netmaskCTx.addStyleName("settings");
		networkSettingsGridLayout.addComponent(netmaskCTx, 1, 2);

		gwAddressDLbl = new Label("Gateway");
		gwAddressDLbl.addStyleName("settings");
		networkSettingsGridLayout.addComponent(gwAddressDLbl, 0, 3);
		gwAddressCTx = new TextField();
		gwAddressCTx.addStyleName("settings");
		networkSettingsGridLayout.addComponent(gwAddressCTx, 1, 3);

		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// 							6LowPan Settings
		//////////////////////////////////////////////////////////////////////////////////////////////
		
		//create panel
		Panel sixLowPanSettingsPanel = new Panel();
		sixLowPanSettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(sixLowPanSettingsPanel);
		

		//create vlayout
		VerticalLayout sixLowPanSettingsLayout = new VerticalLayout();
		sixLowPanSettingsLayout.addStyleName("settings-block");
		sixLowPanSettingsPanel.setContent(sixLowPanSettingsLayout);
		
		//header
		sixLowPanSettingsLbl = new Label("6LowPan Settings");
		sixLowPanSettingsLbl.addStyleName("bold");
		sixLowPanSettingsLayout.addComponent(sixLowPanSettingsLbl);
		
		GridLayout sixLowPanSettingsGridLayout = new GridLayout(2, 10);
		sixLowPanSettingsLayout.addComponent(sixLowPanSettingsGridLayout);	
		
		slpPanIdDLbl = new Label("Panid");
		slpPanIdDLbl.addStyleName("settings");
		sixLowPanSettingsGridLayout.addComponent(slpPanIdDLbl, 0, 3);
		slpPanIdCTx = new TextField();
		slpPanIdCTx.addStyleName("settings");
		sixLowPanSettingsGridLayout.addComponent(slpPanIdCTx, 1, 3);		
		
		
		
	
		// Gateway settings
		
		/*Panel gatewaySettingsPanel = new Panel();
		gatewaySettingsPanel.addStyleName("settings-block");
		globalSettingsLayout.addComponent(gatewaySettingsPanel);
		
		gatewaySettingsLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_6LOWPANGW_SETTINGS));
		gatewaySettingsLbl.addStyleName("bold");
		gatewaySettingsPanel.setContent(gatewaySettingsLbl);
		
		FormLayout gatewayFormLayout = new FormLayout();
		gatewaySettingsPanel.setContent(gatewayFormLayout);
		
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
		*/
		
		VerticalLayout gSpacerLayout = new VerticalLayout();
		gSpacerLayout.setSizeFull();
		globalSettingsLayout.addComponent(gSpacerLayout);
		
		
		/************************************************************************************************************************
		 * 		userSettingsLayout
		 ************************************************************************************************************************/
		userSettingsLayout = new VerticalLayout();
		//userSettingsLayout.setSizeFull();
		
		HorizontalLayout uhLo = new HorizontalLayout();
		//uhLo.setSizeFull();
		uhLo.addStyleName("userlist");
		
		usersTable = new Table(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_NAME));
		usersTable.setWidth(620, Unit.PIXELS);
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
		
		usersTable.addValueChangeListener(new Table.ValueChangeListener() {
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
		groupsTable.setWidth(620, Unit.PIXELS);
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
		groupsDetailLayout.setWidth(460, Unit.PIXELS);
		groupsDetailLayout.setStyleName("detaillist");
		groupsDetailLayout.setVisible(true);
		
		// Header 
		
		/*groupDetailHeaderLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_DETAILS_CAPTION));
		groupDetailHeaderLbl.setWidth(460, Unit.PIXELS);
		groupDetailHeaderLbl.addStyleName("detaillist-header");
		groupDetailHeaderLbl.setHeight(29, Unit.PIXELS);
		groupsDetailLayout.addComponent(groupDetailHeaderLbl);*/
		
		// Footer
		
		HorizontalLayout groupsDetailFooterLayout = new HorizontalLayout();
		groupsDetailFooterLayout.setWidth(460, Unit.PIXELS);
		groupsDetailFooterLayout.setHeight(40, Unit.PIXELS);
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
		
		
		/************************************************************************************************************************
		 * 		updateSettingsLayout
		 ************************************************************************************************************************/
		registerSettingsLayout = new VerticalLayout();
		registerSettingsLayout.setSizeFull();
		
		GridLayout regSettingsGridLayout = new GridLayout(2, 10);
		registerSettingsLayout.addComponent(regSettingsGridLayout);
		

		regCaptionLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_CAPTION));
		regCaptionLbl.addStyleName("reg-settings");
		regCaptionLbl.addStyleName("bold");
		regSettingsGridLayout.addComponent(regCaptionLbl, 0, 0);

		regMIntDLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_MINT_LBL));
		regMIntDLbl.addStyleName("reg-settings");
		regSettingsGridLayout.addComponent(regMIntDLbl, 0, 1);
		regMIntCTx = new TextField();
		regMIntCTx.addStyleName("reg-settings");
		regMIntCTx.setWidth(180, Unit.PIXELS);
		regSettingsGridLayout.addComponent(regMIntCTx, 1, 1);
		
		regPIntDLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_PINT_LBL));
		regPIntDLbl.addStyleName("reg-settings");
		regSettingsGridLayout.addComponent(regPIntDLbl, 0, 2);
		regPIntCSel = new NativeSelect();
		regPIntCSel.addStyleName("reg-settings");
		regPIntCSel.setWidth(180, Unit.PIXELS);
		regPIntCSel.setNullSelectionAllowed(false);
		regSettingsGridLayout.addComponent(regPIntCSel, 1, 2);
		
		regAIntDLbl = new Label(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_AINT_LBL));
		regAIntDLbl.addStyleName("reg-settings");
		regSettingsGridLayout.addComponent(regAIntDLbl, 0, 3);
		regAIntCTx = new TextField();
		regAIntCTx.addStyleName("reg-settings");
		regAIntCTx.setWidth(180, Unit.PIXELS);
		regSettingsGridLayout.addComponent(regAIntCTx, 1, 3);
		
		regTimeDLbl = new Label("Registration time");
		regTimeDLbl.addStyleName("reg-settings");
		regSettingsGridLayout.addComponent(regTimeDLbl, 0, 4);
		regTimeCTx = new TextField();
		regTimeCTx.addStyleName("reg-settings");
		regTimeCTx.setWidth(180, Unit.PIXELS);
		regSettingsGridLayout.addComponent(regTimeCTx, 1, 4);
		
		regSaveButton = new NativeButton("Start registration mode");
		regSaveButton.addStyleName("reg-settings");
		regSaveButton.addClickListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				StartRegModeBttnClicked(event);
			}	
		});
		regSettingsGridLayout.addComponent(regSaveButton, 1, 5);
		regSettingsGridLayout.setComponentAlignment(regSettingsGridLayout.getComponent(1, 5), Alignment.MIDDLE_RIGHT);
		

		
		/************************************************************************************************************************/
		 
		 
		settingsTab.addTab(globalSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL));
		settingsTab.addTab(userSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER));
		//settingsTab.addTab(groupSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP));
		settingsTab.addTab(updateSettingsLayout, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_UPDATE));
		settingsTab.addTab(registerSettingsLayout, "Registration");		
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
		settingsTab.getTab(registerSettingsLayout).setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG));
		
		// update Global settings
		clockSettingsLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_CLOCK_SETTINGS));
		languageSettingsLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GLOBAL_LANG_SETTINGS));
		
		/*gwPrefixCTx.setValue(cfg.getSixLowPanPrefix());
		gwIPCTx.setValue(cfg.getSixLowPanGatewayIP());
		gwUARTCTx.setValue(cfg.getSixLowPanUART());
		gwBaudrateCTx.setValue(cfg.getSixLowPanBaudrate());
		gwTunnelCTx.setValue(cfg.getSixLowPanTunnel());
		*/
		
		// user settings 
		usersTable.setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_NAME));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_USER));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_GROUP));
		usersTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE));
		
		// 6LowPan settings
		slpPanIdCTx.setValue(app.sql.getPanid());
		
		FillUsersTable();
		
		optionsUserDetailsScreen.Update();
		
		// group settings
		/*groupsTable.setCaption(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_NAME));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES));
		groupsTable.setColumnHeader(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD, app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD));
		
		groupDetailHeaderLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_GROUP_DETAILS_CAPTION));*/
		
		regCaptionLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_CAPTION));
		regMIntDLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_MINT_LBL));
		regPIntDLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_PINT_LBL));
		regAIntDLbl.setValue(app.langHelper.GetString(LanguageHelper.SET_TABSHEET_TAB_REG_AINT_LBL));
		
		regPIntCSel.removeAllItems();
		regPIntCSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_LIFETIME));
		regPIntCSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_NORMAL));
		regPIntCSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_FAST_GATHER));
		regPIntCSel.select(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_NORMAL));
		
		regMIntCTx.setValue(app.sql.getRegMInt().toString());
		regPIntCSel.select(app.langHelper.GetString(Device.getProcessIntervallString(app.sql.getRegPInt())));
		regAIntCTx.setValue(app.sql.getRegAInt().toString());
		regTimeCTx.setValue(app.sql.getRegTime().toString());
		
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
			usersTable.addItem(new Object[]{user.getLogin(), "", user.getTimezone().toString()}, user.getID());
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
			Notification.show("Database successfully deleted");
		}else{
			Notification.show("Error: Couldn't delete database", Notification.Type.ERROR_MESSAGE);
		}
	}
	
	private void DeleteDatabaseBttnClicked(ClickEvent event){
		
		/*if(DeviceContainer.sql != null){
			DeviceContainer.sql.CreateTestData(app.deviceContainer, 100000);
		}*/

		if(DeviceContainer.sql.DeleteDatabase()){
			Notification.show("Database successfully deleted");
		}else{
			Notification.show("Error: Couldn't delete database", Notification.Type.ERROR_MESSAGE);
		}
	}
	
	private void StartRegModeBttnClicked(ClickEvent event) {
		//app.coap.send("bla");
		//regCaptionLbl.setValue("bla");
		System.out.println("bla bla");
	}
	
	private void CheckRunmode(){
		Integer newRunmode = 1;
		// get runmode
		
		//check if reg mode
		if(newRunmode == RUNMODE_REG){
		
		}else if(newRunmode != runmode){
			if(newRunmode == RUNMODE_REG){
				//change to registration mode
				
			}else{
				//change back to normal mode
				
			}
		}		
	}
}

