package iscas.dao;

import iscas.entity.User;

import org.bson.types.ObjectId;
import org.mongodb.morphia.Morphia;
import org.mongodb.morphia.dao.BasicDAO;

import com.mongodb.MongoClient;

public class UserDaoImpl extends BasicDAO<User, ObjectId> implements UserDao {
	
	protected UserDaoImpl(MongoClient mongoClient, Morphia morphia,
			String dbName) {
		super(mongoClient, morphia, dbName);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public User createUser(final User user) {
		// TODO Auto-generated method stub
		this.save(user);
		return user;
	}

	@Override
	public void updateUser(User user) {
		// TODO Auto-generated method stub
		this.save(user);
	}

	@Override
	public void deleteUser(ObjectId userId) {
		// TODO Auto-generated method stub
		this.deleteById(userId);
	}

	@Override
	public User findOne(ObjectId userId) {
		// TODO Auto-generated method stub
		return this.get(userId);
	}

	@Override
	public User findByUsername(String username) {
		// TODO Auto-generated method stub
		return this.findOne("username", username);
	}

}
