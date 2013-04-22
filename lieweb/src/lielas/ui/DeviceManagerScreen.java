package lielas.ui;

import java.util.Iterator;

import lielas.LiewebUI;
import lielas.ui.HeaderScreen;
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
import com.vaadin.ui.Notification;
import com.vaadin.ui.Accordion;
import com.vaadin.ui.Table;
import com.vaadin.ui.Label;
import com.vaadin.ui.TabSheet;
import com.vaadin.ui.NativeSelect;
import com.vaadin.data.Item;
import com.vaadin.data.Property.ValueChangeEvent;
import com.vaadin.terminal.*;

import lielas.core.Device;
import lielas.core.DeviceContainer;
import lielas.core.LanguageHelper;
import lielas.core.NewDeviceContainer;

@SuppressWarnings({ "serial", "unused" })
public class DeviceManagerScreen extends Panel{

	private LiewebUI app;
		
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
	
	public DeviceManagerScreen(final LiewebUI app){
		this.app = app; 
		app.headerScreen.setDeviceManagerButtonActive();		
		setSizeFull();
		
		Label hSpacer = new Label();
		hSpacer.setWidth(10, Unit.PIXELS);
		
		Label vSpacer = new Label();
		vSpacer.setHeight(10, Unit.PIXELS);
		
		HorizontalLayout hLayout = new HorizontalLayout();
		hLayout.setSizeFull();
		
		
		
		dmTabSheet = new TabSheet();
		dmTabSheet.setSizeFull();
		dmTabSheet.setWidth(1150, Unit.PIXELS);
		dmTabSheet.addStyleName("tab-devicemanager");
		
		
			
		deviceListLayout = new HorizontalLayout();
		deviceListLayout.setStyleName("deviceManager");
		//deviceListLayout.setSizeFull();
		
		dmTabSheet.addTab(deviceListLayout, app.langHelper.GetString(LanguageHelper.DM_TAB_DEVICELIST));
		
		/************************************************************************************************************************
		 * 
		 * 		Device List View
		 * 
		 ************************************************************************************************************************/		
		
		
		deviceList = new Table(app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_CAPTION));
		deviceList.setWidth(620, Unit.PIXELS);
		deviceList.setSelectable(true);
		deviceList.setImmediate(true);
		deviceList.setPageLength(0);
		deviceList.setStyleName("devicelist");
		deviceList.setSortEnabled(false);
		
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
				deviceList.addItem(new Object[]{i+1, d.getMac(), d.getName(), d.getGroup()}, i+1);
			}
			d = app.deviceContainer.nextItemId(d);
		}
				
		deviceList.addValueChangeListener(new Table.ValueChangeListener() {
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
		
		dmTabSheet.getTab(deviceListLayout).setCaption(app.langHelper.GetString(LanguageHelper.DM_TAB_DEVICELIST));
		
		deviceList.setCaption(app.langHelper.GetString(LanguageHelper.DM_TABLE_DL_DETAILS_CAPTION));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_ADDRESS));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICENAME));
		deviceList.setColumnHeader(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP, app.langHelper.GetString(LanguageHelper.DM_TABLE_DEVICELIST_COL_DEVICEGROUP));
		
		deviceList.removeAllItems();
		
		Device d = app.deviceContainer.firstItemId();
		for( int i=0; i < app.deviceContainer.size(); i++){
			if(d.isRegistered()){
				deviceList.addItem(new Object[]{i+1, d.getAddress(), d.getName(), d.getGroup()}, i+1);
			}
			d = app.deviceContainer.nextItemId(d);
		}
		
		setDeviceListHeight();
		
		dmDetailScreen.Update();
		
			
	}


	private void setDeviceListHeight(){
		float height = 80; // min
		height += (float)app.deviceContainer.size() * 21;
		deviceListSpacer.setHeight(height, Unit.PIXELS);
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
	
	
}

