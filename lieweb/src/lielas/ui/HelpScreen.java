package lielas.ui;

import lielas.LiewebUI;
import lielas.ui.HeaderScreen;
import com.vaadin.ui.Panel;


@SuppressWarnings({ "serial", "unused" })

public class HelpScreen extends Panel {

	LiewebUI app;
	
	public HelpScreen(final LiewebUI app){
		this.app = app;
		Activate();
	}
	
	public void Activate(){
		app.headerScreen.setHelpButttonActive();
	}
	
}
