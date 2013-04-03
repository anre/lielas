package lielas;

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
