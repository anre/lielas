package com.example.lielas.ui;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.HeaderScreen;
import com.vaadin.ui.Panel;


@SuppressWarnings({ "serial", "unused" })

public class HelpScreen extends Panel {

	LielasApplication app;
	
	public HelpScreen(final LielasApplication app){
		this.app = app;
		Activate();
	}
	
	public void Activate(){
		app.headerScreen.setHelpButttonActive();
	}
	
}
