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
import lielas.core.CSVHelper;
import lielas.core.Channel;
import lielas.core.Device;
import lielas.core.LanguageHelper;
import lielas.core.Modul;
import lielas.ui.HeaderScreen;

import lielas.*;

import com.vaadin.data.Item;
import com.vaadin.server.FileDownloader;
import com.vaadin.server.StreamResource;
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
	
	private LiewebUI app;
	
	private VerticalSplitPanel vSplit = null;
	
	private NativeButton downloadBttn = null;
	
	private Table downloadTable = null;
	
	private Label downloadHelpText;
	
	public DownloadScreen(final LiewebUI app){
		this.app = app;
		Activate();
		setSizeFull();
		
		//first hLayout
		HorizontalLayout firstHLayout = new HorizontalLayout();
		firstHLayout.setSizeFull();
		
		//first vLayout
		VerticalLayout firstVLayout = new VerticalLayout();
		//firstVLayout.setSizeFull();
		firstVLayout.setWidth(1150, Unit.PIXELS);
		firstVLayout.setHeight(100, Unit.PERCENTAGE);
		firstVLayout.setStyleName("downlaodscreen");
		firstHLayout.addComponent(firstVLayout);
		firstHLayout.setComponentAlignment(firstVLayout, Alignment.TOP_CENTER);
		
		//second vLayout
		VerticalLayout secondVLayout = new VerticalLayout();
		secondVLayout.setWidth(1150, Unit.PIXELS);
		secondVLayout.setStyleName("downlaodscreen");
		firstVLayout.addComponent(secondVLayout);
		firstVLayout.setExpandRatio(secondVLayout, 1.0f);
		
		//top Spacer
		Label topSpacer = new Label();
		topSpacer.setHeight(20, Unit.PIXELS);
		secondVLayout.addComponent(topSpacer);
		
		//second hLayout
		HorizontalLayout secondHLayout = new HorizontalLayout();
		secondHLayout.setSizeFull();
		secondVLayout.addComponent(secondHLayout);
		secondVLayout.setExpandRatio(secondHLayout, 1.0f);
		
		Label leftSpacer = new Label();
		leftSpacer.setWidth(75, Unit.PIXELS);
		secondHLayout.addComponent(leftSpacer);
		
		//content Layout
		VerticalLayout contentLayout = new VerticalLayout();
		contentLayout.setSizeFull();
		secondHLayout.addComponent(contentLayout);
		secondHLayout.setExpandRatio(contentLayout, 1.0f);

		
		// Download-Button
		
		downloadBttn = new NativeButton(app.langHelper.GetString(LanguageHelper.DL_BTTN_DOWNLOAD));
		downloadBttn.addStyleName("downloadscreen");
		contentLayout.addComponent(downloadBttn);
		contentLayout.setComponentAlignment(downloadBttn, Alignment.TOP_RIGHT);
		
		CSVHelper csv = new CSVHelper(app);
		
		StreamResource sr = csv.GetCSVFile("data");
		FileDownloader fileDownloader = new FileDownloader(sr);
		fileDownloader.extend(downloadBttn);
		
		//bottom Spacer
		Label midSpacer = new Label();
		midSpacer.setHeight(20, Unit.PIXELS);
		contentLayout.addComponent(midSpacer);
		
		//table	
		downloadTable = new Table(app.langHelper.GetString(LanguageHelper.DL_TABLE_CAPTION));
		downloadTable.setWidth(1000, Unit.PIXELS);
		//downloadTable.setHeight(100, Unit.PERCENTAGE);
		downloadTable.setSelectable(true);
		downloadTable.setMultiSelect(true);
		downloadTable.setImmediate(true);
		downloadTable.setPageLength(0);
		downloadTable.setStyleName("downloadscreen");
		downloadTable.setSortEnabled(false);
		contentLayout.addComponent(downloadTable);
		contentLayout.setComponentAlignment(downloadTable,  Alignment.TOP_LEFT);
		contentLayout.setExpandRatio(downloadTable, 1.0f);
		
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
		
		//help text
		downloadHelpText = new Label(app.langHelper.GetString(LanguageHelper.DL_HELPTEXT_MULTISELECT));
		downloadHelpText.addStyleName("downloadscreen");
		downloadHelpText.addStyleName("helptext");
		downloadHelpText.setHeight(20, Unit.PIXELS);
		contentLayout.addComponent(downloadHelpText);
		contentLayout.setComponentAlignment(downloadHelpText, Alignment.TOP_LEFT);

		//bottom Spacer
		Label bottomSpacer = new Label();
		bottomSpacer.setHeight(20, Unit.PIXELS);
		contentLayout.addComponent(bottomSpacer);
		
		setContent(firstHLayout);		
		
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

		downloadTable.removeAllItems();
		FillDownloadTable();
	}
	
	public void Activate(){
		app.headerScreen.setDownloadButtonActive();
	}
	
	private void FillDownloadTable(){  
		Integer rows = 1;
		boolean firstDeviceModul = true;
		
		// Fill Table
		
		Device d = app.deviceContainer.firstItemId();
		
		for( Integer i = 0; i < app.deviceContainer.size(); i++){
		//for( Integer i = 0; i < 2; i++){
			for(Integer j = 1; j < (d.getModuls()+1); j++){
				Modul m = d.getModul(j);
				if(m != null){
					for(Integer k = 1; k < (m.getChannels()+1); k++){
						Channel c = m.getChannel(k);
						if(c != null){
							if(firstDeviceModul){
								downloadTable.addItem(new Object[]{ rows.toString(), d.getMac(), d.getName(), d.getGroup(), j.toString(), k.toString(), c.getType()}, rows.toString());
								rows += 1;
								firstDeviceModul = false;
							}else{
								downloadTable.addItem(new Object[]{ rows.toString(), "", "", "", j.toString(), k.toString(), c.getType()}, rows.toString());
								rows += 1;
							}
						}
					}
				}
			}
			firstDeviceModul = true;
			d = app.deviceContainer.nextItemId(d);
		}
		
		// Style Table
		
		/*downloadTable.setCellStyleGenerator(new Table.CellStyleGenerator() {
			
			@Override
			public String getStyle(Table source, Object itemId,
					Object propertyId) {
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
		});*/
		
	}
	
	private void BttnDownloadClicked(ClickEvent event){
		
		CSVHelper csv = new CSVHelper(app);
		
		StreamResource sr = csv.GetCSVFile("data");
		FileDownloader fileDownloader = new FileDownloader(sr);
		//app.getPage().open(sr);
		csv = null;
	}
	
}

















