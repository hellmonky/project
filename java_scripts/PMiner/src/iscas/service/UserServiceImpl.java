package iscas.service;

import org.bson.types.ObjectId;

import iscas.dao.UserDao;
import iscas.entity.User;

public class UserServiceImpl implements UserService {
	private UserDao userDao;
	
    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }
    
    private PasswordHelper passwordHelper;

    public void setPasswordHelper(PasswordHelper passwordHelper) {
        this.passwordHelper = passwordHelper;
    }
    
	@Override
	public User createUser(User user) {
        //加密密码
        passwordHelper.encryptPassword(user);
        return userDao.createUser(user);
	}

	@Override
	public void changePassword(ObjectId userId, String newPassword) {
        User user =userDao.findOne(userId);
        user.setPassword(newPassword);
        passwordHelper.encryptPassword(user);
        userDao.updateUser(user);
	}

	@Override
	public User findByUsername(String username) {
		 return userDao.findByUsername(username);
	}

}
