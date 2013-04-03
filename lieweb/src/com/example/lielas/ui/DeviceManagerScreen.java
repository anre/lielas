package com.example.lielas.ui;

import java.util.Iterator;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.HeaderScreen;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.Embedded;
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
import lielas.LanguageHelper;
import lielas.NewDeviceContainer;

@SuppressWarnings({ "serial", "unused" })
public class DeviceManagerScreen extends Panel{

	private LielasApplication app;
		
	private Table deviceList = null;
	private Table newDevicesList = null;
	
	private TabSheet dmTabSheet = null;
	
	private HorizontalLayout deviceListLayout = null;
	private HorizontalLayout newDevicesLayout = null;
	
	private VerticalLayout newDevicesDetailListLayout = null;
	
	private Label newDevicesDetailListHEaderLbl = null;
	
	private Label deviceListSpacer = null;
	private Label newDeviceListSpacer = null;
	
	private Label ndlDAddressLbl = null;
	private Label ndlCAddressLbl = null;
	private Label ndlDNameLbl = null;
	private TextField ndlCNameTx = null;
	private Label ndlDGroupLbl = null;
	private TextField ndlCGroupTx = null;
	private Label ndlDMIntLbl = null;
	private TextField ndlCMIntTx = null; 
	private Label ndlDPIntLbl = null;
	private NativeSelect ndlCPIntSel = null;
	private Label ndlDAIntLbl = null;
	private TextField ndlCAIntTx = null;
	
	private NativeButton ndlCheckBttn = null;
	
	private static final int detailVSpacerHeight = 5;
	
	private DeviceManagerDetailScreen dmDetailScreen = null;
	private DeviceManagerDetailScreen dmNewDetailScreen = null;
	
	public DeviceManagerScreen(final LielasApplication app){
		this.app = app; 
		app.headerScreen.setDeviceManagerButtonActive();		
		setSizeFull();
		
		Label hSpacer = new Label();
		hSpacer.setWidth(10, Sizeable.UNITS_PIXELS);
		
		Label vSpacer = new Label();
		vSpacer.setHeight(10, Sizeable.UNITS_PIXELS);
		
		HorizontalLayout hLayout = new HorizontalLayout();
		hLayout.setSizeFull();
		
		
		
		dmTabSheet = new TabSheet();
		dmTabSheet.setSizeFull();
		dmTabSheet.setWidth(1150, Sizeable.UNITS_PIXELS);
		dmTabSheet.addStyleName("tab-devicemanager");
		
		
			
		deviceListLayout = new HorizontalLayout();
		deviceListLayout.setStyleName("deviceManager");
		//deviceListLayout.setSizeFull();
		newDevicesLayout = new HorizontalLayout();
		newDevicesLayout.setStyleName("deviceManager");
		//newDevicesLayout.setSizeFull();
		
		dmTabSheet.addTab(deviceListLayout, app.langHelper.GetString(LanguageHelper.DM_TAB_DEVICELIST));
		dmTabSheet.addTab(newDevicesLayout, app.langHelper.GetString(LanguageHelper.DM_TAB_NEWDEVICES)); 
		
		/************************************************************************************************************************
		 * 
		 * 		Device List View
		 * 
		 ************************************************************************************************************************/		
		
		
		deviceList = new Table(app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_CAPTION));
		deviceList.setWidth(620, Sizeable.UNITS_PIXELS);
		deviceList.setSelectable(true);
		deviceList.setImmediate(true);
		deviceList.setPageLength(0);
		deviceList.setStyleName("devicelist");
		deviceList.setSortDisabled(true);
		
