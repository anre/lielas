/*
*
*	Copyright (c) 2013 Andreas Reder
*	Author      : Andreas Reder <andreas.reder@lielas.org>
*	File		: 
*
*	This File is part of lielas, see www.lielas.org for more information.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
package lielas.core;

import java.io.Serializable;
import java.io.*;
import lielas.core.Config;

@SuppressWarnings("serial")
public class LanguageHelper  implements Serializable{
	
	public static String PROGRAM_TITLE = "PROGRAM_TITLE";
	public static String TAB_DEVICEMANAGER = "TAB_DEVICEMANAGER";
	public static String TAB_DOWNLOAD = "TAB_DOWNLOAD";
	public static String TAB_ANALYSE = "TAB_ANALYSE";
	public static String TAB_EVENTMANAGER = "TAB_EVENTMANAGER";
	public static String TAB_OPTIONS = "TAB_OPTIONS";
	public static String TAB_HELP = "TAB_HELP";
	public static String TAB_LOGOUT = "TAB_LOGOUT";
	public static String LOGIN_USERNAME = "LOGIN_USERNAME";
	public static String LOGIN_PASSWORD = "LOGIN_PASSWORD";
	public static String LOGIN_SUBMIT = "LOGIN_SUBMIT";
	public static String DM_TAB_DEVICELIST = "DM_TAB_DEVICELIST";
	public static String DM_TAB_NEWDEVICES = "DM_TAB_NEWDEVICES";
	public static String DM_TABLE_DEVICELIST_CAPTION = "DM_TABLE_DEVICELIST_CAPTION";
	public static String DM_TABLE_DEVICELIST_COL_ADDRESS = "DM_TABLE_DEVICELIST_COL_ADDRESS";
	public static String DM_TABLE_DEVICELIST_COL_DEVICENAME = "DM_TABLE_DEVICELIST_COL_DEVICENAME";
	public static String DM_TABLE_DEVICELIST_COL_DEVICEGROUP = "DM_TABLE_DEVICELIST_COL_DEVICEGROUP";
	public static String DM_TABLE_NEWDEVICES_CAPTION = "DM_TABLE_NEWDEVICES_CAPTION";
	public static String DM_TABLE_NEWDEVICES_COL_ADDRESS = "DM_TABLE_NEWDEVICES_COL_ADDRESS";
	public static String DM_TABLE_DL_DETAILS_CAPTION = "DM_TABLE_DL_DETAILS_CAPTION";
	public static String DM_TABLE_DL_DETAILS_ADDRESS = "DM_TABLE_DL_DETAILS_ADDRESS";
	public static String DM_TABLE_DL_DETAILS_NAME = "DM_TABLE_DL_DETAILS_NAME";
	public static String DM_TABLE_DL_DETAILS_GROUP = "DM_TABLE_DL_DETAILS_GROUP";
	public static String DM_TABLE_DL_DETAILS_MEASSUREMENT_INTERVALL = "DM_TABLE_DL_DETAILS_MEASSUREMENT_INTERVALL";
	public static String DM_TABLE_DL_DETAILS_PROCESS_INTERVALL = "DM_TABLE_DL_DETAILS_PROCESS_INTERVALL";
	public static String DM_TABLE_DL_DETAILS_ALARM_INTERVALL = "DM_TABLE_DL_DETAILS_ALARM_INTERVALL";
	public static String DM_TABLE_DL_DETAILS_SUPPLY = "DM_TABLE_DL_DETAILS_SUPPLY";
	public static String DM_TABLE_DL_DETAILS_MODUL = "DM_TABLE_DL_DETAILS_MODUL";
	public static String DM_TABLE_DL_DETAILS_CHANNEL = "DM_TABLE_DL_DETAILS_CHANNEL";
	public static String DM_TABLE_DL_DETAILS_TYPE = "DM_TABLE_DL_DETAILS_TYPE";
	public static String DM_TABLE_DL_DETAILS_UNIT = "DM_TABLE_DL_DETAILS_UNIT";
	public static String DM_TABLE_DL_DETAILS_MIN_VAL = "DM_TABLE_DL_DETAILS_MIN_VAL";
	public static String DM_TABLE_DL_DETAILS_MAX_VAL = "DM_TABLE_DL_DETAILS_MAX_VAL";
	public static String DM_TABLE_DL_SAVE_BTTN = "DM_TABLE_DL_SAVE_BTTN";
	public static String DM_TABLE_DL_DELETE_BTTN = "DM_TABLE_DL_DELETE_BTTN";
	public static String AT_PINT_OPT_LIFETIME  = "AT_PINT_OPT_LIFETIME";
	public static String AT_PINT_OPT_NORMAL  = "AT_PINT_OPT_NORMAL";
	public static String AT_PINT_OPT_FAST_GATHER  = "AT_PINT_OPT_FAST_GATHER";
	public static String DM_TABLE_NDL_DETAILS_CAPTION = "DM_TABLE_NDL_DETAILS_CAPTION";
	public static String DM_TABLE_NDL_DETAILS_ADDRESS = "DM_TABLE_NDL_DETAILS_ADDRESS";
	public static String DM_TABLE_NDL_DETAILS_NAME = "DM_TABLE_NDL_DETAILS_NAME";
	public static String DM_TABLE_NDL_DETAILS_GROUP = "DM_TABLE_NDL_DETAILS_GROUP";
	public static String DM_TABLE_NDL_DETAILS_MEASSUREMENT_INTERVALL = "DM_TABLE_NDL_DETAILS_MEASSUREMENT_INTERVALL";
	public static String DM_TABLE_NDL_DETAILS_PROCESS_INTERVALL = "DM_TABLE_NDL_DETAILS_PROCESS_INTERVALL";
	public static String DM_TABLE_NDL_DETAILS_ALARM_INTERVALL = "DM_TABLE_NDL_DETAILS_ALARM_INTERVALL";
	public static String DM_TABLE_NDL_DETAILS_SUPPLY = "DM_TABLE_NDL_DETAILS_SUPPLY";
	public static String DM_TABLE_NDL_DETAILS_MODUL = "DM_TABLE_NDL_DETAILS_MODUL";
	public static String DM_TABLE_NDL_DETAILS_CHANNEL = "DM_TABLE_NDL_DETAILS_CHANNEL";
	public static String DM_TABLE_NDL_DETAILS_TYPE = "DM_TABLE_NDL_DETAILS_TYPE";
	public static String DM_TABLE_NDL_DETAILS_UNIT = "DM_TABLE_NDL_DETAILS_UNIT";
	public static String DM_TABLE_NDL_DETAILS_MIN_VAL = "DM_TABLE_NDL_DETAILS_MIN_VAL";
	public static String DM_TABLE_NDL_DETAILS_MAX_VAL = "DM_TABLE_NDL_DETAILS_MAX_VAL";
	public static String DM_TABLE_NDL_CHECK_BTTN = "DM_TABLE_NDL_CHECK_BTTN";
	public static String DL_BTTN_DOWNLOAD = "DL_BTTN_DOWNLOAD";
	public static String DL_TABLE_CAPTION = "DL_TABLE_CAPTION";
	public static String DL_TABLE_COL_ADDRESS = "DL_TABLE_COL_ADDRESS";
	public static String DL_TABLE_COL_MODUL = "DL_TABLE_COL_MODUL";
	public static String DL_TABLE_COL_CHANNEL = "DL_TABLE_COL_CHANNEL";
	public static String DL_TABLE_COL_NAME = "DL_TABLE_COL_NAME";
	public static String DL_TABLE_COL_GROUP = "DL_TABLE_COL_GROUP";
	public static String DL_TABLE_COL_SENSOR = "DL_TABLE_COL_SENSOR";
	public static String DL_CSV_MAC = "DL_CSV_MAC";
	public static String DL_CSV_NAME = "DL_CSV_NAME";
	public static String DL_CSV_GROUP = "DL_CSV_GROUP";
	public static String DL_CSV_MODUL_CHANNEL_ADDRESS = "DL_CSV_MODUL_CHANNEL_ADDRESS";
	public static String DL_CSV_UNIT = "DL_CSV_UNIT";
	public static String DL_HELPTEXT_MULTISELECT = "DL_HELPTEXT_MULTISELECT";
	
	public static String SET_TABSHEET_TAB_GLOBAL = "SET_TABSHEET_TAB_GLOBAL";
	public static String SET_TABSHEET_TAB_USER = "SET_TABSHEET_TAB_USER";
	public static String SET_TABSHEET_TAB_GROUP = "SET_TABSHEET_TAB_GROUP";
	public static String SET_TABSHEET_TAB_UPDATE = "SET_TABSHEET_TAB_UPDATE";
	public static String SET_TABSHEET_TAB_REG = "SET_TABSHEET_TAB_REG";
	
	public static String SET_TABSHEET_TAB_GLOBAL_CLOCK_SETTINGS = "SET_TABSHEET_TAB_GLOBAL_CLOCK_SETTINGS";
	public static String SET_TABSHEET_TAB_GLOBAL_CLOCK_STATE = "SET_TABSHEET_TAB_GLOBAL_CLOCK_STATE";
	public static String SET_TABSHEET_TAB_GLOBAL_DB_SETTINGS = "SET_TABSHEET_TAB_GLOBAL_DB_SETTINGS";
	public static String SET_TABSHEET_TAB_GLOBAL_DB_DELETE = "SET_TABSHEET_TAB_GLOBAL_DB_DELETE";
	public static String SET_TABSHEET_TAB_GLOBAL_DB_DELETE_POP_HEADER = "SET_TABSHEET_TAB_GLOBAL_DB_DELETE_POP_HEADER";
	public static String SET_TABSHEET_TAB_GLOBAL_DB_DELETE_POP_TEXT = "SET_TABSHEET_TAB_GLOBAL_DB_DELETE_POP_TEXT";
	public static String SET_TABSHEET_TAB_GLOBAL_DB_DELETE_BEFORE = "SET_TABSHEET_TAB_GLOBAL_DB_DELETE_BEFORE";
	public static String SET_TABSHEET_TAB_GLOBAL_LANG_SETTINGS = "SET_TABSHEET_TAB_GLOBAL_LANG_SETTINGS";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_SETTINGS = "SET_TABSHEET_TAB_GLOBAL_NETWORK_SETTINGS";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_TYPE = "SET_TABSHEET_TAB_GLOBAL_NETWORK_TYPE";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_IP = "SET_TABSHEET_TAB_GLOBAL_NETWORK_IP";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_NETMASK = "SET_TABSHEET_TAB_GLOBAL_NETWORK_NETMASK";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_GATEWAY = "SET_TABSHEET_TAB_GLOBAL_NETWORK_GATEWAY";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE = "SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE_POP_HEADER = "SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE_POP_HEADER";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE_POP_TEXT = "SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE_POP_TEXT";
	public static String SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE_SUCCESS = "SET_TABSHEET_TAB_GLOBAL_NETWORK_SAVE_SUCCESS";
	public static String SET_TABSHEET_TAB_GLOBAL_6LOWPANGW_SETTINGS = "SET_TABSHEET_TAB_GLOBAL_6LOWPANGW_SETTINGS";
	
	public static String SET_TABSHEET_TAB_USER_TABLE_NAME = "SET_TABSHEET_TAB_USER_TABLE_NAME";
	public static String SET_TABSHEET_TAB_USER_TABLE_COL_USER = "SET_TABSHEET_TAB_USER_TABLE_COL_USER";
	public static String SET_TABSHEET_TAB_USER_TABLE_COL_GROUP = "SET_TABSHEET_TAB_USER_TABLE_COL_GROUP";
	public static String SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE = "SET_TABSHEET_TAB_USER_TABLE_COL_TIMEZONE";
	public static String SET_TABSHEET_TAB_USER_DETAILS_CAPTION = "SET_TABSHEET_TAB_USER_DETAILS_CAPTION";
	public static String SET_TABSHEET_TAB_USER_DETAILS_BTTN_NEW_USER = "SET_TABSHEET_TAB_USER_DETAILS_BTTN_NEW_USER";
	public static String SET_TABSHEET_TAB_USER_DETAILS_BTTN_SAVE_USER = "SET_TABSHEET_TAB_USER_DETAILS_BTTN_SAVE_USER";
	public static String SET_TABSHEET_TAB_USER_DETAILS_BTTN_DELETE_USER = "SET_TABSHEET_TAB_USER_DETAILS_BTTN_DELETE_USER";
	public static String SET_TABSHEET_TAB_USER_DETAILS_LOGIN_LBL = "SET_TABSHEET_TAB_USER_DETAILS_LOGIN_LBL";
	public static String SET_TABSHEET_TAB_USER_DETAILS_FORENAME_LBL = "SET_TABSHEET_TAB_USER_DETAILS_FORENAME_LBL";
	public static String SET_TABSHEET_TAB_USER_DETAILS_NAME_LBL = "SET_TABSHEET_TAB_USER_DETAILS_NAME_LBL";
	public static String SET_TABSHEET_TAB_USER_DETAILS_TIMEZONE_LBL = "SET_TABSHEET_TAB_USER_DETAILS_TIMEZONE_LBL";
	public static String SET_TABSHEET_TAB_USER_DETAILS_PASSWORD_LBL = "SET_TABSHEET_TAB_USER_DETAILS_PASSWORD_LBL";
	public static String SET_TABSHEET_TAB_USER_DETAILS_REPEAT_PASSWORD_LBL = "SET_TABSHEET_TAB_USER_DETAILS_REPEAT_PASSWORD_LBL";
	public static String SET_TABSHEET_TAB_USER_DETAILS_POPUP_CAPTION = "SET_TABSHEET_TAB_USER_DETAILS_POPUP_CAPTION";
	public static String SET_TABSHEET_TAB_USER_DETAILS_POPUP_TEXT = "SET_TABSHEET_TAB_USER_DETAILS_POPUP_TEXT";
	
	public static String SET_TABSHEET_TAB_GROUP_TABLE_NAME = "SET_TABSHEET_TAB_GROUP_TABLE_NAME";
	public static String SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP = "SET_TABSHEET_TAB_GROUP_TABLE_COL_GROUP";
	public static String SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES = "SET_TABSHEET_TAB_GROUP_TABLE_COL_DEL_DEVICES";
	public static String SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES = "SET_TABSHEET_TAB_GROUP_TABLE_COL_REG_DEVICES";
	public static String SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD = "SET_TABSHEET_TAB_GROUP_TABLE_COL_DOWNLOAD";
	public static String SET_TABSHEET_TAB_GROUP_DETAILS_CAPTION = "SET_TABSHEET_TAB_GROUP_DETAILS_CAPTION";
	
	public static String SET_TABSHEET_TAB_REG_CAPTION = "SET_TABSHEET_TAB_REG_CAPTION";
	public static String SET_TABSHEET_TAB_REG_MINT_LBL = "SET_TABSHEET_TAB_REG_MINT_LBL";
	public static String SET_TABSHEET_TAB_REG_PINT_LBL = "SET_TABSHEET_TAB_REG_PINT_LBL";
	public static String SET_TABSHEET_TAB_REG_AINT_LBL = "SET_TABSHEET_TAB_REG_AINT_LBL";
	
	public static String POPUP_NO = "POPUP_NO";
	public static String POPUP_YES = "POPUP_YES";
	
	
	//public static String  = "";
	
	private String path = "";
	private String langComment = "";
	private String langDelimiter = "";
	
	private String[][] token;
	private String[][] oldToken;
	private int nrOfTokens;	
	
	public LanguageHelper(String fileName){
		token = new String[1000][2];
		oldToken = new String[1000][2];
		
		Config cfg = new Config();
		cfg.LoadSettings();
		path = Config.appPath + cfg.getLangPath();
		langComment = cfg.getLangComment();
		langDelimiter = cfg.getLangDelimiter();
		
		LoadLanguage(fileName);
	}
		
	public void LoadLanguage(String fileName){
		try{
			File file = new File(path + fileName);
			BufferedReader br = new BufferedReader(
					new InputStreamReader(
							new FileInputStream(file), "UTF8"));
			String line;
			String[] tmp;
			String[][] tmpTokens= new String[1000][2];
			boolean error = false;
			int len;
			
			line = br.readLine();
			line = line.replaceAll("[\uFEFF-\uFFFF]", ""); 
			for(len=0;line != null && error == false; len++){
				if(line.startsWith(langComment) || line.isEmpty()){
					// comment line or empty line, do nothing
				}else{
					tmp = line.split(langDelimiter);
					if(tmp.length == 2){
						for(int i=0; i< tmp.length; i++){
							tmpTokens[len][i] = tmp[i];
						}
					}else{
						error = true;
					}
				}
				line = br.readLine();
			}
			
			if(error == false){
				for(nrOfTokens=0; nrOfTokens<len;nrOfTokens++){
					oldToken[nrOfTokens][0] = token[nrOfTokens][0];
					oldToken[nrOfTokens][1] = token[nrOfTokens][1];
					
					token[nrOfTokens][0]=tmpTokens[nrOfTokens][0];
					token[nrOfTokens][1]=tmpTokens[nrOfTokens][1];
				}
			}
			br.close();
		} catch (Exception e){
			e.printStackTrace();
		}
	}
	
	public String GetString(String description){
		for( int i = 0; i < nrOfTokens; i++){
			if(description.equalsIgnoreCase(token[i][0])){
				if(token[i][1] != null){
					return token[i][1];
				}
				return "";
			}
		}
		return "";
	}
	
	public String GetOldString(String description){
		for( int i = 0; i < nrOfTokens; i++){
			if(description.equalsIgnoreCase(oldToken[i][0])){
				if(oldToken[i][1] != null){
					return oldToken[i][1];
				}
				return "";
			}
		}
		return "";
	}
	
	
}
