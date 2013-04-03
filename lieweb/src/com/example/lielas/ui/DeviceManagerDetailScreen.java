package com.example.lielas.ui;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.HeaderScreen;
import com.example.lielas.ui.YesNoPopupScreen.PopupClosedListener;
import com.google.gwt.core.ext.linker.ModuleMetricsArtifact;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.Embedded;
import com.vaadin.ui.GridLayout;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.TextField;
import com.vaadin.ui.NativeButton;
import com.vaadin.ui.Panel;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.CheckBox;
import com.vaadin.ui.FormLayout;
import com.vaadin.ui.NativeSelect;
import com.vaadin.ui.Window;
import com.vaadin.ui.Window.Notification;
import com.vaadin.ui.Accordion;
import com.vaadin.ui.Table;
import com.vaadin.ui.Label;
import com.vaadin.ui.TabSheet;
import com.vaadin.ui.Form;
import com.vaadin.ui.NativeSelect;
import com.vaadin.data.Item;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.terminal.*;

import lielas.Device;
import lielas.DeviceContainer;
import lielas.Channel;
import lielas.ExceptionHandler;
import lielas.Modul;
import lielas.LanguageHelper;
import lielas.NewDeviceContainer;
import lielas.UserContainer;

@SuppressWarnings({ "serial", "unused" })
public class DeviceManagerDetailScreen extends VerticalLayout{

	private LielasApplication app;
	private Device device;
	
	private Label detailListHeaderLbl = null;
	
	private Label dlDAddressLbl = null;
	private Label dlCAddressLbl = null;
	private Label dlDNameLbl = null;
	private TextField dlCNameTx = null;
	private Label dlDGroupLbl = null;
	private TextField dlCGroupTx = null;
	private Label dlDMIntLbl = null;
	private TextField dlCMIntTx = null; 
	private Label dlDPIntLbl = null;
	private NativeSelect dlCPIntSel = null;
	private Label dlDAIntLbl = null;
	private TextField dlCAIntTx = null;
	private Label dlDSupplyLbl = null;
	private Label dlCSupplyLbl = null;

	private NativeButton dlSaveBttn = null;
	
	private static final int detailVSpacerHeight = 5;
	
	public static boolean TYPE_REGISTERED = true;
	public static boolean TYPE_NOT_REGISTERED = false;
	
	private Label modulAddressDLbl[];
	private Label modulMintDLbl[];
	private TextField modulMIntCTx[]; 
	private Label modulPintDLbl[];
	private NativeSelect modulPIntCTx[]; 
	private Label modulAintDLbl[];
	private TextField modulAIntCTx[]; 
	
	private int channelModulAddress[];
	private Label channelAddressDLbl[];
	private Label channelAddressCLbl[];
	private Label channelNameDLbl[];
	private TextField channelNameCTx[];
	private Label channelGroupDLbl[];
	private TextField channelGroupCTx[];
	private Label channelTypeDLbl[];
	private Label channelTypeCLbl[];
	private Label channelUnitDLbl[];
	private Label channelUnitCLbl[];
	private Label channelLastValueDLbl[];
	private Label channelLastValueCLbl[];
	
	private boolean registered;
	private int maxChannels = 20;
	
	public DeviceManagerDetailScreen(final LielasApplication app){
		this.app = app;
		this.device = null;
		this.setWidth(460, Sizeable.UNITS_PIXELS);
		this.addStyleName("detaillist");
		this.registered = false;
	}
	
	public DeviceManagerDetailScreen(final LielasApplication app, Device d, boolean registered){
		this.app = app;
		this.device = d;
		this.setWidth(460, Sizeable.UNITS_PIXELS);
		this.addStyleName("detaillist");
		this.registered = registered;
		Update();
	}
	
	public void setDevice( Device d, boolean registered){
		this.device = d;
		this.registered = registered;
	}
	
	public Device getDevice(){
		return this.device;
	}
	
