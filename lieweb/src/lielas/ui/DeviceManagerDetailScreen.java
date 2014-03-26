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

package lielas.ui;

import lielas.LiewebUI;
import lielas.ui.HeaderScreen;
import lielas.ui.YesNoPopupScreen.PopupClosedListener;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.Embedded;
import com.vaadin.ui.GridLayout;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.Notification;
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
import com.vaadin.ui.Accordion;
import com.vaadin.ui.Table;
import com.vaadin.ui.Label;
import com.vaadin.ui.TabSheet;
import com.vaadin.ui.NativeSelect;
import com.vaadin.data.Item;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.shared.ui.label.ContentMode;
import com.vaadin.terminal.*;

import lielas.core.Device;
import lielas.core.DeviceContainer;
import lielas.core.Channel;
import lielas.core.ExceptionHandler;
import lielas.core.Modul;
import lielas.core.LanguageHelper;
import lielas.core.NewDeviceContainer;
import lielas.core.TcpClient;
import lielas.core.UserContainer;

@SuppressWarnings({ "serial", "unused" })
public class DeviceManagerDetailScreen extends VerticalLayout{

	private LiewebUI app;
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
	private Label dlDSupplyStateLbl = null;
	private Label dlCSupplyStateLbl = null;

	private NativeButton dlSaveBttn = null;
	
	private static final int detailVSpacerHeight = 5;
	
	public static boolean TYPE_REGISTERED = true;
	public static boolean TYPE_NOT_REGISTERED = false;
	
	private Label modulAddressDLbl[];
	
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
	
	public DeviceManagerDetailScreen(final LiewebUI app){
		this.app = app;
		this.device = null;
		this.setWidth(460, Unit.PIXELS);
		this.addStyleName("detaillist");
		this.registered = false;
	}
	
	public DeviceManagerDetailScreen(final LiewebUI app, Device d, boolean registered){
		this.app = app;
		this.device = d;
		this.setWidth(460, Unit.PIXELS);
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
		
		dlCAddressLbl.setValue(device.getMac());
		dlCNameTx.setValue(device.getName());
		dlCGroupTx.setValue(device.getGroup());	

		for(int i = 1; i < (device.getModuls()+1) ; i++){
			Modul m = device.getModul(i);
			for( int j = 1; j <= m.getChannels();j++){
				Channel c = m.getChannel(j);
				channelNameCTx[cmAddress].setValue(c.getName());
				channelGroupCTx[cmAddress].setValue(c.getGroup());
				if(registered){
					value = app.sql.GetLastValue(device, m, c, app.user);
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
		detailListHeaderLbl.setWidth(460, Unit.PIXELS);
		detailListHeaderLbl.addStyleName("detaillist-header");
		detailListHeaderLbl.setHeight(29, Unit.PIXELS);
		this.addComponent(detailListHeaderLbl);
		
		VerticalLayout detailListBodyLayout = new VerticalLayout();
		detailListBodyLayout.setWidth(460, Unit.PIXELS);
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
		dlBodyLayout.setWidth(438, Unit.PIXELS);
		dlBodyLayout.setColumnExpandRatio(0, 0);
		dlBodyLayout.setColumnExpandRatio(1, 0);
		dlBodyLayout.setColumnExpandRatio(2, 0);
		detailListBodyLayout.addComponent(dlBodyLayout);
		
		// show Address
		
		dlDAddressLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_ADDRESS));
		dlDAddressLbl.setWidth(109, Unit.PIXELS);
		dlDAddressLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDAddressLbl, 0, 0, 1, 0);
		dlCAddressLbl = new Label("");
		dlCAddressLbl.setWidth(215, Unit.PIXELS);
		dlCAddressLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlCAddressLbl, 2, 0);
		
