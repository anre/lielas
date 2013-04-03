package com.example.lielas.ui;


import lielas.LanguageHelper;

import com.example.lielas.LielasApplication;
import com.example.lielas.ui.HeaderScreen;
import com.vaadin.terminal.Sizeable;
import com.vaadin.ui.Alignment;
import com.vaadin.ui.LoginForm.LoginEvent;
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
import com.vaadin.ui.Window.Notification;
import com.vaadin.ui.LoginForm;

@SuppressWarnings({ "serial", "unused" })
public class LoginScreen extends VerticalLayout {
	
	private LielasApplication app;
	
	
	private TextField nameTxt;
	private PasswordField passwordTxt;
	private Button loginButton;
	
	public int rights;
	
	public LoginScreen(final LielasApplication app){
		this.app = app;
		
		//setCaption("Login");
		setSizeFull();
		
		Update();

	
		
	}

	public void Update(){
		this.removeAllComponents();
		
		VerticalLayout vLo = new VerticalLayout();
		vLo.setWidth(200, Sizeable.UNITS_PIXELS);
		vLo.setHeight(200, Sizeable.UNITS_PIXELS);
		this.addComponent(vLo);
		this.setComponentAlignment(vLo, Alignment.MIDDLE_CENTER);
		
		MyLoginForm loginForm = new MyLoginForm(app.langHelper.GetString(LanguageHelper.LOGIN_USERNAME), 
												app.langHelper.GetString(LanguageHelper.LOGIN_PASSWORD), 
												app.langHelper.GetString(LanguageHelper.LOGIN_SUBMIT));
		loginForm.addListener(new MyLoginForm.LoginListener(){

			@Override
			public void onLogin(LoginEvent event) {
				String username = event.getLoginParameter("username");
				String pw = event.getLoginParameter("password");
				app.Authenticate(username, pw);
			}
			
		});
		
		
		vLo.addComponent(loginForm);
		
		
	}
	

	
	
}

