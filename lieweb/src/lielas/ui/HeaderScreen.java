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

import lielas.core.LanguageHelper;

import lielas.LiewebUI;
import lielas.ui.DownloadScreen;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.Embedded;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.TextField;
import com.vaadin.ui.Button;
import com.vaadin.ui.NativeButton;
import com.vaadin.ui.Panel;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.CheckBox;
import com.vaadin.ui.FormLayout;
import com.vaadin.ui.NativeSelect;
import com.vaadin.ui.Notification;
import com.vaadin.ui.Label;
import com.vaadin.ui.themes.BaseTheme;
import com.vaadin.server.ThemeResource;
import com.vaadin.terminal.*;

@SuppressWarnings({ "serial", "unused" })
public class HeaderScreen extends Panel{

	private LiewebUI app;
	
	private NativeButton DeviceManagerButton;
	private NativeButton DownloadButton;
	private NativeButton AnalyseButton;
	private NativeButton EventManagerButton;
	private NativeButton OptionsButton;
	private NativeButton HelpButton;
	private NativeButton LogoutButton;
	
	private NativeButton FirstLanguageLbl;
	private NativeButton SecondLanguageLbl;
	
	public HeaderScreen(final LiewebUI app){
		this.app = app;
		
		this.setHeight(73, Unit.PIXELS);
		this.addStyleName("header");
		
		//setSizeFull();
		//setScrollable(false);
			
		Label leftSpacer = new Label();
		leftSpacer.setWidth(40, Unit.PIXELS);
		
		Label rightSpacer =  new Label();
		rightSpacer.setWidth(20, Unit.PIXELS);
		
		Label middleSpacer = new Label();
		middleSpacer.setWidth(100, Unit.PERCENTAGE);
		
		FirstLanguageLbl = new NativeButton("EN");
		SecondLanguageLbl = new NativeButton("DE");
		Label delimiterLbl = new Label("|");
		
		FirstLanguageLbl.setWidth(30, Unit.PIXELS);
		FirstLanguageLbl.setHeight(28, Unit.PIXELS);
		FirstLanguageLbl.setStyleName("header-language");
		FirstLanguageLbl.addStyleName(BaseTheme.BUTTON_LINK);
		delimiterLbl.setWidth(10, Unit.PIXELS);
		delimiterLbl.setHeight(28, Unit.PIXELS);
		delimiterLbl.setStyleName("header-language");
		delimiterLbl.addStyleName(BaseTheme.BUTTON_LINK);
		SecondLanguageLbl.setWidth(30, Unit.PIXELS);
		SecondLanguageLbl.setStyleName("header-language");
		SecondLanguageLbl.addStyleName(BaseTheme.BUTTON_LINK);
		SecondLanguageLbl.setHeight(28, Unit.PIXELS);
		
		FirstLanguageLbl.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});
		
		SecondLanguageLbl.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});
		
		HorizontalLayout lo =  new HorizontalLayout();
		lo.setHeight(73, Unit.PIXELS);
		lo.setWidth(100, Unit.PERCENTAGE);
		lo.setMargin(false);
		lo.setStyleName("header");
		
		DeviceManagerButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_DEVICEMANAGER));
		DeviceManagerButton.setStyleName("header");
		DeviceManagerButton.setVisible(false);
		DeviceManagerButton.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});
		
		DownloadButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_DOWNLOAD));
		DownloadButton.setStyleName("header");
		DownloadButton.setVisible(false);
		DownloadButton.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});
		
		AnalyseButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_ANALYSE));
		AnalyseButton.setStyleName("header");
		AnalyseButton.addStyleName("inactive");
		AnalyseButton.setVisible(false);
		AnalyseButton.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});
		
		EventManagerButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_EVENTMANAGER));
		EventManagerButton.setStyleName("header");
		EventManagerButton.addStyleName("inactive");
		EventManagerButton.setVisible(false);
		EventManagerButton.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});	
		
		OptionsButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_OPTIONS));
		OptionsButton.setStyleName("header");
		OptionsButton.setVisible(false);
		OptionsButton.addClickListener(new NativeButton.ClickListener() {
			public void buttonClick(ClickEvent event) {
				ButtonClickHandler(event);
			}
		});
		
		HelpButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_HELP));
		HelpButton.setStyleName("header");
		HelpButton.addStyleName("inactive");
		HelpButton.setVisible(false);
		HelpButton.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});
		
		LogoutButton = new NativeButton(app.langHelper.GetString(LanguageHelper.TAB_LOGOUT));
		LogoutButton.setStyleName("header");
		LogoutButton.setVisible(false);
		LogoutButton.addClickListener(new NativeButton.ClickListener(){
			public void buttonClick(ClickEvent event){
				ButtonClickHandler(event);
			}
		});		
		

		ThemeResource res_header = new ThemeResource("images/logo.png");
		Embedded pic_header = new Embedded(null, res_header);	
		
		lo.addComponent(leftSpacer);
		lo.addComponent(DeviceManagerButton);
		lo.addComponent(DownloadButton);
		lo.addComponent(AnalyseButton);
		lo.addComponent(EventManagerButton);
		lo.addComponent(OptionsButton);
		lo.addComponent(HelpButton);
		lo.addComponent(LogoutButton);
		lo.addComponent(middleSpacer);
		lo.addComponent(FirstLanguageLbl);
		lo.addComponent(delimiterLbl);
		lo.addComponent(SecondLanguageLbl);
		lo.addComponent(pic_header);
		lo.addComponent(rightSpacer);
		
		lo.setExpandRatio(middleSpacer, 1.0f);
		
		lo.setComponentAlignment(DeviceManagerButton, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(DownloadButton, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(AnalyseButton, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(EventManagerButton, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(OptionsButton, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(HelpButton, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(LogoutButton, Alignment.BOTTOM_LEFT);
		lo.setComponentAlignment(pic_header, Alignment.TOP_RIGHT);	
		lo.setComponentAlignment(FirstLanguageLbl, Alignment.BOTTOM_RIGHT);
		lo.setComponentAlignment(delimiterLbl, Alignment.BOTTOM_CENTER);
		lo.setComponentAlignment(SecondLanguageLbl, Alignment.BOTTOM_LEFT);
		
		this.setContent(lo);
	}
	
	public void setPermisson(int p){
		if(p == 1 ){
			DeviceManagerButton.setVisible(true);
			DownloadButton.setVisible(true);
			AnalyseButton.setVisible(true);
			EventManagerButton.setVisible(true);
			OptionsButton.setVisible(true);
			HelpButton.setVisible(true);
			LogoutButton.setVisible(true);
		}else{
			DeviceManagerButton.setVisible(false);
			DownloadButton.setVisible(false);
			AnalyseButton.setVisible(false);
			EventManagerButton.setVisible(false);
			OptionsButton.setVisible(false);
			HelpButton.setVisible(false);
			LogoutButton.setVisible(false);
		}
	}
	
	// 	Set Button active
	
	private void DeactivateAllButtons(){
		DeviceManagerButton.removeStyleName("active");
		DownloadButton.removeStyleName("active");
		AnalyseButton.removeStyleName("active");
		EventManagerButton.removeStyleName("active");
		OptionsButton.removeStyleName("active");
		HelpButton.removeStyleName("active");
		LogoutButton.removeStyleName("active");
	}
	
	public void setDeviceManagerButtonActive(){
		DeactivateAllButtons();
		DeviceManagerButton.addStyleName("active");
	}
	
	public void setDownloadButtonActive(){
		DeactivateAllButtons();
		DownloadButton.addStyleName("active");
	}
	
	public void setAnalyseButttonActive(){
		DeactivateAllButtons();
		AnalyseButton.addStyleName("active");
	}
	
	public void setEventManagerButttonActive(){
		DeactivateAllButtons();
		EventManagerButton.addStyleName("active");
	}
	
	public void setOptionsButtonActive(){
		DeactivateAllButtons();
		OptionsButton.addStyleName("active");
	}

	public void setHelpButttonActive(){
		DeactivateAllButtons();
		HelpButton.addStyleName("active");
	}
	
	// Button Handler
	
	private void ButtonClickHandler(ClickEvent event){
		Button srcButton = event.getButton();
		if(srcButton == DeviceManagerButton){
			ButtonDeviceManagerClicked();
		}else if(srcButton == DownloadButton){
			ButtonDownloadClicked();
		}else if(srcButton == AnalyseButton){
			//ButtonAnalyseClicked();
		}else if(srcButton == OptionsButton){
			ButtonOptionsClicked();
		}else if(srcButton == HelpButton){
			//ButtonHelpClicked();
		}else if(srcButton == LogoutButton){
			ButtonLogoutClicked();
		}else if(srcButton == FirstLanguageLbl){
			app.langHelper.LoadLanguage("lang_en.properties");
			app.Update();
		}else if(srcButton == SecondLanguageLbl){
			app.langHelper.LoadLanguage("lang_de.properties");
			app.Update();
		}
	}
	
	public void Update(){
		DeviceManagerButton.setCaption(app.langHelper.GetString(LanguageHelper.TAB_DEVICEMANAGER));
		DownloadButton.setCaption(app.langHelper.GetString(LanguageHelper.TAB_DOWNLOAD));
		AnalyseButton.setCaption(app.langHelper.GetString(LanguageHelper.TAB_ANALYSE));
		//EventManager.setCaption(app.langHelper.GetString(LanguageHelper.TAB_EVENTMANAGER));
		OptionsButton.setCaption(app.langHelper.GetString(LanguageHelper.TAB_OPTIONS));
		HelpButton.setCaption(app.langHelper.GetString(LanguageHelper.TAB_HELP));
		LogoutButton.setCaption(app.langHelper.GetString(LanguageHelper.TAB_LOGOUT));
	}
	
	private void ButtonDeviceManagerClicked(){
		app.setMainComponent(app.getDeviceManagerScreen());
		app.deviceManagerScreen.Activate();
	}
	
	private void ButtonDownloadClicked(){
		app.setMainComponent(app.getDownloadScreen());
		app.downloadScreen.Activate();
	}
	
	private void ButtonAnalyseClicked(){
		app.setMainComponent(app.getAnalyseScreen());
		app.analyseScreen.Activate();
	}
	
	private void ButtonOptionsClicked(){
		app.setMainComponent(app.getOptionsScreen());
		app.optionsScreen.Activate();
	}
	
	private void ButtonHelpClicked(){
		app.setMainComponent(app.getHelpScreen());
		app.helpScreen.Activate();
	}
	
	private void ButtonLogoutClicked(){
		app.Logout();
	}

}

