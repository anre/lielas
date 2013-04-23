package lielas.ui;

import lielas.LiewebUI;
import lielas.ui.HeaderScreen;
import com.vaadin.ui.Panel;

@SuppressWarnings({ "serial", "unused" })

public class AnalyseScreen extends Panel{

	private LiewebUI app;
	
	public AnalyseScreen(final LiewebUI app){
		this.app = app;
	}
	
	public void Activate(){
		app.headerScreen.setAnalyseButttonActive();
	}
}