		// show Name
		dlDNameLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_NAME));
		dlDNameLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDNameLbl, 0, 1, 1, 1);
		dlCNameTx = new TextField();
		dlCNameTx.setStyleName("detaillist-body");
		dlCNameTx.addStyleName("dmDetailGridPadding");
		dlCNameTx.setWidth(215, Unit.PIXELS);
		dlBodyLayout.addComponent(dlCNameTx, 2, 1);
		
		// Group
		dlDGroupLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_GROUP));
		dlDGroupLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDGroupLbl, 0, 2, 1, 2);
		dlCGroupTx = new TextField();
		dlCGroupTx.setStyleName("detaillist-body");
		dlCGroupTx.addStyleName("dmDetailGridPadding");
		dlCGroupTx.setWidth(215, Unit.PIXELS);
		dlBodyLayout.addComponent(dlCGroupTx, 2, 2);
		
		// Meassurement Intervall
	/*	dlDMIntLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_MEASSUREMENT_INTERVALL));
		dlDMIntLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDMIntLbl, 0, 3, 1, 3);
		dlCMIntTx = new TextField();
		dlCMIntTx.setStyleName("detaillist-body");
		dlCMIntTx.addStyleName("dmDetailGridPadding");
		dlCMIntTx.setWidth(215, Unit.PIXELS);
		dlBodyLayout.addComponent(dlCMIntTx, 2, 3);
		
		// Process Intervall
		dlDPIntLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_PROCESS_INTERVALL));
		dlDPIntLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDPIntLbl, 0, 4, 1, 4);
		dlCPIntSel = new NativeSelect();
		dlCPIntSel.setStyleName("detaillist-body");
		dlCPIntSel.addStyleName("dmDetailGridPadding");
		dlCPIntSel.addStyleName("dm");
		dlCPIntSel.setNullSelectionAllowed(false);
		dlCPIntSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_LIFETIME));
		dlCPIntSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_NORMAL));
		dlCPIntSel.addItem(app.langHelper.GetString(LanguageHelper.AT_PINT_OPT_FAST_GATHER));
		dlCPIntSel.setWidth(215, Unit.PIXELS);
		dlBodyLayout.addComponent(dlCPIntSel, 2, 4);
		
		// Alarm Intervall
		dlDAIntLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_ALARM_INTERVALL));
		dlDAIntLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDAIntLbl, 0, 5, 1, 5);
		dlCAIntTx = new TextField();
		dlCAIntTx.setStyleName("detaillist-body");
		dlCAIntTx.addStyleName("dmDetailGridPadding");
		dlCAIntTx.setWidth(215, Unit.PIXELS);
		dlBodyLayout.addComponent(dlCAIntTx, 2, 5); */

		// Supply
		dlDSupplyLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_SUPPLY));
		dlDSupplyLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDSupplyLbl, 0, 3, 1, 3);
		dlCSupplyLbl = new Label(app.langHelper.GetString(this.device.getDeviceVSourceString()));
		dlCSupplyLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlCSupplyLbl, 2, 3);	  
		
		//SupplyState
		dlDSupplyStateLbl = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_SUPPLY_STATE));
		dlDSupplyStateLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlDSupplyStateLbl, 0, 4, 1, 4);
		dlCSupplyStateLbl = new Label(app.langHelper.GetString(this.device.getDeviceVSourceStateString()));
		dlCSupplyStateLbl.addStyleName("dmDetailGridPadding");
		dlBodyLayout.addComponent(dlCSupplyStateLbl, 2, 4);	  

		// Moduls
		modulAddressDLbl = new Label[20];
		
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
			//modulAddressDLbl[i].setWidth(215, Unit.PIXELS);
			dlBodyLayout.addComponent(modulAddressDLbl[i], 0, lines, 2, lines++);
			
			VerticalLayout mSpacerLayout = new VerticalLayout();
			mSpacerLayout.setHeight(10, Unit.PIXELS);
			dlBodyLayout.addComponent(mSpacerLayout, 0, lines++ );
			
			for( int j = 1; j <= device.getChannels(i);j++){
				channelModulAddress[cmAddress] = device.getModul(i).getAddress();
				
				Channel c = device.getChannel(i, j);
				
				// Address
				
				channelAddressDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_CHANNEL));
				channelAddressDLbl[cmAddress].addStyleName("dmDetailGridPadding");
				channelAddressDLbl[cmAddress].addStyleName("bold");
				dlBodyLayout.addComponent(channelAddressDLbl[cmAddress], 1, lines );
				
				channelAddressCLbl[cmAddress] = new Label(Integer.toString(j));
				channelAddressCLbl[cmAddress].addStyleName("bold");
				channelAddressCLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelAddressCLbl[cmAddress], 2, lines++);
				
				// Name
				
				channelNameDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_NAME));
				channelNameDLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelNameDLbl[cmAddress], 1, lines );
				
				channelNameCTx[cmAddress] = new TextField();
				channelNameCTx[cmAddress].setStyleName("detaillist-body");
				channelNameCTx[cmAddress].addStyleName("dmDetailGridPadding");
				channelNameCTx[cmAddress].setWidth(215, Unit.PIXELS);
				dlBodyLayout.addComponent(channelNameCTx[cmAddress], 2, lines++);
				
				// Group
				
				channelGroupDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_GROUP));
				channelGroupDLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelGroupDLbl[cmAddress], 1, lines );
				
				channelGroupCTx[cmAddress] = new TextField();
				channelGroupCTx[cmAddress].setStyleName("detaillist-body");
				channelGroupCTx[cmAddress].addStyleName("dmDetailGridPadding");
				channelGroupCTx[cmAddress].setWidth(215, Unit.PIXELS);
				dlBodyLayout.addComponent(channelGroupCTx[cmAddress], 2, lines++);
				
				// Type
				
				channelTypeDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_TYPE));
				channelTypeDLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelTypeDLbl[cmAddress], 1, lines );
				
				channelTypeCLbl[cmAddress] = new Label(c.getType());
				channelTypeCLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelTypeCLbl[cmAddress], 2, lines++ );
				
				// Unit
				
				channelUnitDLbl[cmAddress] = new Label(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_UNIT));
				channelUnitDLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelUnitDLbl[cmAddress], 1, lines );
				
				channelUnitCLbl[cmAddress] = new Label(c.getUnit());
				channelUnitCLbl[cmAddress].addStyleName("dmDetailGridPadding");
				dlBodyLayout.addComponent(channelUnitCLbl[cmAddress], 2, lines++ );
				
				//Last value
				if(registered){
					channelLastValueDLbl[cmAddress] = new Label("Last Value");
					channelLastValueDLbl[cmAddress].addStyleName("dmDetailGridPadding");
					dlBodyLayout.addComponent(channelLastValueDLbl[cmAddress], 1, lines );

					channelLastValueCLbl[cmAddress] = new Label(" ", ContentMode.PREFORMATTED);
					channelLastValueCLbl[cmAddress].addStyleName("dmDetailGridPadding");
					dlBodyLayout.addComponent(channelLastValueCLbl[cmAddress], 2, lines++ );
				}
				// Spacer
				VerticalLayout channelSpacerLayout = new VerticalLayout();
				channelSpacerLayout.setHeight(10, Unit.PIXELS);
				dlBodyLayout.addComponent(channelSpacerLayout, 0, lines++ );
				
				cmAddress += 1;
			}
			
		}
		
		// Footer
		
		HorizontalLayout detailListFooterLayout = new HorizontalLayout();
		detailListFooterLayout.setWidth(460, Unit.PIXELS);
		detailListFooterLayout.setHeight(40, Unit.PIXELS);
		detailListFooterLayout.setStyleName("detaillist-footer");
		detailListFooterLayout.setMargin(false);
		
		// Save Button

		Label dlFooterVSpacer = new Label();
		detailListFooterLayout.addComponent(dlFooterVSpacer);
		detailListFooterLayout.setComponentAlignment(dlFooterVSpacer, Alignment.TOP_RIGHT);
		
		dlSaveBttn = new NativeButton(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_SAVE_BTTN));
		dlSaveBttn.addStyleName("detaillist-footer");
		detailListFooterLayout.addComponent(dlSaveBttn);
		detailListFooterLayout.setComponentAlignment(dlSaveBttn, Alignment.TOP_CENTER);		
		
		dlSaveBttn.addClickListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
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
				if(mInt < 1){
					Notification.show(type + "Inteterval is to small, minimum is 60s", Notification.Type.WARNING_MESSAGE);
					return -1;
				}else if(mInt > 43200){
					Notification.show(type + "Inteterval is to big, maximum is 43200s", Notification.Type.WARNING_MESSAGE);
					return -1;
				}
				return mInt;
			}catch(Exception e){
				Notification.show(type + "Inteterval is no valid number", Notification.Type.WARNING_MESSAGE);
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
		
		if( !error){
			device.setName((String) dlCNameTx.getValue());
			device.setGroup((String) dlCGroupTx.getValue());
			
			for(int i = 1; i < (device.getModuls()+1); i++){
				Modul m = device.getModul(i);
				
				m.setMeassurementIntervall(modulMInt[i]);
				
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
							Notification.show("Settings successfully saved");
							TcpClient tcpClient = new TcpClient(app.config.getTcpServerAddress(), app.config.getTcpServerPort());
							tcpClient.SendSaveDeviceSettingsMessage(app.user.getID(), device.getID());
						}else{
							Notification.show("Error: Couldn't save settings");
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














