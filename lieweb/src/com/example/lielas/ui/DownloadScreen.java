package com.example.lielas.ui;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.HeaderScreen;

import lielas.*;

import com.vaadin.data.Item;
import com.vaadin.terminal.Sizeable;
import com.vaadin.terminal.StreamResource;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.Panel;
import com.vaadin.ui.VerticalSplitPanel;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.NativeButton;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Table;
import com.vaadin.ui.Label;
import com.vaadin.ui.themes.Reindeer;


@SuppressWarnings({ "serial", "unused" })

public class DownloadScreen extends Panel{
	
	private LielasApplication app;
	
	private VerticalSplitPanel vSplit = null;
	
	private NativeButton downloadBttn = null;
	
	private Table downloadTable = null;
	
	private Label downloadHelpText;
	
	public DownloadScreen(final LielasApplication app){
		this.app = app;
		Activate();
		
		HorizontalLayout hLayout = new HorizontalLayout();
		hLayout.setSizeFull();
		
		setSizeFull();
		
		vSplit = new VerticalSplitPanel();
		vSplit.setLocked(true);
		vSplit.setSplitPosition(0, Sizeable.UNITS_PIXELS);
		vSplit.setWidth(1150, Sizeable.UNITS_PIXELS);
		vSplit.setHeight(100, Sizeable.UNITS_PERCENTAGE);
		vSplit.addStyleName("downloadscreen");
		
		// Filter
		
		HorizontalLayout firstSplitLayout = new HorizontalLayout();
		firstSplitLayout.setSizeFull();
		vSplit.setFirstComponent(firstSplitLayout);		
		
		// Table
		
		VerticalLayout secondSplitLayout = new VerticalLayout();
		secondSplitLayout.setSizeFull();
		vSplit.setSecondComponent(secondSplitLayout);
	
		downloadTable = new Table(app.langHelper.GetString(LanguageHelper.DL_TABLE_CAPTION));
		downloadTable.setWidth(1000, Sizeable.UNITS_PIXELS);
		downloadTable.setSelectable(true);
		downloadTable.setMultiSelect(true);
		downloadTable.setImmediate(true);
		downloadTable.setPageLength(0);
		downloadTable.setStyleName("downloadscreen");
		downloadTable.setSortDisabled(true);
		secondSplitLayout.addComponent(downloadTable);
		secondSplitLayout.setComponentAlignment(downloadTable,  Alignment.TOP_LEFT);
		
		downloadTable.addContainerProperty("", String.class, null);
		downloadTable.addContainerProperty(LanguageHelper.DL_TABLE_COL_ADDRESS, String.class, null);
		downloadTable.addContainerProperty(LanguageHelper.DL_TABLE_COL_NAME, String.class, null);
		downloadTable.addContainerProperty(LanguageHelper.DL_TABLE_COL_GROUP, String.class, null);
		downloadTable.addContainerProperty(LanguageHelper.DL_TABLE_COL_MODUL, String.class, null);
		downloadTable.addContainerProperty(LanguageHelper.DL_TABLE_COL_CHANNEL, String.class, null);
		downloadTable.addContainerProperty(LanguageHelper.DL_TABLE_COL_SENSOR, String.class, null);
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_ADDRESS, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_ADDRESS));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_NAME, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_NAME));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_GROUP));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_MODUL, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_MODUL));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_CHANNEL, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_CHANNEL));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_SENSOR, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_SENSOR));

		FillDownloadTable();
		
		downloadHelpText = new Label(app.langHelper.GetString(LanguageHelper.DL_HELPTEXT_MULTISELECT));
		downloadHelpText.addStyleName("downloadscreen");
		downloadHelpText.addStyleName("helptext");
		secondSplitLayout.addComponent(downloadHelpText);
		secondSplitLayout.setComponentAlignment(downloadHelpText, Alignment.TOP_LEFT);
		
		// Download-Button
		
		downloadBttn = new NativeButton(app.langHelper.GetString(LanguageHelper.DL_BTTN_DOWNLOAD));
		downloadBttn.addStyleName("downloadscreen");
		secondSplitLayout.addComponent(downloadBttn);
		secondSplitLayout.setComponentAlignment(downloadBttn, Alignment.TOP_RIGHT);
		secondSplitLayout.setExpandRatio(downloadBttn, 1.0f);
		
		downloadBttn.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event) {
				// TODO Auto-generated method stub
				BttnDownloadClicked(event);
			}
		});
		
		hLayout.addComponent(vSplit);
		hLayout.setComponentAlignment(vSplit, Alignment.TOP_CENTER);
		setContent(hLayout);
	}
	
	public void Update(){
		downloadBttn.setCaption(app.langHelper.GetString(LanguageHelper.DL_BTTN_DOWNLOAD));
		
		downloadTable.setCaption(app.langHelper.GetString(LanguageHelper.DL_TABLE_CAPTION));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_ADDRESS, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_ADDRESS));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_NAME, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_NAME));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_GROUP, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_GROUP));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_MODUL, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_MODUL));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_CHANNEL, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_CHANNEL));
		downloadTable.setColumnHeader(LanguageHelper.DL_TABLE_COL_SENSOR, app.langHelper.GetString(LanguageHelper.DL_TABLE_COL_SENSOR));
		
		downloadHelpText.setValue(app.langHelper.GetString(LanguageHelper.DL_HELPTEXT_MULTISELECT));
		
	}
	
	public void Activate(){
		app.headerScreen.setDownloadButtonActive();
	}
	
	private void FillDownloadTable(){  
		int rows = 1;
		boolean firstDeviceModul = true;
		
		// Fill Table
		
		Device d = app.deviceContainer.firstItemId();
		
		for( int i = 0; i < app.deviceContainer.size(); i++){
			for(int j = 1; j < (d.getModuls()+1); j++){
				Modul m = d.getModul(j);
				if(m != null){
					for(int k = 1; k < (m.getChannels()+1); k++){
						Channel c = m.getChannel(k);
						if(c != null){
							if(firstDeviceModul){
								downloadTable.addItem(new Object[]{ rows, d.getAddress(), d.getName(), d.getGroup(), j, k, c.getType()}, rows++);
								firstDeviceModul = false;
							}else{

								downloadTable.addItem(new Object[]{ rows, "", "", "", j, k, c.getType()}, rows++);
							}
						}
					}
				}
			}
			firstDeviceModul = true;
			d = app.deviceContainer.nextItemId(d);
		}
		
		// Style Table
		
		downloadTable.setCellStyleGenerator(new Table.CellStyleGenerator() {
			
			@Override
			public String getStyle(Object itemId, Object propertyId) {
				if( propertyId == null){
					// Style row
					Item item = downloadTable.getItem(itemId);
					String address = (String) item.getItemProperty(LanguageHelper.DL_TABLE_COL_ADDRESS).getValue();
					if(address != "" && address != null){
						return "bold";
					}else{
						return "nobackground";
					}
				}
				return null;
			}
		});
		
	}
	
	private void BttnDownloadClicked(ClickEvent event){
		CSVHelper csv = new CSVHelper(app);
		
		StreamResource sr = csv.GetCSVFile("data");
		app.getMainWindow().open(sr);
		csv = null;
	}
	
}

















