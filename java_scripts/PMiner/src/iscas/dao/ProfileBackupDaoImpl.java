package iscas.dao;

import iscas.entity.Profile;

import org.mongodb.morphia.Key;
import org.mongodb.morphia.Morphia;
import org.mongodb.morphia.dao.BasicDAO;
import com.mongodb.MongoClient;

public class ProfileBackupDaoImpl extends BasicDAO<Profile, String> implements ProfileBackupDao {

	protected ProfileBackupDaoImpl(MongoClient mongoClient, Morphia morphia,
			String dbName) {
		super(mongoClient, morphia, dbName);
		// TODO Auto-generated constructor stub
	}

	@Override
	public String saveOrModify(Profile profile) {
		// returns the Id of profile
		Key<Profile> result = this.save(profile);
		return (String) result.getId();
	}
}
