package lielas;

import java.io.Serializable;
import java.security.MessageDigest;
import java.sql.*;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import javax.xml.bind.DatatypeConverter;

import lielas.Device;
import lielas.Modul;
import lielas.Channel;
import lielas.Config;
import lielas.ExceptionHandler;


public class SQLHelper implements Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 8744849037419385578L;

	private Connection conn = null;
	
	private String dbUser;
	private String dbPass;
	private String csvDelimiter;
	
	public SQLHelper(){
			
	}
	
	public void Connect(){
		try{
			Class.forName("org.postgresql.Driver");
		}catch (ClassNotFoundException e){
			ExceptionHandler.HandleException(e);
		}
		
		LoadSettings();
		
		String url = "jdbc:postgresql://192.168.0.100:5432/lielas";
		String user = dbUser;
		String password = dbPass;
		
		
		
		try{
			conn = DriverManager.getConnection(url, user, password);
			
			InitDatabase(true);
		} catch(SQLException e){
			ExceptionHandler.HandleException(e);
		}		
		
	}
	
	public void LoadSettings(){
		Config cfg =  new Config();
		cfg.LoadSettings();
		dbUser = cfg.getDbUser();
		dbPass = cfg.getDbPass();
		csvDelimiter = cfg.getCsvDelimiter();
	}
	
	public void InitDatabase(boolean withTestData){
		String createTableStr = null;
		
		
		if( conn == null){
			Connect();
		}
		
		try{
			Statement st = conn.createStatement();
			
			if(!TableExists("devices")){
				createTableStr = "CREATE TABLE devices( ID integer NOT NULL, address text, registered boolean, name text, dev_group text, " +
						"mint text, pint text, aint text, moduls text, PRIMARY KEY(ID))";
				st.executeUpdate(createTableStr);
				
				if(withTestData){
					PreparedStatement pst = conn.prepareStatement("INSERT INTO devices ( id, address, registered, mint, pint, aint, moduls) VALUES ( ?, ?, ?, ?, ? ,?, ?)");
					
					String moduls[] = new String[10];
					moduls[1] = "1";
					
					for( int i=1; i<=1; i++){
						pst.setInt(1, i);
						pst.setString(2,  "aaab::ff:fe00:3");
						pst.setBoolean(3, true);
						pst.setString(4, "60");
						pst.setString(5,  "2");
						pst.setString(6, "60");
						pst.setString(7, moduls[i]);
						pst.executeUpdate();
					}
					pst.close();
				}
			}

			if(!TableExists("channels")){
				createTableStr = "CREATE TABLE channels( ID integer NOT NULL, address text, type text, unit text, name text, dev_group text, PRIMARY KEY(ID))";
				st.executeUpdate(createTableStr);
				
				if(withTestData){
					PreparedStatement pst = conn.prepareStatement("INSERT INTO channels ( id, address, type, unit, name, dev_group) VALUES (? ,?, ?, ?, ?, ?)");
					
					int[] adr = { 0, 1, 2, 3, 1, 1, 2, 1, 2, 3, 4, 1, 2, 1, 2, 3, 4, 1, 2, 1, 1, 1, 2, 1, 2};
					
					String type[] = new String[30];
					int j = 1;
					type[j++] = "SHT_T";
					type[j++] = "SHT_H";
					type[j++] = "LPS_P";
					
					String unit[] = new String[30];
					j = 1;
					unit[j++] = "°C";
					unit[j++] = "%";
					unit[j++] = "mBar";
					
					for( int i=1; i<=j; i++){
						pst.setInt(1, i);
						pst.setString(2, Integer.toString(adr[i]));
						pst.setString(3, type[i]);
						pst.setString(4,  unit[i]);
						pst.setString(5,  "");
						pst.setString(6,  "");
						pst.executeUpdate();
					}
				}
			}

			if(!TableExists("moduls")){
				createTableStr = "CREATE TABLE moduls( ID integer NOT NULL, address text, channels text, mint text, pint text, aint text,PRIMARY KEY(ID))";
				st.executeUpdate(createTableStr);
				
				if(withTestData){
					PreparedStatement pst = conn.prepareStatement("INSERT INTO moduls ( id, address, channels, mint, pint, aint) VALUES ( ?, ?, ?, ?, ?, ?)");
					
					int[] adr = { 0, 0, 1, 2, 0, 1, 0, 1, 0, 1, 2, 0, 1 };
					
					String channels[] = new String[30];
					int j = 1;
					channels[j++] = "1;2;3";
					
					
					for( int i=1; i<=j; i++){
						pst.setInt(1, i);
						pst.setString(2,  Integer.toString(adr[i]));
						pst.setString(3, channels[i]);
						pst.setString(4, "60");
						pst.setString(5, "2");
						pst.setString(6, "60");
						pst.executeUpdate();
					}
					pst.close();
				}
			}
			

			if(!TableExists("groups")){
				createTableStr = "CREATE TABLE groups( ID integer NOT NULL, user_group text, delete_devices boolean, register_devices boolean, download boolean, PRIMARY KEY(ID))";
				st.executeUpdate(createTableStr);
			}
			
			if(!TableExists("users")){
				createTableStr = "CREATE TABLE users( ID integer NOT NULL, login text, \"first name\" text, \"family name\" text, usergroup text, timezone text, password text, PRIMARY KEY(ID))";
				st.executeUpdate(createTableStr);
				
				if(withTestData){
					st.executeUpdate("INSERT INTO users ( id, login, \"first name\", \"family name\", usergroup, " +
									 "timezone, password) VALUES(1, 'demo', '', '', 'admin', '+1', '" + getInternalMD5Hash("demo") + "' )");
				}
			}
			
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
	}
		
	public boolean DeleteDatabase(){
		try{
			Statement st = conn.createStatement();
			
			st.executeUpdate("DROP TABLE devices");
			st.executeUpdate("DROP TABLE moduls");
			st.executeUpdate("DROP TABLE channels");
			st.executeUpdate("DROP TABLE users");
			
			st.close();
			return true;
		} catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return false;
	}
	
	public boolean DeleteData(){
		try{
			Statement st = conn.createStatement();
			
			if(TableExists("data")){
				st.executeUpdate("DROP TABLE data");
			}
			st.close();
			return true;
		} catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return false;
	}
	
	public Device GetDevice(int ID){
		Device d = null;
		String address;
		boolean registered;
		String name;
		String group;
		int mInt = 0;
		int pInt = 0;
		int aInt = 0;
		String supply;
		int id;
		
		String moduls;
		String strModulIDs[];
		
		String channels;
		String strChannelIDs[];
		
		String delimiter = ";";
		
		if( conn == null){
			Connect();
		}
		
		try{
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT id, address, registered, name, dev_group, mint, pint, aint, moduls  FROM devices ORDER BY id");
			
			if(rs.next()){
				while(rs.getInt(1) <= ID){
					if(!rs.next()){
						rs.close();
						st.close();
						return null;
					}
				}

				id = rs.getInt(1);
				address = rs.getString(2);
				registered = rs.getBoolean(3);
				name = rs.getString(4);
				if(name == null){
					name = "";
				}
				group = rs.getString(5);
				if(group == null){
					group = "";
				}
				if(rs.getString(6) != null){
					mInt = Integer.parseInt(rs.getString(6));
				}
				
				if(rs.getString(7) != null){
					pInt = Integer.parseInt(rs.getString(7));
				}
				
				if(rs.getString(8) != null){
					aInt = Integer.parseInt(rs.getString(8));
				}
				
				supply = rs.getString(9);
				moduls = GetDeviceModuls(id);
				
				d = new Device(address, registered, name, group, mInt, pInt, aInt, supply, id);
				
				// get Moduls
				if( moduls != null){
					strModulIDs = moduls.split(delimiter);
					for(int i=0; i<strModulIDs.length; i++){
						Modul m = GetModul(Integer.parseInt(strModulIDs[i]));
						if( m != null){
							m.setAddress(i+1);
							d.addModul(m.getID(), m);
							
							//get Channels
							channels = GetModulChannels(m.getID());
							if(channels != null){
								strChannelIDs = channels.split(delimiter);
								for(int j=0; j < strChannelIDs.length; j++){
									Channel c = GetChannel(Integer.parseInt(strChannelIDs[j]));
									if( c != null){
										d.addChannel(m.getAddress(), c);
									}
								}
							}
							
						}
					}
				}
			}
			rs.close();
			st.close();
			return d;
		} catch (SQLException e){
			ExceptionHandler.HandleException(e);
		}
		return null;
	}
	
	public String GetDeviceModuls( int ID){
		String moduls = null;
		
		try{
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT moduls FROM devices WHERE id = " + ID);
			if(rs.next()){
				moduls = rs.getString(1);
			}
			
			rs.close();
			st.close();
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return moduls;				
	}
	
	public Modul GetModul(int ID){
		Modul m = null;
		
		int id;
		int address = 0;
		int mInt = 60;
		int pInt = 2;
		int aInt = 60;
		
		try{
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT id, address, mint, pint, aint FROM moduls WHERE id = " + ID);
			if(rs.next()){
				id = rs.getInt(1);
				if(rs.getString(2) != null)
					address = Integer.parseInt(rs.getString(2));
				if(rs.getString(3) != null)
					mInt = Integer.parseInt(rs.getString(3));
				if(rs.getString(4) != null)
					pInt = Integer.parseInt(rs.getString(4));
				if(rs.getString(5) != null)
					aInt = Integer.parseInt(rs.getString(5));
				
				m = new Modul(id, mInt, pInt, aInt);
				m.setAddress(address);
			}
			
			rs.close();
			st.close();
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return m;
	}
	
	public String GetModulChannels(int ID){
		String channels = null;
		
		try{
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT channels FROM moduls WHERE id = " + ID);
			if(rs.next()){
				channels = rs.getString(1);
			}
			
			rs.close();
			st.close();
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return channels;		
	}
	
	public Channel GetChannel(int ID){
		Channel c = null;
		
		int id;
		int address;
		String name;
		String group;
		String type;
		String minValue;
		String maxValue;
		String unit;
		
		try{
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT id, address, name, dev_group, type, unit FROM channels WHERE id = " + ID);
			if(rs.next()){
				id = rs.getInt(1);
				address = Integer.parseInt(rs.getString(2));
				name = rs.getString(3);
				group = rs.getString(4);
				type = rs.getString(5);
				minValue = "";
				maxValue= "";
				unit = rs.getString(6);
				
				c = new Channel(id, address, type, unit, minValue, maxValue);
				
				if(name != null){
					c.setName(name);
				}
				
				if(group != null){
					c.setGroup(group);
				}
			}
			
			rs.close();
			st.close();
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return c;
	}
	
	public boolean SaveDevice(Device d){
		String update;
		
		try{
			Statement st = conn.createStatement();
			update = "UPDATE devices SET name = '" + d.getName() + "' WHERE id = " + d.getID();
			st.executeUpdate(update);
			update = "UPDATE devices SET dev_group = '" + d.getGroup() + "' WHERE id = " + d.getID();
			st.executeUpdate(update);
			update = "UPDATE devices SET mint = '" + d.getMeassurementIntervall() + "' WHERE id = " + d.getID();
			st.executeUpdate(update);
			update = "UPDATE devices SET pint = '" + d.getProcessIntervall() + "' WHERE id = " + d.getID();
			st.executeUpdate(update);
			update = "UPDATE devices SET aint = '" + d.getAlarmIntervall() + "' WHERE id = " + d.getID();
			st.executeUpdate(update);		
			st.close();
			
			// update modules
			for(int i = 1; i < (d.getModuls()+1);i++){
				if(!SaveModul(d.getModul(i)))
					return false;
			}
			
		}catch (Exception e){
			ExceptionHandler.HandleException(e);
			return false;
		}
		return true;
	}
	
	public boolean SaveModul(Modul m){
		String update;
		
		try{
			Statement st = conn.createStatement();
			
			update = "UPDATE moduls SET mint = '" + m.getMeassurementIntervall() + "' WHERE id = " + m.getID();
			st.executeUpdate(update);

			update = "UPDATE moduls SET pint = '" + m.getProcessIntervall() + "' WHERE id = " + m.getID();
			st.executeUpdate(update);

			update = "UPDATE moduls SET aint = '" + m.getAlarmIntervall() + "' WHERE id = " + m.getID();
			st.executeUpdate(update);
			
			st.close();
			
			// update channels
			for(int i = 1; i <= m.getChannels(); i++){
				if(!SaveChannel(m.getChannel(i)))
					return false;
			}
			
		}catch (Exception e){
			ExceptionHandler.HandleException(e);
			return false;
		}
		return true;
	}
	
	public boolean SaveChannel(Channel c){
		String update;
		
		try{
			Statement st = conn.createStatement();
			
			update = "UPDATE channels SET name = '" + c.getName() + "' WHERE id = " +  c.getID();
			st.executeUpdate(update);

			update = "UPDATE channels SET dev_group = '" + c.getGroup() + "' WHERE id = " + c.getID();
			st.executeUpdate(update);
			
			st.close();
			
		}catch (Exception e){
			ExceptionHandler.HandleException(e);
			return false;
		}
		return true;
	}

	private boolean TableExists(String tableName){
		
		try{
			DatabaseMetaData dbm = conn.getMetaData();
			ResultSet rs = dbm.getTables(null, null, tableName, null);
			if(rs.next()){
				rs.close();
				return true;
			}
			rs.close();			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}	
		return false;
	}
	
	public void CreateTestData(DeviceContainer dc, int rows){

		String sqlStr;
		String columnOrder = "datetime";
		String valueString = "";
		
		if( conn == null){
			Connect();
		}
		
		try{
			Statement st = conn.createStatement();
			
		
			if(TableExists("data")){
				st.executeUpdate("DROP TABLE data");
			}
			CreateDataTable(dc);
				
			Calendar date = Calendar.getInstance();
			java.text.SimpleDateFormat sdf = new java.text.SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			
			
			// create sql statement column order
			Device d = dc.firstItemId();
			for( int i = 0; i < dc.size(); i++){
				for( int j = 0; j < d.getModuls(); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						columnOrder += ", \"" + d.getAddress() + "." + m.getAddress() + "." + c.getAddress() + "\"";
						valueString += ", '22,5'";
					}
				}
				d = dc.nextItemId(d);
			}
			
			for( int i = 0; i < rows; i++){
				/*for(int j=0; j < dc.size(); j++){
				
				}*/
				
				sqlStr = "INSERT INTO data ( " + columnOrder + " ) VALUES ( '" + sdf.format(date.getTime()) + "' " + valueString + " )";
				st.executeUpdate(sqlStr);
				date.add(Calendar.SECOND, 10);
				
			}
			
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
	}
	
	public void CreateDataTable(DeviceContainer dc){
		if( conn == null){
			Connect();
		}
		
		try{
			Statement st = conn.createStatement();

			if(!TableExists("data")){
				st.executeUpdate("CREATE TABLE data( datetime timestamp NOT NULL, PRIMARY KEY(datetime))");
			}
			
			Device d = dc.firstItemId();
				
			for( int i = 0; i < dc.size(); i++){
				for( int j = 0; j < d.getModuls(); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						st.executeUpdate("ALTER TABLE data ADD COLUMN \"" + d.getAddress() + "." + m.getAddress() + "." + c.getAddress() + "\" text");
					}
				}
				d = dc.nextItemId(d);
			}
			
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}		
	}
	
	public StringBuilder GetDataTable(DeviceContainer dc){

		String tableOrder = "datetime";
		StringBuilder csvStr = new StringBuilder(" ");
		String data;
		ResultSet rs;
		
		if( conn == null){
			Connect();
		}
		
		try{
			Statement st = conn.createStatement();
			
			Device d = dc.firstItemId();
			
			// create sql statement column order
			for( int i = 0; i < dc.size(); i++){
				for( int j = 1; j < (d.getModuls()+1); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						if(!ColumnExists(d.getAddress() + "." + m.getAddress() + "." + c.getAddress() )){
							st.execute("ALTER TABLE data ADD COLUMN \""+ d.getAddress() + "." + m.getAddress() + "." + c.getAddress() + "\" text");
						}
						tableOrder += ", \"" + d.getAddress() + "." + m.getAddress() + "." + c.getAddress() + "\"";
					}
				}
				d = dc.nextItemId(d);
			}
			
			d = dc.firstItemId();
			
			// create first header line
			csvStr.append("Address");
			for( int i = 0; i < dc.size(); i++){
				for( int j = 1; j < (d.getModuls()+1); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						if( j == 1 && k ==1){
							csvStr.append(csvDelimiter + d.getAddress());
						}else{
							csvStr.append(csvDelimiter);
						}
					}
				}
				d = dc.nextItemId(d);
			}	
			
			// create second header line
			csvStr.append("\r\nName");
			d = dc.firstItemId();
			for( int i = 0; i < dc.size(); i++){
				for( int j = 1; j < (d.getModuls()+1); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						csvStr.append(csvDelimiter + " " + c.getName());
					}
				}
				d = dc.nextItemId(d);
			}

			// create third header line
			csvStr.append("\r\nGroup");
			d = dc.firstItemId();
			for( int i = 0; i < dc.size(); i++){
				for( int j = 1; j < (d.getModuls()+1); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						csvStr.append(csvDelimiter + " " + c.getGroup());
					}
				}
				d = dc.nextItemId(d);
			}
			// create fourth header line
			csvStr.append("\r\nModul.Channel Address");
			d = dc.firstItemId();
			for( int i = 0; i < dc.size(); i++){
				for( int j = 1; j < (d.getModuls()+1); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						csvStr.append(csvDelimiter + " " + m.getAddress() + "." + c.getAddress());
					}
				}
				d = dc.nextItemId(d);
			}				


			// create fifth header line
			csvStr.append("\r\nUnit");
			d = dc.firstItemId();
			for( int i = 0; i < dc.size(); i++){
				for( int j = 1; j < (d.getModuls()+1); j++){
					Modul m = d.getModul(j);
					for( int k = 1; k <= m.getChannels(); k++){
						Channel c = m.getChannel(k);
						csvStr.append(csvDelimiter + " " + c.getUnit());
					}
				}
				d = dc.nextItemId(d);
			}		
			
			if(TableExists("data")){
				rs = st.executeQuery("SELECT " + tableOrder + " FROM data ORDER BY datetime ASC");
			}else{
				return csvStr;
			}
		
			
			csvStr.append("\r\n");
			if(!tableOrder.equals("datetime")){	// add data if a data was queried
				ResultSetMetaData rsMetaData = rs.getMetaData();
				int columnCount = rsMetaData.getColumnCount();
				while(rs.next()){
					csvStr.append(rs.getString(1));
					for( int i = 1; i < columnCount; i++){
						data = rs.getString(i + 1);
						if(data == null){
							csvStr.append(csvDelimiter);
						}else{
							csvStr.append(csvDelimiter + data);
						}
						
					}
					csvStr.append( "\r\n");
				}
			}
				
			st.close();
			rs.close();
			return csvStr;
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}		
		
		return null;
	}
	
	public User GetUser(int id){
		User user = null;
		
		try{

			if( conn == null){
				Connect();
			}
			
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT id, login, \"first name\", \"family name\", usergroup, timezone, password  FROM users WHERE id = " + id);
			
			if(rs.next()){
				user = new User(id);
				user.setLogin(rs.getString(2));
				user.setForename(rs.getString(3));
				user.setName(rs.getString(4));
				user.setUsergroup(rs.getString(5));
				user.setTimezone(rs.getString(6));
				user.setPassword(rs.getString(7));
			}
			
			rs.close();
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return user;
	}
	
	public void GetUser(UserContainer uc){
		User user = null;
		try{

			if( conn == null){
				Connect();
			}
			
			Statement st = conn.createStatement();
			ResultSet rs = st.executeQuery("SELECT id, login, \"first name\", \"family name\", usergroup, timezone, password  FROM users ORDER BY id ASC");
			
			while(rs.next()){
				user = new User(rs.getInt(1));
				user.setLogin(rs.getString(2));
				user.setForename(rs.getString(3));
				user.setName(rs.getString(4));
				user.setUsergroup(rs.getString(5));
				user.setTimezone(rs.getString(6));
				user.setPassword(rs.getString(7));
				uc.addItem(user);
			}
			
			rs.close();
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}		
	}
	
	public int CreateUser(){
		int id = 0;
		
		try{
			Statement st = conn.createStatement();
			
			ResultSet rs = st.executeQuery("SELECT id FROM users ORDER BY id DESC LIMIT 1");
			
			if(rs.next()){
				id = rs.getInt(1) + 1;
			}else{
				id = 1;
			}
		
			st.executeUpdate("INSERT INTO users ( id ) VALUES ( " + id + ")");

			rs.close();
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return id;
	}
	
	public int GetNewUserId(){
		int id = 0;
		
		try{
			Statement st = conn.createStatement();
			
			ResultSet rs = st.executeQuery("SELECT id FROM users ORDER BY id DESC LIMIT 1");
			
			if(rs.next()){
				id = rs.getInt(1) + 1;
			}else{
				id = 1;
			}
		
			rs.close();
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		
		return id;
	}
	
	public void SaveUser(User user){
		try{
			Statement st = conn.createStatement();
			if(this.GetUser(user.getID()) == null){
				st.executeUpdate("INSERT INTO users ( id ) VALUES ( " + user.getID() + ")");
			}
			st.execute("UPDATE users SET login = '" + user.getLogin() + "' WHERE id = " + user.getID());
			st.execute("UPDATE users SET \"first name\" = '" + user.getForename() + "' WHERE id = " + user.getID());
			st.execute("UPDATE users SET \"family name\" = '" + user.getName() + "' WHERE id = " + user.getID());
			st.execute("UPDATE users SET timezone = '" + user.getTimezone() + "' WHERE id = " + user.getID());
			st.execute("UPDATE users SET password = '" + user.getPassword() + "' WHERE id = " + user.getID());

			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
	}
	
	public boolean RegisterDevice(Device d){
		Modul m;
		Channel c;
		int i, j;
		String columnName;
		
		try{
			Statement st = conn.createStatement();
			st.execute("UPDATE devices SET registered=true WHERE id=" + d.getID());
			st.close();
			
			for( i = 1; i < 20; i++){
				m = d.getModul(i);
				if( m == null){
					return true;
				}
				for(j = 1; j < 20 ; j++){
					c = m.getChannel(j);
					if(c == null){
						break;
					}
					columnName= d.getAddress() + "." + m.getAddress() + "." + c.getAddress();
					
					if(!ColumnExists(columnName)){
						st = conn.createStatement();
						st.execute("ALTER TABLE data ADD COLUMN \"" + columnName + "\" text");
						st.close();
					}
				}
			}
			
			
			return true;
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return false;
	}
	
	public String GetLastValue(Device d, Modul m, Channel c){
		String value = "";
		Timestamp dt;
		
		try{
			String adr = d.getAddress() + "." + m.getAddress() + "." + c.getAddress();
			if(ColumnExists(adr)){
				Statement st = conn.createStatement();
				ResultSet rs = st.executeQuery("SELECT datetime, \"" + adr + "\" FROM data WHERE  \"" + adr + "\" NOT LIKE '' ORDER BY datetime DESC LIMIT 1");
				if(rs.next()){
					dt = rs.getTimestamp(1);
					value = new SimpleDateFormat("dd.MM.yyyy HH:mm:ss").format(dt) + "\t" + rs.getString(2);
				}
				rs.close();
				st.close();
			}
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}		
		return value;
	}
	
	public boolean DeleteUser(int id){
		
		try{
			Statement st = conn.createStatement();
			
			st.executeUpdate("DELETE FROM users WHERE id = " + id);
			
			st.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
			return false;
		}
		return true;
	}
	
	private boolean ColumnExists(String column){
		ResultSet rs;
		try{
			Statement st = conn.createStatement();
			st = conn.createStatement();
			rs = st.executeQuery("SELECT column_name FROM information_schema.columns WHERE table_name='data' and column_name='" + column + "'");
			if(rs.next()){
				st.close();
				rs.close();
				return true;
			}
			st.close();
			rs.close();
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return false;
	}
	
	private String getInternalMD5Hash(String str){
		String md5 = "";
		MessageDigest md = null;
		
		try{
			md = MessageDigest.getInstance("MD5");
			md.reset();
			md.update(str.getBytes());
			byte[] md5Byte= md.digest();
			md5 = DatatypeConverter.printHexBinary(md5Byte);
			return md5;
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return null;
	}
	
	public static String getMD5Hash(String str){
		String md5 = "";
		MessageDigest md = null;
		
		try{
			md = MessageDigest.getInstance("MD5");
			md.reset();
			md.update(str.getBytes());
			byte[] md5Byte= md.digest();
			md5 = DatatypeConverter.printHexBinary(md5Byte);
			return md5;
			
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return null;
	}
	
	public boolean checkMD5Hash(String s1, String s2){
		if(s1.equals(s2)){
			return true;
		}
		return false;
	}
	
}
