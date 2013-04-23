package lielas.ui;

import com.vaadin.ui.VerticalLayout;

@SuppressWarnings("serial")
public class MyLoginForm extends VerticalLayout{
	String username;
	String password;
	String submit;
	
	public MyLoginForm(String usernameCaption, String passwordCaption, String submitCaption){
		this.username = usernameCaption;
		this.password = passwordCaption;
		this.submit = submitCaption;
	}
	
	
	
	
}
