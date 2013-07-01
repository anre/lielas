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
import com.vaadin.data.util.BeanItemContainer;

@SuppressWarnings("serial")
public class UserContainer extends BeanItemContainer<User> implements Serializable{

	private static SQLHelper sql = null;
	
	public UserContainer() throws InstantiationException, IllegalAccessException{
		super(User.class);
	}
	
	public static UserContainer loadUsers(SQLHelper sqlHelper){
		UserContainer uc = null;
		
		sql = sqlHelper;
		
		try{
			uc = new UserContainer();
			sql.GetUser(uc);
		}catch(Exception e){
			ExceptionHandler.HandleException(e);
		}
		return uc;
	}
	
/*	public static User GetUserByLogin(String login){
		User user = null;
		
		for(int i = 0; i < this.size(); i++){
			user = UserContainer.getIdByIndex(i);
			if(user.getLogin().equals(login)){
				return user;
			}
		}
		return user;
	}
	
	public static User GetUserById(int id){
		User user = null;
		
		for(int i = 0; i < this.size(); i++){
			user = this.getIdByIndex(i);
			if(user.getID() == id){
				return user;
			}
		}
		return user;	
	}*/
	
}