	public void LoadContent(){
		
		int idCounter = 0;
		int cmAddress = 1;
		String value;
		
		dlCAddressLbl.setValue(device.getAddress());
		dlCNameTx.setValue(device.getName());
		dlCGroupTx.setValue(device.getGroup());
		dlCMIntTx.setValue(device.getMeassurementIntervall());
		dlCPIntSel.select(app.langHelper.GetString(Device.getProcessIntervallString(device.getProcessIntervall())));
		dlCAIntTx.setValue(device.getAlarmIntervall());		

		for(int i = 1; i < (device.getModuls()+1) ; i++){
			Modul m = device.getModul(i);
			modulMIntCTx[i].setValue(m.getMeassurementIntervall());
			modulPIntCTx[i].select(app.langHelper.GetString(Modul.getProcessIntervallString(m.getProcessIntervall())));
			modulPIntCTx[i].setValue(m.getProcessIntervall());
			modulAIntCTx[i].setValue(m.getAlarmIntervall());
			for( int j = 1; j <= m.getChannels();j++){
				Channel c = m.getChannel(j);
				channelNameCTx[cmAddress].setValue(c.getName());
				channelGroupCTx[cmAddress].setValue(c.getGroup());
				if(registered){
					value = app.sql.GetLastValue(device, m, c);
					if(value != ""){
						channelLastValueCLbl[cmAddress].setValue(value + c.getUnit());
					}
				}
				cmAddress += 1;
			}
		}
		
	}
	
