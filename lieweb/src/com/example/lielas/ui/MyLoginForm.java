package com.example.lielas.ui;

import com.vaadin.terminal.gwt.client.ApplicationConnection;
import com.vaadin.ui.LoginForm;

@SuppressWarnings("serial")
public class MyLoginForm extends LoginForm{
	String username;
	String password;
	String submit;
	
	public MyLoginForm(String usernameCaption, String passwordCaption, String submitCaption){
		this.username = usernameCaption;
		this.password = passwordCaption;
		this.submit = submitCaption;
	}
	
	@Override
	protected byte[] getLoginHTML(){
		String appUri = getApplication().getURL().toString() + getWindow().getName() + "/";
		String x, h, b; // XML header, HTML header and body
		
		   x = "<!DOCTYPE html PUBLIC \"-//W3C//DTD "
				      + "XHTML 1.0 Transitional//EN\" "
				      + "\"http://www.w3.org/TR/xhtml1/"
				      + "DTD/xhtml1-transitional.dtd\">\n";
		   
		   h = "<head><script type='text/javascript'>"
				      + "var setTarget = function() {"
				      + "  var uri = '" + appUri + "loginHandler';"
				      + "  var f = document.getElementById('loginf');"
				      + "  document.forms[0].action = uri;"
				      + "  document.forms[0].username.focus();"
				      + "};"
				      + ""
				      + "var styles = window.parent.document.styleSheets;"
				      + "for(var j = 0; j < styles.length; j++) {\n"
				      + "  if(styles[j].href) {"
				      + "    var stylesheet = document.createElement('link');\n"
				      + "    stylesheet.setAttribute('rel', 'stylesheet');\n"
				      + "    stylesheet.setAttribute('type', 'text/css');\n"
				      + "    stylesheet.setAttribute('href', styles[j].href);\n"
				      + "    document.getElementsByTagName('head')[0]"
				      + "                .appendChild(stylesheet);\n"
				      + "  }"
				      + "}\n"
				      + "function submitOnEnter(e) {"
				      + "  var keycode = e.keyCode || e.which;"
				      + "  if (keycode == 13) {document.forms[0].submit();}"
				      + "}\n"
				      + "</script>"
				      + "</head>";
		   
		    b = "<body onload='setTarget();'"
		    	      + "  style='margin:0;padding:0; background:transparent;'"
		    	      + "  class='"
		    	      + ApplicationConnection.GENERATED_BODY_CLASSNAME + "'>"
		    	      + "<div class='v-app v-app-loginpage'"
		    	      + "     style='background:transparent;'>"
		    	      + "<iframe name='logintarget' style='width:0;height:0;"
		    	      + "border:0;margin:0;padding:0;'></iframe>"
		    	      + "<form id='loginf' target='logintarget'"
		    	      + "      onkeypress='submitOnEnter(event)'"
		    	      + "      method='post'>"
		    	      + "<table>"
		    	      + "<tr><td>" + username + "</td>"
		    	      + "<td><input class='v-textfield' style='display:block;'"
		    	      + "           type='text' name='username'></td></tr>"
		    	      + "<tr><td>" + password + "</td>"
		    	      + "    <td><input class='v-textfield'"
		    	      + "          style='display:block;' type='password'"
		    	      + "          name='password'></td></tr>"
		    	      + "</table>"
		    	      + "<div>"
		    	      + "<div onclick='document.forms[0].submit();'"
		    	      + "     tabindex='0' class='v-button' role='button'>"
		    	      + "<span class='v-button-wrap'>"
		    	      + "<span class='v-button-caption'>"
		    	      + submit + "</span>"
		    	      + "</span></div></div></form></div></body>";
		    return (x + "<html>" + h + b + "</html>").getBytes();
	}
	
	
}
