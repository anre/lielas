package com.example.lielas.ui;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.HeaderScreen;
import com.vaadin.ui.Panel;

@SuppressWarnings({ "serial", "unused" })

public class AnalyseScreen extends Panel{

	private LielasApplication app;
	
	public AnalyseScreen(final LielasApplication app){
		this.app = app;
	}
	
	public void Activate(){
		app.headerScreen.setAnalyseButttonActive();
	}
}
