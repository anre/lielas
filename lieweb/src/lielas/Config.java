package lielas;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Properties;

public class Config {

	static String appPath = "E:\\Firma\\SVN\\Lielas\\";
	//static String appPath = "E:\\Buzybee\\Lielas\\";
	//static String appPath = "/usr/local/lielas/";

	//static String dbUser = "lielas";
	//static String dbPass = "lielas";
	String dbUser = "postgres";
	String dbPass = "postgres";
	
	String langPath = "lang\\";
	String firstLang = "en.properties";
	String secondLang = "de.properties";
	String langDelimiter = "=";
	String langComment	= "//";
	
	String csvDelimiter = ";";
	String csvFiletypeEnding = ".csv";
	
	String sixLowPanPrefix = "";
	String sixLowPanGatewayIP = "";
	String sixLowPanUART = "";
	String sixLowPanBaudrate = "";
	String sixLowPanTunnel = "";
	
	public Config(){
		
	}
	
	public void LoadSettings(){
		Properties configFile = new Properties();
		BufferedInputStream stream;
		
		
		try {
			stream = new BufferedInputStream(new FileInputStream(appPath + "config.properties"));
		} catch (Exception e1) {
			e1.printStackTrace();
			return;
		}
		
		try {
			configFile.load(stream);
			stream.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		dbUser = configFile.getProperty("dbUser");
		dbPass = configFile.getProperty("dbPassword");
		
		langPath = configFile.getProperty("languagePath");
		firstLang = configFile.getProperty("firstLanguage");
		secondLang = configFile.getProperty("secondLanguage");
		langDelimiter = configFile.getProperty("languageDelimiter");
		langComment = configFile.getProperty("languageComment");
		
		csvDelimiter = configFile.getProperty("csvDelimiter");
		csvFiletypeEnding = configFile.getProperty("csvFiletypeEnding");
		
		sixLowPanPrefix = configFile.getProperty("6LowPanPrefix");
		sixLowPanGatewayIP = configFile.getProperty("6LowPanGwIp");
		sixLowPanUART = configFile.getProperty("6LowPanUart");
		sixLowPanBaudrate = configFile.getProperty("6LowPanBaudrate");
		sixLowPanTunnel = configFile.getProperty("6LowPanTun");
		
	}
	
	public void SaveSettings(){
		Properties configFile = new Properties();
		try{
			configFile.store(new FileOutputStream("config.properties"), null);
		} catch ( Exception e){
			ExceptionHandler.HandleException(e);
		}
		
	}

	public String getDbUser() {
		return dbUser;
	}

	public void setDbUser(String dbUser) {
		this.dbUser = dbUser;
	}

	public String getDbPass() {
		return dbPass;
	}

	public void setDbPass(String dbPass) {
		this.dbPass = dbPass;
	}

	public String getLangPath() {
		return langPath;
	}

	public void setLangPath(String langPath) {
		this.langPath = langPath;
	}

	public String getFirstLang() {
		return firstLang;
	}

	public void setFirstLang(String firstLang) {
		this.firstLang = firstLang;
	}

	public String getSecondLang() {
		return secondLang;
	}

	public void setSecondLang(String secondLang) {
		this.secondLang = secondLang;
	}

	public String getLangDelimiter() {
		return langDelimiter;
	}

	public void setLangDelimiter(String langDelimiter) {
		this.langDelimiter = langDelimiter;
	}

	public String getLangComment() {
		return langComment;
	}

	public void setLangComment(String langComment) {
		this.langComment = langComment;
	}

	public String getCsvDelimiter() {
		return csvDelimiter;
	}

	public void setCsvDelimiter(String csvDelimiter) {
		this.csvDelimiter = csvDelimiter;
	}

	public String getCsvFiletypeEnding() {
		return csvFiletypeEnding;
	}

	public void setCsvFiletypeEnding(String csvFiletypeEnding) {
		this.csvFiletypeEnding = csvFiletypeEnding;
	}

	public String getSixLowPanPrefix() {
		return sixLowPanPrefix;
	}

	public void setSixLowPanPrefix(String sixLowPanPrefix) {
		this.sixLowPanPrefix = sixLowPanPrefix;
	}

	public String getSixLowPanGatewayIP() {
		return sixLowPanGatewayIP;
	}

	public void setSixLowPanGatewayIP(String sixLowPanGatewayIP) {
		this.sixLowPanGatewayIP = sixLowPanGatewayIP;
	}

	public String getSixLowPanUART() {
		return sixLowPanUART;
	}

	public void setSixLowPanUART(String sixLowPanUART) {
		this.sixLowPanUART = sixLowPanUART;
	}

	public String getSixLowPanBaudrate() {
		return sixLowPanBaudrate;
	}

	public void setSixLowPanBaudrate(String sixLowPanBaudrate) {
		this.sixLowPanBaudrate = sixLowPanBaudrate;
	}

	public String getSixLowPanTunnel() {
		return sixLowPanTunnel;
	}

	public void setSixLowPanTunnel(String sixLowPanTunnel) {
		this.sixLowPanTunnel = sixLowPanTunnel;
	}
}
