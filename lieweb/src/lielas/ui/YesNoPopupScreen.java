package lielas.ui;


import java.util.ArrayList;
import java.util.List;

import lielas.LiewebUI;

import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.HorizontalLayout;
import com.vaadin.ui.Label;
import com.vaadin.ui.NativeButton;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.Window;

@SuppressWarnings("serial")
public class YesNoPopupScreen extends VerticalLayout{
	private Window wnd;
	
	LiewebUI app;
	
	private NativeButton yesButton;
	private NativeButton noButton;
	
	private boolean isOpen = true;
	private boolean yesClicked = false;
	private boolean noClicked = false;
	
	Label textLbl;
	
	private List<PopupClosedListener> listener = new ArrayList<PopupClosedListener>();
	
	public YesNoPopupScreen(LiewebUI app, String caption, String text){
		this.app = app;
		
		wnd = new Window(caption);
		VerticalLayout layout = new VerticalLayout();
		layout.setSizeFull();
		layout.addStyleName("popup");
		
		wnd.setModal(true);
		wnd.setWidth(460, Unit.PIXELS);
		wnd.setHeight(60,  Unit.PIXELS);
		wnd.setResizable(false);
		wnd.setClosable(false);
		wnd.setContent(layout);
				
		textLbl = new Label(text);
		textLbl.setStyleName("popup");
		layout.addComponent(textLbl);
		
		HorizontalLayout buttonLo = new HorizontalLayout();
		layout.addComponent(buttonLo);
		buttonLo.setWidth(100,  Unit.PERCENTAGE);
		buttonLo.addStyleName("button-layout");
		
		Label spacer = new Label();
		spacer.setWidth(100,  Unit.PERCENTAGE);
		buttonLo.addComponent(spacer);
		buttonLo.setExpandRatio(spacer, 0.8f);
		
		yesButton = new NativeButton("Yes");
		yesButton.setStyleName("popup");
		yesButton.addClickListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event){
				YesButtonClicked(event);
			}
		});
		buttonLo.addComponent(yesButton);
		//buttonLo.setComponentAlignment(yesButton, Alignment.MIDDLE_RIGHT);
		
		noButton = new NativeButton("No");
		noButton.setStyleName("popup");
		noButton.addClickListener(new ClickListener(){
			@Override
			public void buttonClick(ClickEvent event){
				NoButtonClicked(event);
			}
		});
		buttonLo.addComponent(noButton);
		//buttonLo.setComponentAlignment(noButton, Alignment.MIDDLE_RIGHT);
		
		app.addWindow(wnd);
	
	}
	
	private void YesButtonClicked(ClickEvent e){
		yesClicked = true;
		app.removeWindow(wnd);
		isOpen = false;
		firePopupClosedEvent();
	}
	
	private void NoButtonClicked(ClickEvent e){
		noClicked = true;
		app.removeWindow(wnd);
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
