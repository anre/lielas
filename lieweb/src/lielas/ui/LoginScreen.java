package lielas.ui;


import lielas.core.LanguageHelper;

import lielas.LiewebUI;
import lielas.ui.HeaderScreen;

import com.vaadin.event.ShortcutAction.KeyCode;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.TextField;
import com.vaadin.ui.PasswordField;
import com.vaadin.ui.Button;
import com.vaadin.ui.Panel;
import com.vaadin.ui.Button.ClickEvent;
import com.vaadin.ui.Button.ClickListener;
import com.vaadin.ui.CheckBox;
import com.vaadin.ui.FormLayout;
import com.vaadin.ui.NativeSelect;
import com.vaadin.ui.VerticalLayout;
import com.vaadin.ui.Notification;

@SuppressWarnings({ "serial", "unused" })
public class LoginScreen extends VerticalLayout {
	
	private LiewebUI app;
	
	
	private TextField nameTxt;
	private PasswordField passwordTxt;
	private Button loginButton;
	
	public int rights;
	
	public LoginScreen(final LiewebUI app){
		this.app = app;
		
		//setCaption("Login");
		setSizeFull();
		
		VerticalLayout vLo = new VerticalLayout();
		vLo.setWidth(200, Unit.PIXELS);
		vLo.setHeight(140, Unit.PIXELS);
		this.addComponent(vLo);
		this.setComponentAlignment(vLo, Alignment.MIDDLE_CENTER);
		
		nameTxt = new TextField("Login");
		vLo.addComponent(nameTxt);
		
		passwordTxt = new PasswordField("Password");
		vLo.addComponent(passwordTxt);
		
		loginButton = new Button("Login");
		loginButton.setClickShortcut(KeyCode.ENTER);
		vLo.addComponent(loginButton);	
		
		loginButton.addClickListener(new Button.ClickListener() {
			
			@Override
			public void buttonClick(ClickEvent event) {
				LoginButtonPressed();
				
			}
		});
		
	}

	public void Update(){
	
	}
	
	private void LoginButtonPressed(){
		app.Authenticate(this.nameTxt.getValue(), this.nameTxt.getValue());
	}

	
	
}