		deviceList.addContainerProperty("", Integer.class, null);
		deviceList.addContainerProperty(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS, String.class, null);
		deviceList.addContainerProperty(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME, String.class, null);
		deviceList.addContainerProperty(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP, String.class, null);
		
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP));
		
		Device d = app.deviceContainer.firstItemId();
		for( int i=0; i < app.deviceContainer.size(); i++){
			if(d.isRegistered()){
				deviceList.addItem(new Object[]{i+1, d.getAddress(), d.getName(), d.getGroup()}, i+1);
			}
			d = app.deviceContainer.nextItemId(d);
		}
				
		deviceList.addListener(new Table.ValueChangeListener() {
			public void valueChange(ValueChangeEvent event) {
				DeviceListClickedHandler(event);
			}
		});

		deviceListLayout.addComponent(deviceList);
		
		deviceListSpacer = new Label();
		setDeviceListHeight();
		
		deviceListLayout.addComponent(deviceListSpacer);
		
		dmDetailScreen = new DeviceManagerDetailScreen(this.app);
		
		deviceListLayout.addComponent(dmDetailScreen);	
		
		
		/************************************************************************************************************************
		 * 
		 * 		new Devices List View
		 * 
		 ************************************************************************************************************************/
		
		
		newDevicesList = new Table(app.langHelper.GetString(LanguageHelper.DM_TABLE_NEWDEVICES_CAPTION));
		newDevicesList.setWidth(620, Sizeable.UNITS_PIXELS);
		newDevicesList.setSelectable(true);
		newDevicesList.setImmediate(true);
		newDevicesList.setPageLength(0);
		newDevicesList.setStyleName("newdevicelist");
		newDevicesList.setSortDisabled(true);
		
		newDevicesList.addContainerProperty("", Integer.class, null);
		newDevicesList.addContainerProperty(app.langHelper.GetString(LanguageHelper.DM_TABLE_NEWDEVICES_COL_ADDRESS), String.class, null);
		
		Device newDevice = app.deviceContainer.firstItemId();
		for(int i = 0; i< app.deviceContainer.size();i++){
			if(!newDevice.isRegistered()){
				newDevicesList.addItem(new Object[]{i+1, newDevice.getAddress()}, i+1);
			}
			newDevice = app.deviceContainer.nextItemId(newDevice);
		}
					
		
		newDevicesList.addListener(new Table.ValueChangeListener() {
			public void valueChange(ValueChangeEvent event) {
				NewDevicesListClickedHandler(event);
			}
		});
		

		newDevicesLayout.addComponent(newDevicesList); 

		newDeviceListSpacer = new Label();
		setNewDeviceListHeight();
		
		newDevicesLayout.addComponent(newDeviceListSpacer);
		
		
		dmNewDetailScreen = new DeviceManagerDetailScreen(this.app);
		newDevicesLayout.addComponent(dmNewDetailScreen);
		
		
		hLayout.addComponent(dmTabSheet);
		hLayout.setComponentAlignment(dmTabSheet, Alignment.TOP_CENTER);
		hLayout.setExpandRatio(dmTabSheet, 1.0f);
		hLayout.addComponent(vSpacer);
		setContent(hLayout);
	}
	
	
	public void Activate(){
		app.headerScreen.setDeviceManagerButtonActive();
	}
	

	
	public void Update(){
		
		Device newDeviceListSelectedDevice = dmNewDetailScreen.getDevice();

		dmTabSheet.getTab(deviceListLayout).setCaption(app.langHelper.GetString(LanguageHelper.DM_TAB_DEVICELIST));
		dmTabSheet.getTab(newDevicesLayout).setCaption(app.langHelper.GetString(LanguageHelper.DM_TAB_NEWDEVICES));
		
		deviceList.setCaption(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_CAPTION));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP));
		
		deviceList.removeAllItems();
		newDevicesList.removeAllItems();
		
		Device d = app.deviceContainer.firstItemId();
		for( int i=0; i < app.deviceContainer.size(); i++){
			if(d.isRegistered()){
				deviceList.addItem(new Object[]{i+1, d.getAddress(), d.getName(), d.getGroup()}, i+1);
				if(newDeviceListSelectedDevice != null){
					if(d.getID() == newDeviceListSelectedDevice.getID()){
						dmNewDetailScreen.setDevice(null, false);
					}
				}
			}else{
				newDevicesList.addItem(new Object[]{i+1, d.getAddress()}, i+1);
			}
			d = app.deviceContainer.nextItemId(d);
		}
		
		setDeviceListHeight();
		
		dmDetailScreen.Update();
		dmNewDetailScreen.Update();
		
		newDevicesList.setCaption(app.langHelper.GetString(LanguageHelper.DM_TABLE_NEWDEVICES_CAPTION));
		newDevicesList.setColumnHeader(LanguageHelper.DM_TABLE_NEWDEVICES_COL_ADDRESS, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS));
			
	}


	private void setDeviceListHeight(){
		float height = 80; // min
		height += (float)app.deviceContainer.size() * 21;
		deviceListSpacer.setHeight(height, Sizeable.UNITS_PIXELS);
	}
	
	private void setNewDeviceListHeight(){
		float height = 80; // min
		height += (float)app.deviceContainer.size() * 21;
		newDeviceListSpacer.setHeight(height, Sizeable.UNITS_PIXELS);
	}	
	
	
	private void DeviceListClickedHandler(ValueChangeEvent event){
		Device device = null;
		
		if(deviceList.getValue() == null){
			dmDetailScreen.setVisible(false);
		}else{
			device = app.deviceContainer.getIdByIndex((Integer)deviceList.getValue()-1);
			dmDetailScreen.setDevice(device, true);
			dmDetailScreen.Update();
		}
	}
	
	private void NewDevicesListClickedHandler(ValueChangeEvent event){
		Device device = null;
		
		if(newDevicesList.getValue() == null){
			dmNewDetailScreen.setVisible(false);
		}else{
			device = app.deviceContainer.getIdByIndex((Integer)newDevicesList.getValue()-1);
			dmNewDetailScreen.setDevice(device, false);
			dmNewDetailScreen.Update();
		}	
	}
	
}