	public void Update(){
		
		int cmAddress = 1;
		
		if( device == null){
			return;
		}
		
		this.removeAllComponents();
		
		this.setVisible(true);

		detailListHeaderLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_CAPTION));
		detailListHeaderLbl.setWidth(460, Sizeable.UNITS_PIXELS);
		detailListHeaderLbl.addStyleName("detaillist-header");
		detailListHeaderLbl.setHeight(29, Sizeable.UNITS_PIXELS);
		this.addComponent(detailListHeaderLbl);
		
		VerticalLayout detailListBodyLayout = new VerticalLayout();
		detailListBodyLayout.setWidth(438, Sizeable.UNITS_PIXELS);
		detailListBodyLayout.setStyleName("detaillist-body");		
		
		// Calculate number of lines needed
		int lines = 7; 	// 7 Lines for Device Info
		for(int i = 1; i < (device.getModuls()+1) ; i++){
			lines += 3;		// 1 Line Header, 1 Name, 1 Group
			for( int j = 1; j <= device.getChannels(i);j++){
				lines += 8;
				if(registered){
					lines += 2;
				}
			}
		}
		
		
		GridLayout dlBodyLayout = new GridLayout(3, lines);
		dlBodyLayout.setSizeFull();
		dlBodyLayout.setWidth(438, Sizeable.UNITS_PIXELS);
		dlBodyLayout.setColumnExpandRatio(0, 0);
		dlBodyLayout.setColumnExpandRatio(1, 0);
		dlBodyLayout.setColumnExpandRatio(2, 0);
		detailListBodyLayout.addComponent(dlBodyLayout);
		
		// show Address
		
		dlDAddressLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_ADDRESS));
		dlDAddressLbl.setWidth(109, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlDAddressLbl, 0, 0, 1, 0);
		dlCAddressLbl = new Label("");
		dlCAddressLbl.setWidth(215, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlCAddressLbl, 2, 0);
		
		// show Name
		dlDNameLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_NAME));
		dlBodyLayout.addComponent(dlDNameLbl, 0, 1, 1, 1);
		dlCNameTx = new TextField();
		dlCNameTx.setStyleName("detaillist-body");
		dlCNameTx.setWidth(215, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlCNameTx, 2, 1);
		
		// Group
		dlDGroupLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_GROUP));
		dlBodyLayout.addComponent(dlDGroupLbl, 0, 2, 1, 2);
		dlCGroupTx = new TextField();
		dlCGroupTx.setStyleName("detaillist-body");
		dlCGroupTx.setWidth(215, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlCGroupTx, 2, 2);
		
		// Meassurement Intervall
		dlDMIntLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_MEASSUREMENT_INTERVALL));
		dlBodyLayout.addComponent(dlDMIntLbl, 0, 3, 1, 3);
		dlCMIntTx = new TextField();
		dlCMIntTx.setStyleName("detaillist-body");
		dlCMIntTx.setWidth(215, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlCMIntTx, 2, 3);
		
		// Process Intervall
		dlDPIntLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_PROCESS_INTERVALL));
		dlBodyLayout.addComponent(dlDPIntLbl, 0, 4, 1, 4);
		dlCPIntSel = new NativeSelect();
		dlCPIntSel.setStyleName("detaillist-body");
		dlCPIntSel.setNullSelectionAllowed(false);
		dlCPIntSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_LIFETIME));
		dlCPIntSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_NORMAL));
		dlCPIntSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_FAST_GATHER));
		dlCPIntSel.setWidth(215, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlCPIntSel, 2, 4);
		
		// Alarm Intervall
		dlDAIntLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_ALARM_INTERVALL));
		dlBodyLayout.addComponent(dlDAIntLbl, 0, 5, 1, 5);
		dlCAIntTx = new TextField();
		dlCAIntTx.setStyleName("detaillist-body");
		dlCAIntTx.setWidth(215, Sizeable.UNITS_PIXELS);
		dlBodyLayout.addComponent(dlCAIntTx, 2, 5); 

		// Supply
		dlDSupplyLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_SUPPLY));
		dlBodyLayout.addComponent(dlDSupplyLbl, 0, 6, 1, 6);
		dlCSupplyLbl = new Label("Battery");
		dlBodyLayout.addComponent(dlCSupplyLbl, 2, 6);	  

		// Moduls
		modulAddressDLbl = new Label[20];
		modulMintDLbl = new Label[20];
		modulMIntCTx = new TextField[20];
		modulPintDLbl = new Label[20];
		modulPIntCTx = new NativeSelect[20];
		modulAintDLbl = new Label[20];
		modulAIntCTx = new TextField[20];
		
		channelModulAddress = new int[20];
		for(int i = 0; i < channelModulAddress.length; i++){
			channelModulAddress[i] = -1;
		}
		
		channelAddressDLbl = new Label[20];
		channelAddressCLbl = new Label[20];
		channelNameDLbl = new Label[20];
		channelNameCTx = new TextField[20];
		channelGroupDLbl = new Label[20];
		channelGroupCTx = new TextField[20];
		channelTypeDLbl = new Label[20];
		channelTypeCLbl = new Label[20];
		channelUnitDLbl = new Label[20];
		channelUnitCLbl = new Label[20];
		channelLastValueDLbl = new Label[20];
		channelLastValueCLbl = new Label[20];
		

		
		lines = 8;
		for(int i = 1; i < (device.getModuls()+1) ; i++){
			
			
			modulAddressDLbl[i] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_MODUL) + " " + Integer.toString(i));
			modulAddressDLbl[i].addStyleName("bold");
			modulAddressDLbl[i].addStyleName("newmodul");
			//modulAddressDLbl[i].setWidth(215, Sizeable.UNITS_PIXELS);
			dlBodyLayout.addComponent(modulAddressDLbl[i], 0, lines, 2, lines++);
			
			// MInt
			
			modulMintDLbl[i] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_MEASSUREMENT_INTERVALL));
			dlBodyLayout.addComponent(modulMintDLbl[i], 0, lines, 1, lines);
			
			modulMIntCTx[i] = new TextField();
			modulMIntCTx[i].setStyleName("detaillist-body");
			modulMIntCTx[i].setWidth(215, Sizeable.UNITS_PIXELS);
			dlBodyLayout.addComponent(modulMIntCTx[i], 2, lines++);
			
			// PInt

			modulPintDLbl[i] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_PROCESS_INTERVALL));
			dlBodyLayout.addComponent(modulPintDLbl[i], 0, lines, 1, lines);
			
			modulPIntCTx[i] = new NativeSelect();
			modulPIntCTx[i].setStyleName("detaillist-body");
			modulPIntCTx[i].setWidth(215, Sizeable.UNITS_PIXELS);
			modulPIntCTx[i].addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_LIFETIME));
			modulPIntCTx[i].addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_NORMAL));
			modulPIntCTx[i].addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_FAST_GATHER));
			dlBodyLayout.addComponent(modulPIntCTx[i], 2, lines++);
			
			// AInt

			modulAintDLbl[i] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_ALARM_INTERVALL));
			dlBodyLayout.addComponent(modulAintDLbl[i], 0, lines, 1, lines);
			
			modulAIntCTx[i] = new TextField();
			modulAIntCTx[i].setStyleName("detaillist-body");
			modulAIntCTx[i].setWidth(215, Sizeable.UNITS_PIXELS);
			dlBodyLayout.addComponent(modulAIntCTx[i], 2, lines++);

			VerticalLayout mSpacerLayout = new VerticalLayout();
			mSpacerLayout.setHeight(10, Sizeable.UNITS_PIXELS);
			dlBodyLayout.addComponent(mSpacerLayout, 0, lines++ );
			
			for( int j = 1; j <= device.getChannels(i);j++){
				channelModulAddress[cmAddress] = device.getModul(i).getAddress();
				
				Channel c = device.getChannel(i, j);
				
				// Address
				
				channelAddressDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_CHANNEL));
				channelAddressDLbl[cmAddress].addStyleName("bold");
				dlBodyLayout.addComponent(channelAddressDLbl[cmAddress], 1, lines );
				
				channelAddressCLbl[cmAddress] = new Label(Integer.toString(j));
				channelAddressCLbl[cmAddress].addStyleName("bold");
				dlBodyLayout.addComponent(channelAddressCLbl[cmAddress], 2, lines++);
				
				// Name
				
				channelNameDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_NAME));
				dlBodyLayout.addComponent(channelNameDLbl[cmAddress], 1, lines );
				
				channelNameCTx[cmAddress] = new TextField();
				channelNameCTx[cmAddress].setStyleName("detaillist-body");
				channelNameCTx[cmAddress].setWidth(215, Sizeable.UNITS_PIXELS);
				dlBodyLayout.addComponent(channelNameCTx[cmAddress], 2, lines++);
				
				// Group
				
				channelGroupDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_GROUP));
				dlBodyLayout.addComponent(channelGroupDLbl[cmAddress], 1, lines );
				
				channelGroupCTx[cmAddress] = new TextField();
				channelGroupCTx[cmAddress].setStyleName("detaillist-body");
				channelGroupCTx[cmAddress].setWidth(215, Sizeable.UNITS_PIXELS);
				dlBodyLayout.addComponent(channelGroupCTx[cmAddress], 2, lines++);
				
				// Type
				
				channelTypeDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_TYPE));
				dlBodyLayout.addComponent(channelTypeDLbl[cmAddress], 1, lines );
				
				channelTypeCLbl[cmAddress] = new Label(c.getType());
				dlBodyLayout.addComponent(channelTypeCLbl[cmAddress], 2, lines++ );
				
				// Unit
				
				channelUnitDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_UNIT));
				dlBodyLayout.addComponent(channelUnitDLbl[cmAddress], 1, lines );
				
				channelUnitCLbl[cmAddress] = new Label(c.getUnit());
				dlBodyLayout.addComponent(channelUnitCLbl[cmAddress], 2, lines++ );
				
				//Last value
				if(registered){
					channelLastValueDLbl[cmAddress] = new Label("Last Value");
					dlBodyLayout.addComponent(channelLastValueDLbl[cmAddress], 1, lines );

					channelLastValueCLbl[cmAddress] = new Label(" ", Label.CONTENT_PREFORMATTED);
					dlBodyLayout.addComponent(channelLastValueCLbl[cmAddress], 2, lines++ );
				}
				// Spacer
				VerticalLayout channelSpacerLayout = new VerticalLayout();
				channelSpacerLayout.setHeight(10, Sizeable.UNITS_PIXELS);
				dlBodyLayout.addComponent(channelSpacerLayout, 0, lines++ );
				
				cmAddress += 1;
			}
			
		}
		
		// Footer
		
		HorizontalLayout detailListFooterLayout = new HorizontalLayout();
		detailListFooterLayout.setWidth(460, Sizeable.UNITS_PIXELS);
		detailListFooterLayout.setHeight(40, Sizeable.UNITS_PIXELS);
		detailListFooterLayout.setStyleName("detaillist-footer");
		detailListFooterLayout.setMargin(false);
		
		// Save Button

		Label dlFooterVSpacer = new Label();
		dlFooterVSpacer.setWidth(245, Sizeable.UNITS_PIXELS);
		detailListFooterLayout.addComponent(dlFooterVSpacer);
		detailListFooterLayout.setComponentAlignment(dlFooterVSpacer, Alignment.TOP_RIGHT);
		
		dlSaveBttn = new NativeButton(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_SAVE_BTTN));
		dlSaveBttn.addStyleName("detaillist-footer");
		detailListFooterLayout.addComponent(dlSaveBttn);
		detailListFooterLayout.setComponentAlignment(dlSaveBttn, Alignment.TOP_CENTER);		
		
		dlSaveBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				// TODO Auto-generated method stub
				DlSaveBttnClicked(event);
			}
		});
		
		this.addComponent(detailListBodyLayout);
		this.addComponent(detailListFooterLayout);
		
		
		
		
		LoadContent();
	}
	
	int ParseInterval(String str, String type){
		int mInt = -1;
		
		if(str.matches("\\d+")){
			try{
				mInt = Integer.parseInt(str);
				if(mInt < 10){
					app.getMainWindow().showNotification(type + "Inteterval is to small, minimum is 10s", Window.Notification.TYPE_WARNING_MESSAGE);	
					return -1;
				}else if(mInt > 43200){
					app.getMainWindow().showNotification(type + "Inteterval is to big, maximum is 43200s", Window.Notification.TYPE_WARNING_MESSAGE);
				}
				return mInt;
			}catch(Exception e){
				app.getMainWindow().showNotification(type + "Inteterval is no valid number", Window.Notification.TYPE_WARNING_MESSAGE);
				ExceptionHandler.HandleException(e);
			}
		}
		
		return -1;
	}

	private void DlSaveBttnClicked(ClickEvent event){
		
		boolean error = false;
		int mInt, aInt;
		int[] modulMInt = new int[20];
		int[] modulPInt = new int[20];
		int[] modulAInt = new int[20];
		int channelNr = 1;
		
		mInt = ParseInterval(dlCMIntTx.getValue().toString(), "Meassurement ");
		if(mInt == -1){
			error = true;
		}
		
		aInt =  ParseInterval(dlCAIntTx.getValue().toString(), "Alarm ");
		if(aInt == -1){
			error = true;
		}
		
		for(int i = 1; i < (device.getModuls()+1); i++){
			Modul m = device.getModul(i);
			modulMInt[i] = ParseInterval(modulMIntCTx[i].getValue().toString(), "Module " + m.getAddress() + " Meassurement ");
			if(modulMInt[i] == -1){
				error = true;
			}

			modulPInt[i] = Modul.getProcessIntervallInt(modulPIntCTx[i].getValue().toString(), app.langHelper);
			
			modulAInt[i] = ParseInterval(modulAIntCTx[i].getValue().toString(), "Module " + m.getAddress() + " Alarm ");
			if(modulAInt[i] == -1){
				error = true;
			}
			
		}
		
		if( !error){
			device.setName((String) dlCNameTx.getValue());
			device.setGroup((String) dlCGroupTx.getValue());
			device.setMeassurementIntervall(mInt);
			device.setProcessIntervall(Device.getProcessIntervallInt((String)dlCPIntSel.getValue(), app.langHelper));
			device.setAlarmIntervall(aInt);
			
			for(int i = 1; i < (device.getModuls()+1); i++){
				Modul m = device.getModul(i);
				
				m.setMeassurementIntervall(modulMInt[i]);
				m.setProcessIntervall(modulPInt[i]);
				m.setAlarmIntervall(modulAInt[i]);
				
				channelNr = 1;
				for(int j = 0; j < channelModulAddress.length; j++){
					if(channelModulAddress[j] == m.getAddress()){
						Channel c = m.getChannel(channelNr++);
						c.setName((String)channelNameCTx[j].getValue());
						c.setGroup((String)channelGroupCTx[j].getValue());
					}
				}
			}
			
			YesNoPopupScreen ackPopup = new YesNoPopupScreen(app, "Save settings", "Are you sure you want to save the device settings?");
			ackPopup.addListener(new PopupClosedListener(){
				@Override
				public void popupClosedEvent(YesNoPopupScreen e) {
					if(e.isYesClicked()){
						if(app.deviceContainer.SaveDevice(device)){
							app.getMainWindow().showNotification("Settings successfully saved");
						}else{
							app.getMainWindow().showNotification("Error: Couldn't save settings");
						}
						if(!registered){
							if(app.sql.RegisterDevice(device)){
								device.setRegistered(true);
								
							}
						}
					}
					app.Update();
				}
			});
		}
		
		
		
	}
		
}














