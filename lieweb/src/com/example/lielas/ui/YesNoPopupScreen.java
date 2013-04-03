package com.example.lielas.ui;


import java.util.ArrayList;
import java.util.List;

import com.example.lielas.LielasApplication;
import com.vaadin.terminal.Sizeable;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.Label;
import com.vaadin.ui.NativeButton;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.Window;

@SuppressWarnings("serial")
public class YesNoPopupScreen extends VerticalLayout{
	private Window wnd;
	
	LielasApplication app;
	
	private NativeButton yesButton;
	private NativeButton noButton;
	
	private boolean isOpen = true;
	private boolean yesClicked = false;
	private boolean noClicked = false;
	
	Label textLbl;
	
	private List<PopupClosedListener> listener = new ArrayList<PopupClosedListener>();
	
	public YesNoPopupScreen(LielasApplication app, String caption, String text){
		this.app = app;
		
		wnd = new Window(caption);
		
		wnd.setModal(true);
		wnd.setWidth(460, Sizeable.UNITS_PIXELS);
		wnd.setResizable(false);
		wnd.setBorder(Window.BORDER_NONE);
		wnd.setClosable(false);
				
		textLbl = new Label(text);
		textLbl.setStyleName("popup");
		wnd.addComponent(textLbl);
		
		HorizontalLayout buttonLo = new HorizontalLayout();
		buttonLo.setSizeFull();
		wnd.addComponent(buttonLo);
		
		yesButton = new NativeButton("Yes");
		yesButton.setStyleName("popup");
		yesButton.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event){
				YesButtonClicked(event);
			}
		});
		buttonLo.addComponent(yesButton);
		buttonLo.setComponentAlignment(yesButton, Alignment.MIDDLE_RIGHT);
		buttonLo.setExpandRatio(yesButton, 1.0f);
		
		noButton = new NativeButton("No");
		noButton.setStyleName("popup");
		noButton.addListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event){
				NoButtonClicked(event);
			}
		});
		buttonLo.addComponent(noButton);
		buttonLo.setComponentAlignment(noButton, Alignment.MIDDLE_RIGHT);
		
		app.getMainWindow().addWindow(wnd);
	}
	
	private void YesButtonClicked(ClickEvent e){
		yesClicked = true;
		app.getMainWindow().removeWindow(wnd);
		isOpen = false;
		firePopupClosedEvent();
	}
	
	private void NoButtonClicked(ClickEvent e){
		noClicked = true;
		app.getMainWindow().removeWindow(wnd);
		isOpen = false;
		firePopupClosedEvent();
	}


	public void addListener(PopupClosedListener pl){
		this.listener.add(pl);
	}
	
	private void firePopupClosedEvent(){
		for( PopupClosedListener pl : listener){
			pl.popupClosedEvent(this);
		}
	}
	
	public interface PopupClosedListener
	{
		public void popupClosedEvent(YesNoPopupScreen e);
	}
	
	public boolean isOpen() {
		return isOpen;
	}

	public boolean isYesClicked() {
		return yesClicked;
	}

	public boolean isNoClicked() {
		return noClicked;
	}
}
